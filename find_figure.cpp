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
	
	//аккумулятор для поиска окружностей
	this->accum4circle = cvCreateImage(imgSize, 8, maxR-minR+1);
	
	//хранилище для контуров
	this->storage=cvCreateMemStorage(0);
	}
//==============================================================================

//destructor
//==============================================================================
mycv::FindFigure::~FindFigure()
	{
	cvReleaseMemStorage(&this->storage);
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
							2.0);						//точность

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
	
	//1. обнуление аккумулятора
	cvZero(this->accum4circle);

#ifdef DEBUG
	t1=clock();
#endif	

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
			
			mycv::drawLineB(this->accum4circle, &begin, &end, 1);
			
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
		
	//3. Уточнение окружностей
	int threshold = 5;					//вес точки необходимый для того чтобы ее можно было считать потенциальным центром 
	int value;							//вес текущей точки 
	CvPoint pt;							//точка в поле потенциальных прямых
	
	for(pt.x=0; pt.x<this->accum4circle->width; pt.x++)
		{
		for(pt.y=0; pt.y<this->accum4circle->height; pt.y++)
			{
			value = PIXEL(uchar, this->accum4circle, pt.x, pt.y)[0];
			//если нашли потенциальный центр
			if(value>=threshold)
				{
//				//находим прямогольник в котором будет вестись поиск
//				CvPoint begin = cvPoint( std::max( pt.x - this->maxRadius, 0 ),
//									 std::max( pt.y - this->maxRadius, 0 ) );
//				CvPoint end = cvPoint( std::min( pt.x + this->maxRadius, this->accum4circle->width-1 ),
//										std::min( pt.y + this->maxRadius, this->accum4circle->height-1 ) );
//				
//				//для каждой точки в выбраном прямоугольнике
//				for(int i=begin.x; i!=end.x; i++)
//					{
//					for(int j=begin.y; j!=end.y; j++)
//						{
//						//если точка принадлежит контуру
//						if(PIXEL(uchar, src, i, j)[0]==255)
//							{
//							//расстояния до выбранной точки
//							int r = sqrt( pow( abs(i-pt.x), 2 ) + pow( abs(j-pt.y), 2 ) );
//							//если точка находится в нужном диапозоне
//							if(r>=this->minRadius && r<=this->maxRadius)
//								{
//								PIXEL(uchar, this->accum4circle, pt.x, pt.y)[r-this->minRadius+1]++;
//								}
//							}
//						}
//					}	
				}
			}
		}
			
#ifdef DEBUG
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::FindFigure::findCircle]: окружности найдены (" << t3 << " ms)\n";
#endif
	
	//ище максимумы в массиве
//	int steps_r = this->maxRadius-this->minRadius;
//	int maxValue=0;
//	int maxR;
//	CvPoint maxC;
//	for(int x=0; x<this->accum4circle->width; x++)
//		{
//		for(int y=0; y<this->accum4circle->height; y++)
//			{
//			for(int r=0; r<steps_r; r++)
//				{
//				value = PIXEL(uchar, this->accum4circle, x, y)[r+1];
//				if(maxValue<value)
//					{
//					maxC=cvPoint(x, y);
//					maxR=r;
//					maxValue=value;
//					}
//				}
//			}
//		}
//	
//	IplImage* draw = cvCreateImage(cvGetSize(this->accum4circle), 8, 3);
//	cvCircle(draw, maxC, maxR+this->minRadius, CV_RGB(255, 0, 0));
//	for(int x=0; x<draw->width; x++)
//		{
//		for(int y=0; y<draw->height; y++)
//			{
//			PIXEL(uchar, draw, x, y)[0]=PIXEL(uchar, this->accum4circle, x, y)[1+maxR];
//			}
//		}
//	cvShowImage("draw", draw);
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
void mycv::drawLineB(IplImage* src, CvPoint* begin, CvPoint* end, char ty)
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
			if(x>=0 && x<src->width && y>=0 && y<src->height)
				PIXEL(uchar, src, x, y)[0]+=ty;
			}
		else 
			{
			if(x>=0 && x<src->height && y>=0 && y<src->width)
				PIXEL(uchar, src, y, x)[0]+=ty;
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
