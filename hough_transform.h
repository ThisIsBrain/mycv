#ifndef HOUGH_TRANSFORM_H
#define HOUGH_TRANSFORM_H

#include "mycv.h"

namespace mycv
	{
	//структура для хранения найденных прямых
	struct Lines
		{
		//параметры прямой
		short int theta;
		short int rho;
		
		//расположение точек на прямой
		//mycv::Arr1D<int> points;	//массив точек 
		
		//вес прямой
		float weight;
		};
		
	
		
	class HoughTransform
		{
		public:
		HoughTransform(CvSize sizeImg, int minRadius, int maxRadius);
		~HoughTransform();
		int line(IplImage* src, std::vector<mycv::Lines>* lines);		//поиск линий
		int circle(IplImage* src);		//поиск окружностей
		
		
		private:
		CvSize 		sizeImg;			//размеры исходного изображения
		int 		lenDiagonal;		//длинна диагонали изображения
		
		//прямые
		IplImage* 	accumulator4line;	//аккумуляторный массив для линий
		mycv::Arr3D<short int> allValue4line;	//все значения акккумуляторного массива для линий
		void calcAllVal4Line();			//рассчитать все значения акк. массива для линии
		
		
		//окружности
		int minRadius;
		int maxRadius;
		int amountRadius;
		
		struct TwoNumber
			{
			short int b1;
			short int b2;
			};
	
		mycv::Arr1D< mycv::Arr1D<TwoNumber> > allValue4circle;	//все значения акккумуляторного массива для окружности
		IplImage* accumulator4circle;		//аккумуляторный массив для окружностей
		void calcAllVal4Circle();			//рассчитать все значения акк. массива для окружности
		};
	}

#endif
