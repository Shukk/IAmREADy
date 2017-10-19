#include "process.h"


void greyscale(SDL_Surface *img){
    
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 luminance;
    for (int w = 0; w < img->w; w++) {
        for (int h = 0; h < img ->h; h++) {
            SDL_GetRGB(getpixel(img, w, h), img->format, &r, &g, &b);
            luminance = 0.3*r + 0.59*g + 0.11*b;
            putpixel(img, w, h, SDL_MapRGB(img->format, luminance, luminance, 
            luminance));
        }
    }    
}



void otsu(SDL_Surface *img){
    int *histo = calloc(256, sizeof(int));
    if (histo == NULL){
        puts("ERROR : memory problem");
        exit(EXIT_FAILURE);
    }

    for (int w = 0; w < img->w; w++) {
        for (int h = 0; h < img ->h; h++) {
            Uint8 pix= getGreyPixel(img, w, h);
            histo[pix] += 1;
        }
    }
    
    int total = img->w * img->h;
    
    float sum = 0;
    for (int t = 0; t < 256; t++)
        sum += t * histo[t];
        
    float sumB = 0;
    int wB = 0;
    int wF = 0;
    
    float varMax = 0;
    int threshold = 0;
    
    for(int t = 0; t < 256; t++){
        wB += histo[t];
        if (wB == 0) 
            continue;

        wF = total - wB;
        if (wF == 0)
            break;
        
        sumB += (float)(t * histo[t]);

        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax){
            varMax = varBetween;
            threshold = t;
        }
    }
    int avgWhite = 0;
    int avgBlack = 0;
     for (int w = 0; w < img->w; w++) {
        for (int h = 0; h < img ->h; h++) {
             Uint8 pix= getGreyPixel(img, w, h);
             Uint8 lum;
             if (pix < threshold){
                 lum = 0;
                 avgBlack += 1;
             }
             else{
                 lum = 255;
                 avgWhite += 1;
             }
            putpixel(img, w, h, SDL_MapRGB(img->format, lum, lum, lum));

        }
    }
    if (avgBlack > avgWhite){
        for (int w = 0; w < img->w; w++) {
            for (int h = 0; h < img ->h; h++) {
                Uint8 lum;
                Uint8 pix= getGreyPixel(img, w, h);
                if (pix == 0)
                    lum = 255;
                else
                    lum = 0;
                putpixel(img, w, h, SDL_MapRGB(img->format, lum, lum, lum));

            }
        }
    }
    free(histo);
}



Image binarize(SDL_Surface *img){
     Image image = newImage(img->h, img->w);
     Uint8 r;
     Uint8 g;
     Uint8 b;
     for (size_t i = 0; i < image.lines; i++) {
        for (size_t j = 0; j < image.cols; j++) {
            SDL_GetRGB(getpixel(img, j, i), img->format, &r, &g, &b);
            
            if (r == 0)
                image.matrix[i][j] = 0;
            else
                image.matrix[i][j] = 1;
            
        }
     }
     return image;   
}



void colorProcess(char *path)
{
    init_sdl();
    SDL_Surface *img = load_image(path);
    display_image(img);
    //blackAndWhite(img);
    display_image(img);
    Image res = binarize(img);
    display_image(load_image("image2.jpg"));
    killImage(&res);
    SDL_FreeSurface(img);
}
