#include <cv.h>
#include <highgui.h>

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <time.h>
#include "mycv.h"
#include "hough_transform.h"
#include "find_figure.h"

void mycvHoughCircle(IplImage* src);
void mycvDrawLine(IplImage* src, float a, float b, float c);
void mycvDrawLine(IplImage* src, CvScalar color, int  theta, int rho);
int pifagor(int x, int y);
int createPyramidImg(IplImage* src, int amountImg, std::vector<IplImage*>* images);;

int main(int argc, char *argv[])
	{
//	CvCapture* capture = cvCreateCameraCapture(0);
//	if(capture==NULL)
//		{
//		std::cout << "[camera] error" << std::endl;
//		exit(0);
//		}
	
	long t1, t2;
	float t3;
	
	//img
	IplImage* src = cvLoadImage(argv[1]);
	if(!src)
		{
		std::cout << "[main]: файл не найден\n";
		return 0;
		}
	
	IplImage* img;
	
	//пирамида изображений
	IplImage* srcPyr=cvCreateImage(cvSize(src->width/2, src->height/2), 8, 3);
	cvPyrDown(src, srcPyr, IPL_GAUSSIAN_5x5);
	cvPyrUp(srcPyr, src);
	
	img=src;

	
	CvSize size= cvGetSize(img);
		
	IplImage* dst;
	IplImage* bin =	cvCreateImage(size, 8, 1);		//контур
	IplImage* circle_img = cvCreateImage(size, 8, 3); 

	//window
	cvNamedWindow("src", 0);
	cvNamedWindow("dst", 0);
	cvNamedWindow("bin", 0);
	cvNamedWindow("draw", 0);
	cvNamedWindow("roi", 0);
	cvNamedWindow("circle", 0);
	
	
	
	//получаем контуры
	cvCanny(img, bin, 10, 100, 3);			//конттуры
	
	//--------------------------------------------------------------
	mycv::FindFigure findFigure = mycv::FindFigure(size, 10, 200);
	std::vector<mycv::Line> lines;
	std::vector<mycv::Line>::iterator lineIt;
	std::vector<mycv::Circle> circle;
	std::vector<mycv::Circle>::iterator circleIt;
	
	mycv::ContourStorage contours;
	
	t1=clock();
	
	findFigure.findLine(bin, &lines, &contours);
	findFigure.findCircle(bin, &contours, &lines, &circle);
	
	
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[main]: Фигуры найдены (" << t3 << " ms)\n";
	
	//рисуем линии
	CvScalar color = CV_RGB(255, 0, 0);
	for(lineIt=lines.begin(); lineIt!=lines.end(); ++lineIt)
		{
		cvLine(img, cvPoint(lineIt->begin.x, lineIt->begin.y), cvPoint(lineIt->end.x, lineIt->end.y), CV_RGB(0, 255, 0));
		}
	
	//рисуем окружности
	cvZero(circle_img);
	for(circleIt=circle.begin(); circleIt!=circle.end(); ++circleIt) {
		//рисуем окружность на исходном изображении
		cvCircle(img, circleIt->center, circleIt->radius, CV_RGB(255, 0, 0));
		
		//выводим вес окружностей
		std::cout << "weight=" << circleIt->weight << std::endl;
		
		//рисуем контуры окружности
		//для каэдого сегмента
		for(std::vector<mycv::ContourSegment>::iterator it = circleIt->segments.begin();
			it!=circleIt->segments.end(); ++it)
			{
			for(mycv::ContourIt con = it->begin; con!=it->end; ++con)
				{
				PIXEL(uchar, circle_img, con->x, con->y)[0]=255;
				}
			cvLine(circle_img, *(it->begin), *(it->end), CV_RGB(0, 255, 0));
			}
	}
	
	
	//рисуем 
	//--------------------------------------------------------------
			
	cvShowImage("src", img);
	cvShowImage("bin", bin);
	cvShowImage("circle", circle_img);

	char mode = cvWaitKey(0);
		
	return 0;
	}


//==============================================================================
int createPyramidImg(IplImage* src, int amountImg, std::vector<IplImage*>* images)
	{
	images->resize(amountImg);
	CvSize size = cvGetSize(src);
	
	std::vector<IplImage*>::iterator i = images->begin();	//указатель на первое изображение
	*i = cvCloneImage(src);		//исходное изображение
	
	for(++i; i!=images->end(); ++i)
		{
		//если изображение делится по полам
		if(size.width % 2 == 0 && size.height % 2 == 0)
			{
			size.width /= 2;
			size.height /= 2;
			*i = cvCreateImage(size, 8, 3);	//создаем уменьшенное изображение
			cvPyrDown(*(i-1), *i, 7);	//получаем уменьшенное изображение
			}
		else
			{
			std::cout << "[createPyramidImg]: Size image is not valid\n";
			return -1;
			}
		}
		
	std::cout << "[createPyramidImg]: Pyramid  created\n";
	return 0;
	}
//==============================================================================			
			


//==============================================================================
void mycvDrawLine(IplImage* src, float a, float b, float c)
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
	cvLine(src, begin, end, CV_RGB(255, 0, 0));	
	}
//==============================================================================

int pifagor(int x, int y)
	{
	int len = cvRound( sqrt( (double)(pow( x, 2 ) + pow( y, 2 ) ) ) );
	return len;
	}

//==============================================================================
void mycvDrawLine(IplImage* src, CvScalar color, int  theta, int rho)
	{
	CvPoint begin;
	CvPoint end;
	
	float beta = theta * CV_PI / 180.0; //переводим градусы в радианы
	
	if(theta!=0)
		{
		begin.x=0;
		begin.y=-((begin.x*cos(beta)-rho)/sin(beta));
	
		end.x=src->width-1;
		end.y=-((end.x*cos(beta)-rho)/sin(beta));
		}
	else
		{
		begin.x=rho;
		begin.y=0;
		
		end.x=rho;
		end.y=src->height;
		}
	cvLine(src, begin, end, color);	
	}
//==============================================================================

/*
//преобразование хафа для поиска окружностей
//==============================================================================
void mycvHoughCircle(IplImage* src)
	{
	long t1, t2;
	float t3;
	std::cout << "[mycvHougCircle]: img size " << src->width << " x " << src->height << std::endl;
	
	//сглаживание исходного изображения
	cvSmooth(src, src, CV_GAUSSIAN, 3, 3);
	
	//изображения
	IplImage* bin		=	cvCreateImage(cvGetSize(src), 8, 1);		//контур
	
	//максимальный и минимальный радиус окружности который будет рассматриваться
	int maxR = std::min(src->width, src->height);
	int minR = maxR/10;		
	std::cout << "[mycvHougCircle]: minR = " << minR << std::endl;
	std::cout << "[mycvHougCircle]: maxR = " << maxR << std::endl;		
	
	//аккумулятор
	//[x][y][r]
	IplImage* phase = cvCreateImage(cvGetSize(src), IPL_DEPTH_16U, maxR);
	cvZero(phase);
	
	//получаем контур
	cvCanny(src, bin, 10, 100, 3);
	
	cvShowImage("bin", bin);
	
	std::cout << "[mycvHougCircle]: Контуры найдены\n";
	
	//ищем точки контура
	//b1, b2 - координаты центра окружности по y
	t1 = clock();
	int b1, b2;
	for(int y=0; y<bin->height; y++)
		{
		for(int x=0; x<bin->width; x++)
			{
			if(PIXEL(uchar, bin, x, y)[0]==255)	//если точка принадлежит контуру
				{
				//для каждого возможного радиуса
				for(int r=minR; r<maxR; r++)
					{
					//для каждой координаты центра по x
					for(int a=0; a<bin->width; a++)
						{
						//ищем две координаты центр по y
						b1 = y - sqrt(pow(r, 2) - pow((x - a), 2));
						b2 = y + sqrt(pow(r, 2) - pow((x - a), 2));
						
						//инкриментируем соответствующую точку аккумулятора
						if(b1>=0 && b1<phase->height)
							{
							PIXEL(uchar, phase, a, b1)[r]++;
							}
						if(b2>=0 && b2<phase->height)
							{
							PIXEL(uchar, phase, a, b2)[r]++;
							}
						}
					}
				}
			}
		}
	t2=clock();
	t3=((float)(t2)-(float)(t1))/1000;
	std::cout << "[mycvHougCircle]: Преобразование завершено (" << t3 << " ms)\n";
	
	//ищем макимум в фазовом пространстве
	int rMax=0;
	int aMax=0;
	int bMax=0;
	int valMax=0;
	for(int a=0; a<phase->width; a++)
		{
		for(int b=0; b<phase->height; b++)
			{
			for(int r=minR; r<maxR; r++)
				{
				//PIXEL(uchar, phase, a, b)[r]/=r;
				if(valMax<PIXEL(uchar, phase, a, b)[r])
					{
					valMax=PIXEL(uchar, phase, a, b)[r];
					aMax=a;
					bMax=b;
					rMax=r;
					}
				}
			}
		}
	
	
	//рисуем окружность
	cvCircle(src, cvPoint(aMax, bMax), rMax, CV_RGB(10, 10, 10));	
	}
//==============================================================================*/


