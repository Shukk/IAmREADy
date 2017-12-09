#include "process.h"

void wait_for_keypressed(void) {
    SDL_Event             event;
    // Infinite loop, waiting for event
    for (;;) {
        // Take an event
        SDL_PollEvent( &event );
        // Switch on event type
        switch (event.type) {
        // Someone pressed a key -> leave the function
        case SDL_KEYDOWN: return;
        default: break;
        }
    // Loop until we got the expected event
    }
}


void init_sdl(void) {
    // Init only the video part
    if( SDL_Init(SDL_INIT_VIDEO)==-1 ) {
        // If it fails, die with an error message
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
    }
    // We don't really need a function for that ...
}


SDL_Surface* load_image(char *path) {
    SDL_Surface          *img;
    // Load an image using SDL_image with format detection
    img = IMG_Load(path);
    if (!img)
        // If it fails, die with an error message
        errx(3, "can't load %s: %s", path, IMG_GetError());
    return img;
}


SDL_Surface* display_image(SDL_Surface *img) {
    SDL_Surface          *screen;
    // Set the window to the same size as the image
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if ( screen == NULL ) {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                 img->w, img->h, SDL_GetError());
    }

    /* Blit onto the screen surface */
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    // wait for a key
    wait_for_keypressed();

    // return the screen for further uses
    return screen;
}

SDL_Surface* imageToSDLSurface(Image *image) {
    SDL_Surface* surface;

    surface = SDL_CreateRGBSurface (0, image->cols, image->lines, 32,0,0,0,0);

    for(size_t i = 0; i < image->lines; i++) {
        for(size_t j = 0; j < image->cols; j++) {
            size_t currentPixel = image->matrix[i][j];
            putpixel(surface, j, i, SDL_MapRGB(
                                        surface->format,
                                        currentPixel * 255,
                                        currentPixel * 255,
                                        currentPixel * 255));
        }
    }
    return surface;
}


static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y) {
    Uint8 *p = pixelref(surface, x, y);
    switch(surface->format->BytesPerPixel) {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    }
    return 0;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
    Uint8 *p = pixelref(surface, x, y);
    switch(surface->format->BytesPerPixel) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

Uint8 getGreyPixel(SDL_Surface *img, int i, int j){
    Uint8 r, g, b;
    SDL_GetRGB(getpixel(img, i, j), img->format, &r, &g, &b);
    return r;

}

void convolution(SDL_Surface *img, int **mat, int div, int size){
     int ml = (size-1)/2;
     for(int y = ml; y < img->h - ml; y++){
        for(int x = ml; x < img->w - ml; x++){
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;
            for(int i = -ml; i <= ml; i++){
                for(int j = -ml; j <= ml; j++){
                    Uint8 r, g, b;
                    SDL_GetRGB(getpixel(img, x + j, y + i),img->format, &r, &g
                    ,&b);

                    int cm = mat[j + ml][i + ml];
                    //int cm = 1;
                    sumR += r * cm / div;
                    sumG += g * cm / div;
                    sumB += b * cm / div;
                }
            }

            putpixel(img, x, y, SDL_MapRGB(img->format, sumR, sumG, sumB));
        }
     }

}

/*void gaussian(SDL_Surface *img){
    int *mat[] = {(int []){0,0,1,1,1,0,0},
                (int []){0,1,3,3,3,1,0},
                (int []){1,3,0,3-7,0,3,1},
                (int []){1,3,-7,-24,-7,3,1},
                (int []){1,3,0,-7,0,3,1},
                (int []){0,1,3,3,3,1,0},
                (int []){0,0,1,1,1,0,0}};
    convolution(img, mat,40,7);

}*/
void gaussian(SDL_Surface *img){
    int *mat[] = {(int []){1,4,7,4,1},
                (int []){4,16,26,16,4},
                (int []){7,26,41,26,7},
                (int []){4,16,26,16,4},
                (int []){1,4,7,4,1}};
    convolution(img, mat,273,5);

}
/*void gauvoid gaussian(SDL_Surface *img){
    int *mat[] = {(int []){1,2,1},
                (int []){2,4,2},
                (int []){1,2,1}};
    convolution(img, mat,16,3);

}*/
void contrast(SDL_Surface *img){
    int *mat[] = {(int []){0, -1, 0},
                (int []){-1,5,-1},
                (int []){0,-1,0}};
    convolution(img, mat,1,3);

}
