#ifndef TYPE_H
#define TYPE_H

#include "mycv.h"
#include "contour.h"

namespace mycv 
	{
	typedef std::vector<CvPoint> Contour;
	typedef std::vector<CvPoint>::iterator ContourIt;
	typedef std::vector<mycv::Contour> ContourStorage;
	typedef std::vector<mycv::Contour>::iterator ContourStorageIt;
	
	struct Line
		{
		float a;
		float b;
		float c;
		
		CvPoint begin;
		CvPoint end;
		
		mycv::ContourIt contourBegin;	
		mycv::ContourIt contourEnd;
		
		float weight;
		};
	
	struct Circle
		{
		CvPoint center;	
		float radius;
		
		float weight;
		};
		
	}
	
	
typedef unsigned char u08;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;
typedef signed char s08;
typedef signed short int s16;
typedef signed int s32;
typedef signed long int s64;



#endif
