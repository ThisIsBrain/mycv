#ifndef MATH_H
#define MATH_H

#include "mycv.h"

namespace mycv
	{
	void lineABC(CvPoint begin, CvPoint end, float* a, float* b, float* c);
	float lineD(float a, float b, float c, CvPoint point);
	}

#endif
