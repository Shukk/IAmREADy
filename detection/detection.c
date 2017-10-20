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

void cutLines(Image *image, size_t learn, FILE* file) {
    size_t *tab;
    tab = malloc(sizeof(size_t) * image->lines);

    getHistoLines(image, tab);

    size_t begin = findBegin(tab, 0, 1);
    size_t end = findEnd(tab, begin, 1);

    while(end < image->lines) {
        Image newImage = cut(image, begin, 0, end, image->cols - 1);

        cutCols(&newImage, learn, file);

        begin = findBegin(tab, end + 1, 1);
        end = findEnd(tab, begin, 1);
        killImage(&newImage);
    }
    free(tab);
}

void cutCols(Image *image, size_t learn, FILE* file) {
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

        Image resizeCharImage = resizeWithProp(&charImage, 50);

        if (learn == 1) {
            //TODO CALL NEURAL NETWORK IN ORDER TO FIND THE CHAR

            /*char str[32];

            sprintf(str, "chars/mins/%d%zu%zu.jpg", 
                (int)randomizeDouble(0, 100), begin, end);
            SDL_SaveBMP(imageToSDLSurface(&resizeCharImage), str);*/

            int imageValue = 0;
            //display_image(imageToSDLSurface(&resizeCharImage));
            for (size_t i = 0; i < resizeCharImage.lines; i++) {
                printf("\n");
                for (size_t j = 0; j < resizeCharImage.cols; j++)
                    printf("%zu", resizeCharImage.matrix[i][j]);
            }
            printf("\n");

            printf("ENTER A VALUE : \n");
            while (imageValue < 33 || imageValue > 126)
                imageValue = getchar();
         
            if (file == NULL)
                errx(1, "ERROR THE FILE IS NULL");

            for (size_t i = 0; i < resizeCharImage.lines; i++) {
                for (size_t j = 0; j < resizeCharImage.cols; j++)
                    fprintf(file, "%zu", resizeCharImage.matrix[i][j]);
            }
            fprintf(file, ">%d\n", imageValue); 
        }

        begin = findBegin(tab, end + 1, 1);
        end = findEnd(tab, begin, 1);
        killImage(&newImage);
        killImage(&charImage);
        killImage(&resizeCharImage);
        free(subtab);
    }
    free(tab);
}
