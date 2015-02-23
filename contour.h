#ifndef CONTOUR_H
#define CONTOUR_H

#include "mycv.h"

#include "math.h"

namespace mycv
	{
	typedef std::vector<CvPoint> Contour;
	typedef std::vector<CvPoint>::iterator ContourIt;
	typedef std::vector<mycv::Contour> ContourStorage;
	typedef std::vector<mycv::Contour>::iterator ContourStorageIt;

	typedef std::list<mycv::ContourIt> DominantPoints;		//доминирующие точки контура
	typedef std::list<mycv::ContourIt>::iterator DominantPointsIt;	//
	
	//поиск контура
	void findContours(IplImage* src, ContourStorage* contours);
	
	//Douglas-Peucker algorithm
	void approxContoursDp(mycv::Contour* contour,						
					mycv::DominantPoints* dominantPoints,
					mycv::DominantPointsIt beginNode,
					mycv::DominantPointsIt endNode,
					float eps);
	
	void drawContour(IplImage* src, mycv::Contour* contour);
				 
	//соседний контурный пиксель					 
	inline bool NEIGHBOR_PIXEL(IplImage* src, CvPoint* pt);		
	}

#endif
