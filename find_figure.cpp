#include "find_figure.h"

//constructor
//==============================================================================
mycv::FindFigure::FindFigure(CvSize imgSize, int minR, int maxR)
	{	
	//параметры поиска окружностей
	if(minR>=maxR)
		{
		std::cout << "[mycv::FindFigure::FindFigure]: minR>=maxR\n";
		return;
		}
	this->minRadius = minR;
	this->maxRadius = maxR;
	
	this->accApprox=1.5; //точность аппроксимирования
	this->stepRadius = (maxR+accApprox);
	
	//аккумулятор для поиска окружностей
	this->accum4circle = cvCreateImage(imgSize, 8, 1);
	this->rAccum4circle.init(this->stepRadius+1);
	this->members_circle.init(imgSize.width, imgSize.height);
	}
//==============================================================================

//destructor
//==============================================================================
mycv::FindFigure::~FindFigure()
	{
	cvReleaseImage(&this->accum4circle);
	}
//==============================================================================


//find line
//ПРИМЕЧАНИЕ: Входное изображение - контурное.
//==============================================================================
int mycv::FindFigure::findLine(IplImage* src,
								 std::vector<mycv::Line>* lines,
								 mycv::ContourStorage* contours)
	{
	long t1, t2;
	float t3;
	
	Line line;			//прямая
	
	//mycv::ContourStorage contours;	//найденные контуры
	mycv::ContourStorageIt contourIt;	//указатель на заданный контур
	mycv::DominantPoints dominantPoints;	//доминирующие точки
	
#ifdef DEBUG
	t1=clock();
#endif	
	
	//1. копируем исходный контур
	IplImage* w_img = cvCloneImage(src); 
	
	//2. находим все контуры на изображении
	mycv::findContours(w_img, contours);
	
	//для каждого контура
	for(contourIt=contours->begin(); contourIt!=contours->end(); ++contourIt) {
	
	//3. Аппроксимирование контура (поиск доминирующих точек)
		dominantPoints.clear();
		dominantPoints.push_back(contourIt->begin());		//конечная точка
		dominantPoints.push_back(contourIt->end());	//начальная точка контура
		mycv::DominantPointsIt dIt=dominantPoints.end();
		--dIt;
		mycv::approxContoursDp(&(*contourIt),			//контур
							&dominantPoints,			//точки 
							dominantPoints.begin(),		//начало прямой
							dIt,		//конец прямой
							this->accApprox);						//точность

	//4. Записываем параметры прямых
		mycv::DominantPointsIt endIt = dominantPoints.end();
		--endIt;
		--endIt;
		for(mycv::DominantPointsIt it = dominantPoints.begin();
			it != endIt; ++it) 
			{
			mycv::DominantPointsIt it2 = it;
			++it2;
			
			//записываем координаты концов прямой
			line.begin.x=(*it)->x;
			line.begin.y=(*it)->y;
			line.end.x=(*it2)->x;
			line.end.y=(*it2)->y;
			
			//записываем указатели на точки контура
			line.contourBegin=*it;
			line.contourEnd=*it2;
			
			//вычисляем параметры прямой
			mycv::lineABC(line.begin, line.end, &line.a, &line.b, &line.c);
			
			//вес прямой
			//(пока квадрат длины прямой)
			line.weight = pow(line.begin.x-line.end.x, 2) + pow(line.begin.y-line.end.y, 2);
			
			//записываем прямую в вектор
			lines->push_back(line); 
			}	
		}

#ifdef DEBUG
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::FindFigure::findLine]: Прямые найдены (" << t3 << " ms)\n";
#endif
				
	return 0;
	}
//==============================================================================


//NAME: find circle
//ПРИМЕЧАНИЕ: 
//==============================================================================
int mycv::FindFigure::findCircle(IplImage* src,
								 mycv::ContourStorage* contours,
								 std::vector<mycv::Line>* lines,
								 std::vector<mycv::Circle>* circle)
	{
	long t1, t2;
	float t3;

#ifdef DEBUG
	t1=clock();
#endif	

	//1. обнуление аккумулятора потенциальных центров
	cvZero(this->accum4circle); 
	
	//2. находим потенциальные центры окружностей
	//для каждой прямой на изображении

	for(std::vector<mycv::Line>::iterator i=lines->begin(); i!=lines->end(); ++i)
		{
		//вычисляем прямые нормальные к касательным
		
		CvPoint pt;	//точка середины касательной
		pt.x = i->begin.x+(i->end.x - i->begin.x)/2;
		pt.y = i->begin.y+(i->end.y - i->begin.y)/2;
		
		float vector = sqrt(pow((float)(i->a), 2) + pow((float)(i->b), 2));	//длинна вектора
		
		CvPoint begin;
		CvPoint end;
		
		//рисуем прямые в аккумуляторном массиве
		for(int k=0; k<2; k++)
			{
			begin.x=pt.x+i->a*(float)(this->minRadius)/vector; 
			begin.y=pt.y+i->b*(float)(this->minRadius)/vector;
			
			end.x=pt.x+i->a*(float)(this->maxRadius)/vector; //x0+a*step
			end.y=pt.y+i->b*(float)(this->maxRadius)/vector;
			
			mycv::drawLineB(this->accum4circle,
							&this->members_circle,
							&begin, &end, 1, i);
			
			i->a *= -1;
			i->b *= -1;
			}
		}

#ifdef DEBUG
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::FindFigure::findCircle]: Потенциальные центры окружностей найдены (" << t3 << " ms)\n";
#endif
		
#ifdef DEBUG
	t1=clock();
#endif	
	
	//(DEBUG)++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	IplImage* w = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(w);
	for(int x=0; x<src->width; x++)
		{
		for(int y=0; y<src->height; y++)
			{
			PIXEL(uchar, w, x, y)[0] = PIXEL(uchar, this->accum4circle, x, y)[0]*30;
			}
		}
	cvShowImage("draw", w);
	//(DEBUG)++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//3. Уточнение окружностей
	int thresholdCenter = 2;					//вес точки необходимый для того чтобы ее можно было считать потенциальным центром 
	float thresholdRadius = 31.4/3;				
	int valueCenter;							//вес текущей точки 
	CvPoint ptCenter;							//точка в поле потенциальных прямых
	int lenContour=0;							//длинна контура аппроксимированного прямой
	ContourSegment contour_segment;
	mycv::Circle _circle;					//окружность
	
	//для каждого возможного центра
	for(ptCenter.x=0; ptCenter.x<this->accum4circle->width; ptCenter.x++)
		{
		for(ptCenter.y=0; ptCenter.y<this->accum4circle->height; ptCenter.y++)
			{
			valueCenter = PIXEL(uchar, this->accum4circle, ptCenter.x, ptCenter.y)[0];
			
			//если нашли потенциальный центр
			if(valueCenter>=thresholdCenter)
				{
				
				//сбрасываем аккумулятор радиуса
				for(int i=0; i<this->stepRadius; i++) 
					{
					this->rAccum4circle.value[i]=0;
					}
				
				int max_value_r=0;
				int max_number_r=0;
				
				_circle.segments.clear();
				
				//для каждой прямой которая проголосовала за данный центр
				for(std::vector<std::vector<mycv::Line>::iterator>::iterator it = this->members_circle.value[ptCenter.x][ptCenter.y].begin();
					it!=this->members_circle.value[ptCenter.x][ptCenter.y].end(); ++it)
					{
					//записываем сегмент
					contour_segment.begin=(*it)->contourBegin;	
					contour_segment.end=(*it)->contourEnd;
					_circle.segments.push_back(contour_segment);
						
					//для каждой точки контура который аппроксимирует данная прямая
					//lenContour=(*it)->contourEnd - (*it)->contourBegin; //вычисляем длинну контура
					for(mycv::ContourIt cIt=(*it)->contourBegin;
						cIt!=(*it)->contourEnd; ++cIt)
						{
						//вычисляем расстояние от центра до точки
						float deltaX = abs(cIt->x-ptCenter.x);
						float deltaY = abs(cIt->y-ptCenter.y);
						int r = sqrt( pow( deltaX, 2 ) + pow( deltaY, 2 ) );
						
						//голос за данный радиус
						if(r>=this->minRadius && r<this->stepRadius) 
							{
							this->rAccum4circle.value[r]++;	
							}
						
//						//если за этот радиус проголосовало больше всего точек
						if(this->rAccum4circle.value[r]>max_value_r) {
							max_value_r=this->rAccum4circle.value[r];
							max_number_r=r;
							}	
						}	
					}
					
					
				//Высчитываем вес окружноости
				float weight;	
				if(max_number_r>0)
					{	
					weight = max_value_r * 10 / max_number_r;
					}
				else
					{
					std::cout << "max_number_r=0\n";
					return -1;
					}

//(DEBUG)++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++				
//				if(weight>=50)
//					{
//					std::cout << "L=" << max_value_r << std::endl;
//					std::cout << "R=" << max_number_r << std::endl;
//					std::cout << "value=" << valueCenter << std::endl;
//					
//					
//					_circle.center.x = ptCenter.x;
//					_circle.center.y = ptCenter.y;
//					_circle.radius = max_number_r;
//					_circle.weight = weight;
//					circle->push_back(_circle);
//					cvShowImage("roi", roi);
//					IplImage* dst = cvCreateImage(cvGetSize(src), 8, 3);
//					cvZero(dst);
//					for(std::vector<ContourSegment>::iterator i = _circle.segments.begin();
//						i!=_circle.segments.end(); ++i)
//						{
//						cvLine(dst, (*i->begin), (*i->end), CV_RGB(255, 255, 255));
//						}
//					cvShowImage("dst", dst);
//					return 1;
//					}
//(DEBUG)++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				
				//если набран минимальный вес
				if(thresholdRadius <= weight) 
					{
					_circle.center.x = ptCenter.x;
					_circle.center.y = ptCenter.y;
					_circle.radius = max_number_r;
					_circle.weight = weight;
					circle->push_back(_circle);
					}
				}
			}
		}
			
#ifdef DEBUG
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::FindFigure::findCircle]: окружности найдены (" << t3 << " ms)\n";
#endif

	return 0;
	}
//==============================================================================


//отрисовка прямой
//==============================================================================
void mycv::drawLine(IplImage* src, float a, float b, float c)
	{
	CvPoint begin;
	CvPoint end;
	
	if(b!=0)
		{
		begin.x=0;
		begin.y=(a*begin.x+c)/(-1.0*b);
	
		end.x=src->width-1;
		end.y=(a*end.x+c)/(-1.0*b);
		}
	else
		{
		begin.x=abs(c);
		begin.y=0;
		
		end.x=abs(c);
		end.y=src->height;
		}
	cvLine(src, begin, end, CV_RGB(255, 255, 255));	
	}
//==============================================================================

//Алгоритм Брезенхема
//==============================================================================
void mycv::drawLineB(IplImage* src,
					mycv::Arr2D< std::vector< std::vector<mycv::Line>::iterator > >* accum,
					CvPoint* begin, CvPoint* end, char ty, std::vector<mycv::Line>::iterator i)
	{
	bool step = abs(begin->x - end->x) < abs(begin->y - end->y);	//угол больше 45град
	
	//если угол больше 45 град
	if(step)
		{
		std::swap(begin->x, begin->y);
		std::swap(end->x, end->y);
		}
	
	//правильно размещаем начало и конец отрезка
	if(begin->x > end->x)
		{
		std::swap(begin->x, end->x);
		std::swap(begin->y, end->y);
		}
		
	int deltaX = abs(end->x-begin->x);
	int deltaY = abs(begin->y-end->y);
	float k = (float)(deltaY)/(float)(deltaX);
	int stepY;
	
	if(begin->y > end->y)
		{
		stepY=-1;
		}
	else
		{
		stepY=1;
		}

	float error=0;
	int y=begin->y;
	
	for(int x=begin->x; x<end->x; x++)
		{
		if(!step) 
			{
			if(x>=0 && x<src->width && y>=0 && y<src->height) {
				PIXEL(uchar, src, x, y)[0]+=ty;
				accum->value[x][y].push_back(i);
				}
				
			}
		else 
			{
			if(x>=0 && x<src->height && y>=0 && y<src->width) {
				PIXEL(uchar, src, y, x)[0]+=ty;
				accum->value[y][x].push_back(i);
				}
			}
			
		error+=k;
		if(error>=0.5)
			{
			y+=stepY;	//pixel(x+1, y+1)
			error-=1;
			}
		//pixel(x+1, y)
		}
	}
//==============================================================================

/*End of file*/
