#include "process.h"
#include <math.h>

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

Image *rotation(Image *src_img,int angle){
    /*Image dst_img=newImage(src_img->lines,src_img->cols);

    float rad=(2*3.1416*angle)/360;
    float cosi =(float)cos(rad);
    float sini =(float)sin(rad);

    float x1=(-src_img->lines*sini);
    float y1=(src_img->lines*cosi);
    float x2=(src_img->cols*cosi-src_img->lines*sini);
    float y2=(src_img->lines*cosi+src_img->cols*sini);
    float x3=(src_img->cols*cosi);
    float y3=(src_img->cols*sini);


    float minx=min(0,min(x1,min(x2,x3)));
    float miny=min(0,min(y1,min(y2,y3)));
    float maxx=max(x1,max(x2,x3));
    float maxy=max(y1,max(y2,y3));

    int dst_img_w=(int)ceil(fabs(maxx)-minx);
    int dst_img_h=(int)ceil(fabs(maxy)-miny);

    dst_img->lines=dst_img_h;
    dst_img->cols=dst_img_w;

    for (int x=0;x<dst_img_w;x++){
        for (int y=0;y<dst_img_h;y++){
            int src_img_x=(int)((x+minx)*cosi+(y+miny)*sini);
            int src_img_y=(int)((y+miny)*cosi-(x+minx)*sini);
            if(src_img_x>=0 && src_img_x<src_img->cols && src_img_y>=0 
                && src_img_y<src_img->lines){
                dst_img.matrix[x][y]=src_img.matrix[src_img_x][src_img_y];
            }
        }
    }
    return &dst_img;*/
    angle++;
    return src_img;
}

float min(float x1,float x2){
    if (x1<x2){
        return x1;
    }
    return x2;
}

float max(float x1,float x2){
    if(x1>x2){
        return x1;
    }
    return x2;
}
