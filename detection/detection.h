#ifndef _DETECTION_H_
#define _DETECTION_H_

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include <err.h>
# include "neural/neural.h"
# include "../process/process.h"
//--------------------
//        UTILS
//--------------------

double randomizeDouble();

double randDouble(double min, double max);

double gaussRandom(double min, double max);

void swap(double **a, double **b);

void shuffleArray(double **begin, double **end);

size_t compareTwoString(char str1[], char str2[]);

//--------------------
//      QUEUE
//--------------------

struct list {
    struct list *next;
    size_t          data;
};

struct queue {
    struct list *store;
    size_t       size;
};

void queue_init(struct queue *queue);
int queue_is_empty(struct queue *queue);
void queue_push(struct queue *queue, size_t elm);
size_t queue_pop(struct queue *queue);

//--------------------
//       DETECTION
//--------------------

//return the histogram by lines of an image
void getHistoLines(Image *img, size_t *tab);

//pas is -1 or 1
//return the histogram by columns of an image
void getHistoCols(Image *img, size_t *tab);

//pas is -1 or 1, put 1 by default
//return the first black index
size_t findBegin( size_t *histo, size_t coord, size_t pas);

//pas is -1 or 1, put 1 by default
//return the last black index
size_t findEnd(size_t *histo, size_t coord, size_t pas);

//cutLines and call cutCols
//to detect char learn = 0
//to learn learn = 1
//learn = 2 for the soutenance
//file is the file to learn
void cutLines(Image *image, size_t learn, FILE* file, Network *network);

//cutCols
//to detect char learn = 0
//to learn learn = 1
//learn = 2 for the soutenance
//file is the file to learn
void cutCols(Image *image, size_t learn, FILE* file, Network *network, struct queue *qfile);

//straightens the text
void redress(Image *image);

//calculates the number of black pixels on the first 5 text colons
size_t nb_black_pixels_on_the_first_5_cols(Image *img);

#endif
