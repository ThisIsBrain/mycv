#ifndef MYCV_H
#define MYCV_H

#include <cv.h>
#include <highgui.h>

#include <time.h>
#include <vector>
#include <list>


#include "array1d.h"
#include "array2d.h"
#include "array3d.h"
#include "array4d.h"
#include "type.h"

//макрос для доступа к данным изображени
#define PIXEL(type, img, x, y)	(((type*) ((img)->imageData+(y)*(img)->widthStep))+(x)*(img)->nChannels)

#define DEBUG	

#endif
