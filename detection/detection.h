#ifndef _DETECTION_H_
#define _DETECTION_H_

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include "../process/process.h"

//--------------------
//		UTILS
//--------------------

double randomizeDouble();

double gaussRandom(double min, double max);

//--------------------
//	   DETECTION
//--------------------

//return the histogram by lines of an image
void getHistoLines(Image *img, size_t *tab);

//pas is -1 or 1
//return the historgram by columns of an image
void getHistoCols(Image *img, size_t *tab);

//pas is -1 or 1, put 1 by default
//return the first blakc index
size_t findBegin( size_t *histo, size_t coord, size_t pas);

//pas is -1 or 1, put 1 by default
//return the last black index
size_t findEnd(size_t *histo, size_t coord, size_t pas);

//cutLines and call cutCols
void cutLines(Image *image);

//cutCols
void cutCols(Image *image);

#endif
