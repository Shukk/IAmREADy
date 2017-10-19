#include "process.h"
#include <math.h>

 // Create a new image of type Image with its dimensions 
Image newImage(size_t lines, size_t cols){ 
   
    Image img;
    img.lines = lines;
    img.cols = cols;
    img.matrix = malloc(lines * sizeof(size_t));
    if (img.matrix == NULL)
        errx(1, "ALLOC ERROR img matrix");
    for (size_t i = 0; i < lines; i++) {
        img.matrix[i] = malloc(cols * sizeof(size_t));
        if (img.matrix[i] == NULL)
            errx(1, "ALLOC ERROR img matrix i:%zu", i);
        for (size_t j = 0; j < cols; j++)
            img.matrix[i][j] = 1;
    }
    return img;

}

// Free memory space
void killImage(Image *img){

    for(size_t i = 0; i < img->lines; i++){
        free(img->matrix[i]);
        img->matrix[i] = NULL;
    }
    free(img->matrix);
    img->matrix = NULL;

}

Image cut(Image *img, size_t xi, size_t yi, size_t xf, size_t yf){
    Image newImg = newImage(xf - xi + 1, yf - yi + 1);

    for (size_t i = xi; i <= xf; i++) {
        for (size_t j = yi; j <= yf; j++)
            newImg.matrix[i - xi][j - yi] = img->matrix[i][j];
    }
    return newImg;
}

Image resize(Image *image, size_t newLines, size_t newCols) {
    Image temp = newImage(newLines, newCols);

    double x_ratio = image->cols / (double)newCols;
    double y_ratio = image->lines / (double)newLines;

    double px, py;

    for (size_t i = 0; i < newLines; i++) {
        for (size_t j = 0; j < newCols; j++) {
            px = floor(j * x_ratio);
            py = floor(i * y_ratio);

            temp.matrix[i][j] = image->matrix[(int)py][(int)px];
        }
    }

    return temp;
}

Image resizeWithProp(Image *image, size_t max) {
    Image resizeImage;
    size_t startLines = 0;
    size_t startCols = 0;
    if (image->cols > image->lines) {
        resizeImage = resize(image, (image->lines * max)/image->cols, max);
        startLines = (max / 2) - (resizeImage.lines / 2);
    } else {
        resizeImage = resize(image, max, (image->cols * max)/image->lines);
        startCols = (max / 2) - (resizeImage.cols / 2);
    }

    Image lastImage = newImage(max, max);

    for (size_t i = 0; i < resizeImage.lines; i++) {
        for (size_t j = 0; j < resizeImage.cols; j++) {
            lastImage.matrix[i+startLines][j+startCols] =
                resizeImage.matrix[i][j];
        }
    }

    return lastImage;
}