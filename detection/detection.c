#include "detection.h"
#include "../process/process.h"

void getHistoLines(Image *img, size_t *tab){
    size_t sum;
    for(size_t i = 0; i < img->lines; i++){
        sum = 0;
        for(size_t j = 0; j < img->cols; j++){
            if(img->matrix[i][j] == 0)
                sum++;
        }
        tab[i]=sum;
    }
}

void getHistoCols(Image *img, size_t *tab){
    size_t sum;
    for(size_t j = 0; j < img->cols; j++){
        sum = 0;
        for(size_t i = 0; i < img->lines; i++){
            if(img->matrix[i][j] == 0)
                sum++;
        }
        tab[j]=sum;
    }
}

size_t findBegin( size_t *histo, size_t coord, size_t pas){  
    while(histo[coord] == 0)
        coord += pas;

    return coord;
}

size_t findEnd(size_t *histo, size_t coord, size_t pas){
    while(histo[coord] > 0)
        coord += pas;

    return coord-1;
}

void cutLines(Image *image) {
    size_t *tab;
    tab = malloc(sizeof(size_t) * image->lines);

    getHistoLines(image, tab);
    
    size_t begin = findBegin(tab, 0, 1);
    size_t end = findEnd(tab, begin, 1);

    while(end < image->lines) {
        Image newImage = cut(image, begin, 0, end, image->cols - 1);

        cutCols(&newImage);

        begin = findBegin(tab, end + 1, 1);
        end = findEnd(tab, begin, 1);
        killImage(&newImage);
    }
    free(tab);
}

void cutCols(Image *image) {
    size_t *tab;
    tab = malloc(sizeof(size_t) * image->cols);

    getHistoCols(image, tab);

    size_t begin = findBegin(tab, 0, 1);
    size_t end = findEnd(tab, begin, 1);

    while(end < image->cols) {
        Image newImage = cut(image, 0, begin, image->lines - 1, end);

        size_t *subtab;
        subtab = malloc(sizeof(size_t) * newImage.lines);

        getHistoLines(&newImage, subtab);

        Image charImage = cut(
                            &newImage, 
                            findBegin(subtab, 0, 1), 
                            0, 
                            findBegin(subtab, newImage.lines-1, -1), 
                            newImage.cols - 1);
        

        //TODO CALL NEURAL NETWORK IN ORDER TO FIND THE CHAR

        Image resizeCharImage = resizeWithProp(&charImage, 50);

        char str[32];

        sprintf(str, "chars/c:%zu%zu.jpg", begin, end);
        SDL_SaveBMP(imageToSDLSurface(&resizeCharImage), str);

        begin = findBegin(tab, end + 1, 1);
        end = findEnd(tab, begin, 1);
        killImage(&newImage);
        killImage(&charImage);
        killImage(&resizeCharImage);
        free(subtab);
    }
    free(tab);
}
