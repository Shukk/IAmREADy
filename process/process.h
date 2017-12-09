// color_process.h


# ifndef _PROCESS_H_
# define _PROCESS_H_

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

//--------------------
//        IMAGE
//--------------------

typedef struct Image Image;
struct Image{
    size_t cols; //columns
    size_t lines; //lines
    size_t **matrix; //array with one pixel (black/white) in each case
};


// Create a new image of type Image with its dimensions
Image newImage(size_t lines, size_t cols);

// Free memory space
void killImage(Image *img);

//Cut the image
Image cut(Image *img, size_t xi, size_t yi, size_t xf, size_t yf);

//resize the image
Image resize(Image *image, size_t newLines, size_t newCols);

//resize the image with propotion
Image resizeWithProp(Image *image, size_t max);

//rotates the image according to the angle "angle"
Image *rotation(Image *src_img,int angle);

float min(float x1,float x2);

float max(float x1,float x2);

//--------------------
//        SDL
//--------------------

void wait_for_keypressed(void);

void init_sdl(void);

SDL_Surface* load_image(char *path);

SDL_Surface* display_image(SDL_Surface *img);

SDL_Surface* imageToSDLSurface(Image *image);

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

Uint8 getGreyPixel(SDL_Surface *img, int i, int j);

void convolution(SDL_Surface *img, int **mat, int div, int size);

void gaussian(SDL_Surface *img);

void contrast(SDL_Surface *img);

//--------------------
//        PROCESS
//--------------------

void greyscale(SDL_Surface *img);

void otsu(SDL_Surface *img);

Image binarize(SDL_Surface *img);

void colorProcess();

# endif
