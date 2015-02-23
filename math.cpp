#include "math.h"

//вычисление коэффициэнтов прямой
//==============================================================================
void mycv::lineABC(CvPoint begin, CvPoint end, float* a, float* b, float* c)
	{
	float x1	=	begin.x;	
	float y1	=	begin.y;
	float x2	=	end.x;
	float y2	=	end.y;
	
	*a=y2-y1;
	*b=-1.0*(x2-x1);
	*c=-1.0**a*x1-*b*y1;
	}
//==============================================================================

//расстояние от точки до прямой
//==============================================================================
float mycv::lineD(float a, float b, float c, CvPoint point)
	{
	float d;
	float m_x	=	point.x;
	float m_y	=	point.y;
	
	//mycv::lineABC(begin, end, &a, &b, &c);	//вычисляем коэфф. прямой
	
	//std::cout << "a=" << a << "    b=" << b << "    c=" << c << std::endl;
	
	if(a==0 && b==0)
		{
		std::cout << "a=0 b=0\n";
		d=0;
		}
		
	d=fabs(a * m_x + b * m_y + c)/sqrt(a * a + b * b);	//вычисляем расстояние до точки
	
	return d;
	}
//==============================================================================
