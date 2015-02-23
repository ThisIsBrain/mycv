#include "hough_transform.h"


//constructor
//==============================================================================
mycv::HoughTransform::HoughTransform(CvSize sizeImg, int minRadius, int maxRadius)
	{
	long t1, t2;
	float t3;
	
	this->sizeImg = sizeImg;	//размеры исходного изображения
	this->lenDiagonal = cvRound( sqrt( (double)(pow( sizeImg.width, 2 ) + pow( sizeImg.height, 2 ) ) ) );	
	
#ifdef DEBUG
	std::cout << "[mycv::HoughTransform::HoughTransform]: size img " << sizeImg.width << " x " << sizeImg.height << std::endl;
#endif

	//line
	//======================
	//аккумулятор
	this->accumulator4line = cvCreateImage(cvSize(180+1, this->lenDiagonal*2), IPL_DEPTH_8U, 1);
	
	//все значения акккумуляторного массива для линий
	this->allValue4line.init(sizeImg.width, sizeImg.height, 180+1);
	
	//рассчет параметров для каждой точки изображения
	calcAllVal4Line();
	
#ifdef DEBUG
	std::cout << "[mycv::HoughTransform::HoughTransform]: Аккумулятор для прямых рассчитан\n";
#endif
	
	//circle
	//======================	
	this->maxRadius=maxRadius;
	this->minRadius=minRadius;
	this->amountRadius=maxRadius-minRadius;		//
	
	//аккумулятор
	this->accumulator4circle = cvCreateImage(sizeImg, IPL_DEPTH_16U, amountRadius);
	
	//все значения акккумуляторного массива для окружностей
	allValue4circle.init(amountRadius);
	for(int i=minRadius; i<maxRadius; i++)
		{
		allValue4circle.value[i-minRadius].init(2*i+2);
		}
	
	//рассчет параметров для каждой точки изображения
	calcAllVal4Circle();
	
#ifdef DEBUG
	std::cout << "[mycv::HoughTransform::HoughTransform]: Аккумулятор для окружностей рассчитан\n";
#endif

	}
//==============================================================================

//destructor
//==============================================================================
mycv::HoughTransform::~HoughTransform()
	{
	cvReleaseImage(&this->accumulator4line);
	cvReleaseImage(&this->accumulator4circle);
	}
//==============================================================================

//circle
//==============================================================================
int mycv::HoughTransform::circle(IplImage* src)
	{	
	long t1, t2;
	float t3;

#ifdef DEBUG
	//если получено изображение не заявленного размера
	if(src->width!=this->sizeImg.width || src->height!=this->sizeImg.height)	
		{
		std::cout << "[HoughTransform::circle]: Size image is not valid" << std::endl;
		return -1;
		}
#endif
	
	//обнуление аккумулятора
	cvZero(accumulator4circle);
	
	//для каждой точки пренадлежащей контуру
	t1=clock();
	for(int x=0; x<sizeImg.width; x++)
		{
		for(int y=0; y<sizeImg.height; y++)
			{
			if(PIXEL(uchar, src, x, y)[0]==255)	//если точка принадлежит контуру
				{
				//для каждого возможного радиуса
				for(int r=minRadius; r<this->maxRadius; r++)
					{
					//для каждой координаты центра по x
					for(int a=x-r; a<x+r; a++)
						{
						//ищем две координаты центра по y
						int b1 = allValue4circle.value[r-minRadius].value[a-(x-r)].b1+(y-r);
						int b2 = allValue4circle.value[r-minRadius].value[a-(x-r)].b2+(y-r);
						
						//инкриментируем соответствующую точку аккумулятора
						if(b1>=0 && b1<accumulator4circle->height && a>=0 && a<accumulator4circle->height)
							{
							PIXEL(uchar, accumulator4circle, a, b1)[r]++;
							}
						if(b2>=0 && b2<accumulator4circle->height && a>=0 && a<accumulator4circle->height)
							{
							PIXEL(uchar, accumulator4circle, a, b2)[r]++;
							}
						}
					}
				}
			}
		}
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::HoughTransform::circle]: Преобразование завершено (" << t3 << " ms)\n";
	
	//анализ аккумулятора
	
	int valueMax=0;
	int radiusMax=0;
	CvPoint center;
	
	for(int x=0; x<accumulator4circle->width; x++)
		{
		for(int y=0; y<accumulator4circle->height; y++)
			{
			for(int r=0; r<amountRadius; r++)
				{
				uchar val;
				val=PIXEL(uchar, accumulator4circle, x, y)[r];
				if(val>valueMax)
					{
					valueMax=val;
					center=cvPoint(x, y);
					radiusMax=r;
					}
				}
			}
		}
	
	
	IplImage* draw = cvCreateImage(cvGetSize(src), 8, 3);
	cvZero(draw);
	cvCircle(draw, center, radiusMax, CV_RGB(125, 0, 0));	
	cvShowImage("draw", draw);
	
	IplImage* dst = cvCreateImage(cvGetSize(src), 8, 1);
	for(int x=0; x<dst->width; x++)
		{
		for(int y=0; y<dst->height; y++)
			{
			PIXEL(uchar, dst, x, y)[0] = PIXEL(uchar, accumulator4circle, x, y)[radiusMax];
			}
		}	
	cvShowImage("dst", dst);
	
	return 0;
	}
//==============================================================================

//line
//==============================================================================
int mycv::HoughTransform::line(IplImage* src, std::vector<mycv::Lines>* lines)
	{
#ifdef DEBUG
	long t1, t2;
	float t3;
#endif

#ifdef DEBUG
	//если получено изображение не заявленного размера
	if(src->width!=this->sizeImg.width || src->height!=this->sizeImg.height)	
		{
		std::cout << "[HoughTransform::line]: Size image is not valid (" << src->width << " x " << src->height << ")" << std::endl;
		return -1;
		}
#endif
	
	//обнуление аккумулятора
	cvZero(this->accumulator4line);

	//аккумулятор для запоминания точек
	mycv::Arr2D< std::vector<CvPoint> > points;
	points.init(180+1, this->lenDiagonal*2);

	
#ifdef DEBUG
	t1=clock();
#endif
	//для каждой точки пренадлежащей контуру
	for(int x=0; x<this->sizeImg.width; x++)
		{
		for(int y=0; y<this->sizeImg.height; y++)
			{
			if(PIXEL(uchar, src, x, y)[0]==255)		//если точка принадлежит контуру
				{
				//для каждого градуса наклона
				for(int f=0; f<180+1; f++)
					{
					int r = this->allValue4line.value[x][y][f];	//получаем длину радиус-вектора
					//помечаем соотвествующую точку аккумулятора
					PIXEL(uchar, this->accumulator4line, f, r)[0]++; 
					points.value[f][r].push_back(cvPoint(x, y));
					}
				}
			}
		}
	
	//вычитаем голоса перекрестных точек
	for(int f=0; f<180+1; f++)
		{
		for(int r=0; r<this->accumulator4line->height; r++)
			{
			//перебираем все точки
			for(std::vector<CvPoint>::iterator it = points.value[f][r].begin(); it!=points.value[f][r].end(); ++it)
				{
				if(abs(it->x - (it-1)->x)>1 ||  abs(it->y - (it-1)->y)>1 ||
				abs(it->x - (it+1)->x)>1 ||  abs(it->y - (it+1)->y)>1)	{//если точки расположины на большом расстоянии от друг друга

					 PIXEL(uchar, this->accumulator4line, f, r)[0]--;
					// points.value[f][r].erase(it);
					 }
					
				}
			}
		}
#ifdef DEBUG
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::HoughTransform::line]: Преобразование завершено (" << t3 << " ms)\n";
#endif

	
#ifdef DEBUG
	t1=clock();
#endif

	//анализ аккумулятора
	IplImage* draw= cvCreateImage(cvGetSize(src), 8, 1);
	int threshold = 1;
	int maxValue=0;
	int maxF=0;
	int maxR=0;
	int maxLine=100;
	Lines line;
	
	for(int i=0; i<maxLine; i++)
		{
		maxValue=0;
		//ищем максимум аккумулятора
		for(int f=0; f<180+1; f++)
			{
			for(int r=0; r<this->accumulator4line->height; r++)
				{
				if(PIXEL(uchar, accumulator4line, f, r)[0]>maxValue)
					{
					maxValue=PIXEL(uchar, accumulator4line, f, r)[0];
					maxR=r;
					maxF=f;
					}
				}
			}
		
		//если значение меньше минимального порога
		if(maxValue<threshold)	{ break; }
		
		//удаляем следы всех точек лежащих на прямой
		//для каждой точки на прямой
		for(std::vector<CvPoint>::iterator it=points.value[maxF][maxR].begin(); it!=points.value[maxF][maxR].end(); ++it)
			{
			//для каждого f
			for(int f=0; f<180+1; f++)
				{
				int x = it->x;
				int y = it->y;
				int r = this->allValue4line.value[x][y][f];	//получаем длину радиус-вектора
				if(PIXEL(uchar, this->accumulator4line, f, r)[0]!=0) //вычитаем взнос данной точки из аккумулятора
					{
					PIXEL(uchar, this->accumulator4line, f, r)[0]--;
					}
				if(PIXEL(uchar, this->accumulator4line, f, r-1)[0]!=0) //вычитаем взнос данной точки из аккумулятора
					{
					PIXEL(uchar, this->accumulator4line, f, r-1)[0]--;
					}
				if(PIXEL(uchar, this->accumulator4line, f, r+1)[0]!=0) //вычитаем взнос данной точки из аккумулятора
					{
					PIXEL(uchar, this->accumulator4line, f, r+1)[0]--;
					}
				PIXEL(uchar, draw, x, y)[0]=255;
				}
			}
		
		//записываем данные прямой
		line.theta=maxF;
		line.rho=maxR-this->lenDiagonal;	
		line.weight = maxValue;
		lines->push_back(line);		
		}
		
#ifdef DEBUG
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycv::HoughTransform::line]:  Анализ аккумулятора завершен (" << t3 << " ms)\n";
#endif

#ifdef DEBUG
	cvNamedWindow("HoughTransform::line accumulator", 0);
	cvSaveImage("hough.png", this->accumulator4line);
	cvShowImage("HoughTransform::line accumulator", this->accumulator4line);
	cvShowImage("draw", draw);
#endif	

	return 0;	
	}
//==============================================================================


//рассчитать все значения акк. массива для линии
//==============================================================================
void mycv::HoughTransform::calcAllVal4Line()
	{
	//для каждой точки изображения
	for(int y=0; y<this->sizeImg.height; y++)
		{
		for(int x=0; x<this->sizeImg.width; x++)
			{
			//для каждого f
			for(int f=0; f<180+1; f++)
				{
				//переводим градусы в радианы
				float theta = f * CV_PI / 180.0;
				//считаем радиус для данного угла
				int r = ((x) * cos(theta) + (y) * sin(theta) + lenDiagonal);
				//записываем значение
				this->allValue4line.value[x][y][f]=r;
				}
			}
		}
	}
//==============================================================================


//рассчитать все значения акк. массива для окружности
//==============================================================================
void mycv::HoughTransform::calcAllVal4Circle()
	{
	//для каждого возможного радиуса
	for(int r=minRadius; r<maxRadius; r++)
		{
		//координаты точки лежащей на окружности
		int x = (2*r)/2;
		int y = x;
		
		//для каждой координаты центра по x
		for(int a=0; a<2*r; a++)
			{
			//ищем две координаты центр по y
			int b1 = y - sqrt(pow(r, 2) - pow((x - a), 2));
			int b2 = y + sqrt(pow(r, 2) - pow((x - a), 2));

#ifdef DEBUG
			if(b1>=2*r+2 || b1<0 ||  b2>=2*r+2 || b2<0)
				{
				std::cout << "[mycv::HoughTransform::calcAllVal4Circle]: " << b1 << ' ' << b2 << std::endl;
				continue;
				}
#endif			
	
			this->allValue4circle.value[r-minRadius].value[a].b1 = b1;
			this->allValue4circle.value[r-minRadius].value[a].b2 = b2;
			}
		}
	}
//==============================================================================

