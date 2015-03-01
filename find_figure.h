#ifndef FIND_FIGURE_H#define FIND_FIGURE_H#include "mycv.h"#include "contour.h"namespace mycv	{	class FindFigure		{		public:		FindFigure(CvSize imgSize, int minR, int maxR);		~FindFigure();				int findLine(IplImage* src,					std::vector<mycv::Line>* lines,					mycv::ContourStorage* contours);							int findCircle(IplImage* src,						mycv::ContourStorage* contours,						std::vector<mycv::Line>* lines,						std::vector<mycv::Circle>* circle);				private:		float accApprox; //точность аппроксимирования		float accR;			//точность поиска радиуса		int stepRadius;		IplImage* accum4circle;		mycv::Arr1D< float > rAccum4circle;		mycv::Arr2D< std::vector< std::vector<mycv::Line>::iterator > > members_circle;		int minRadius;		int maxRadius;		};		void lineABC(CvPoint* begin, CvPoint* end, float* a, float* b, float* c);	void drawLine(IplImage* src, float a, float b, float c);	void drawLineB(IplImage* src,					mycv::Arr2D< std::vector< std::vector<mycv::Line>::iterator > >* accum,					CvPoint* begin, CvPoint* end, char ty, std::vector<mycv::Line>::iterator i);	}#endif