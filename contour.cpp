#include "contour.h"



//ПАРАМЕТРЫ:
//==============================================================================
void mycv::approxContoursDp(mycv::Contour* contour,						
					mycv::DominantPoints* dominantPoints,
					mycv::DominantPointsIt beginNode,
					mycv::DominantPointsIt endNode,
					float eps)
	{
	float maxD=0.0;		//максимальное расcтояние до точки
	float a, b, c;		//параметры данной прямой
	
	mycv::ContourIt maxPt;					//указатель на самую удаленную точку
	mycv::ContourIt contourBegin = *beginNode;	//начало контура
	mycv::ContourIt contourEnd   = *endNode;	//конец контура
	mycv::DominantPointsIt newNode;			//итератор на новый узел

	//1. Вычисление парметров данной прямой
	mycv::lineABC(cvPoint(contourBegin->x, contourBegin->y),
				 cvPoint(contourEnd->x, contourEnd->y),
				 &a, &b, &c);
				 
	//2. Вычисляем самую удаленную точку	
	//проходим по контуру в диапозоне begin-end
	for(mycv::ContourIt it=contourBegin; it!=contourEnd; ++it)
		{
		float value=mycv::lineD(a, b, c, cvPoint(it->x, it->y));
		if(value>maxD)
			{
			maxD=value;
			maxPt=it;
			}
		}


	//3. если точка сильно удалена от прямой - добавляем новый узел
	if(maxD>=eps)
		{		
		//добаволяем новый узел и получаем на него итератор
		newNode=dominantPoints->insert(endNode, maxPt);	
		
	//4.аппроксимируем полученые прямые
		mycv::approxContoursDp(contour,
								dominantPoints,
								beginNode,
								newNode,
								eps);
		mycv::approxContoursDp(contour,
								dominantPoints,
								newNode,
								endNode,
								eps);
		}
	
	}
//==============================================================================


//==============================================================================
void mycv::drawContour(IplImage* src, mycv::Contour* contour)
	{
	CvPoint point;
	mycv::ContourIt it;
	
	for(it=contour->begin(); it!=contour->end(); ++it)
		{
		point=*it;
		//if(point.x >=0 && point.x<src->width && point.y >= 0 && point.y < src->height)
		PIXEL(uchar, src, point.x, point.y)[0]=255;
		}
	
	}
//==============================================================================


//==============================================================================
void mycv::findContours(IplImage* src, ContourStorage* contours)
	{
	CvPoint point;	//текущая точка
	mycv::Contour contour;	//текущий контур
	
	//для каждой точки принадлежащей контуру
	for(int x=0; x<src->width; x++)
		{
		for(int y=0; y<src->height; y++)
			{
			if(PIXEL(uchar, src, x, y)[0]==255)	//если нашли начало контура
				{
				//начальный размер вектора
				contour.clear();
				//contour.reserve(std::max(src->width, src->height));	
				
				//первая точка контура
				point = cvPoint(x, y);	
				contour.push_back(point);	//set point in vector
				
				//пока не дойдем до конца контура
				for(; NEIGHBOR_PIXEL(src, &point); )	//переход на соседний пиксель
					{					
					//set point in vector
					contour.push_back(point);
					}
				
				//если не слишком короткий контур
				if(contour.size()>1)
					{
					//contour.shrink_to_fit();		//сжимаем вектор до минимального значения
					contours->push_back(contour);	//закидываем данный контур в массив контуров
					}
				}
			}
		}
	}
//==============================================================================					

//соседний пиксель на контуре
//==============================================================================
inline bool mycv::NEIGHBOR_PIXEL(IplImage* src, CvPoint* pt)
	{
	//здесь должна быть проверка на принадлежность пикселя контуру
	
	PIXEL(uchar, src, pt->x, pt->y)[0]=254;	//помечаем точку как найденную
	
	//перебираем соседние пиксели
	if(PIXEL(uchar, src, pt->x, pt->y+1)[0]==255)		//32
		{
		pt->y+=1;
		return true;
		}
		
	if(PIXEL(uchar, src, pt->x+1, pt->y)[0]==255)		//23
		{
		pt->x+=1;
		return true;
		}
		
	if(PIXEL(uchar, src, pt->x, pt->y-1)[0]==255)		//12
		{
		pt->y-=1;
		return true;
		}
	
	if(PIXEL(uchar, src, pt->x-1, pt->y)[0]==255)		//21
		{
		pt->x-=1;
		return true;
		}
	
	if(PIXEL(uchar, src, pt->x+1, pt->y+1)[0]==255)		//33
		{
		pt->x+=1;
		pt->y+=1;
		return true;
		}
			
	if(PIXEL(uchar, src, pt->x+1, pt->y-1)[0]==255)		//13
		{
		pt->x+=1;
		pt->y-=1;
		return true;
		}
		
	if(PIXEL(uchar, src, pt->x-1, pt->y+1)[0]==255)		//31
		{
		pt->x-=1;
		pt->y+=1;
		return true;
		}
	
	if(PIXEL(uchar, src, pt->x-1, pt->y-1)[0]==255)		//11
		{
		pt->x-=1;
		pt->y-=1;
		return true;
		}
	
	return false;	//нет соседних пикселей
	}
//==============================================================================	

/*end of file*/
