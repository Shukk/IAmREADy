#include "detection.h"
#include "../process/process.h"

//#include"neural/neural.h"

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

void cutLines(Image *image, size_t learn, FILE* file, Network *network) {
    size_t *tab;
    tab = malloc(sizeof(size_t) * image->lines);

    getHistoLines(image, tab);
    
    size_t begin = findBegin(tab, 0, 1);
    size_t end = image->lines-1;
    if (learn == 3)
        end = findBegin(tab, image->lines-1, -1);
    else
        end = findEnd(tab, begin, 1);

    size_t i = 0;


    struct queue spaceQueue;
    struct queue charQueue;

    if (learn == 0) {
        queue_init(&spaceQueue);
        queue_init(&charQueue);
        queue_push(&spaceQueue, begin);
    }

    size_t last = begin;
    size_t spaceaccu = begin;
    size_t characcu = 0;

    while(end < image->lines) {
        if (learn == 0)
            characcu += end - begin - 1;

        Image newImage = cut(image, begin, 0, end, image->cols - 1);
        if (learn == 2)
            display_image(imageToSDLSurface(&newImage));

        cutCols(&newImage, learn, file, network, &charQueue);

        if (learn == 0) {
            //fprintf(file, "\n");
            queue_push(&charQueue, '\n');
        }

        size_t newBegin = findBegin(tab, end + 1, 1);
        size_t newEnd = findEnd(tab, newBegin, 1);
        // test ajout saut de ligne
        /*if(learn==0){
            for(size_t n=0;(newBegin-end+3)>=(end-begin)*n;n++){
                fprintf(file, "\n");
            }
        }*/
        // ICI ajouter un saut à la ligne n fois

        if (learn == 0) {
            queue_push(&spaceQueue, newBegin - end - 1);
            spaceaccu += newBegin - end - 1;
            last = newBegin - end - 1;
        }
        begin = newBegin;
        end = newEnd;
        killImage(&newImage);

        i++;
    }

    if (learn == 0) {
        spaceaccu -= last;
        double spacemoy = spaceaccu / i;
        double charmoy = characcu / i;

        double moy = (spacemoy + charmoy) / 3;

        while (spaceQueue.size > 1) {
            size_t space = queue_pop(&spaceQueue);

            for (size_t k = 1; k < space / moy; k++)
                fprintf(file, "\n");

            char c = queue_pop(&charQueue);
            while (charQueue.size > 0 && c != '\n') {
                fprintf(file, "%c", c);
                c = queue_pop(&charQueue);
            }
            fprintf(file, "\n");
        }
        queue_pop(&spaceQueue);
    }
    else if (learn == 3) {
        printf("lines : %zu\n", i);
    }
    free(tab);
}

void cutCols(Image *image, size_t learn, FILE* file, Network *network, struct queue *qfile) {
    size_t *tab;
    tab = malloc(sizeof(size_t) * image->cols);

    char *allchar = "!#$%&()*+-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~";

    getHistoCols(image, tab);

    size_t begin = findBegin(tab, 0, 1);
    size_t end = findEnd(tab, begin, 1);

    size_t i = 0;

    struct queue spaceQueue;
    struct queue charQueue;

    if (learn == 0) {
        queue_init(&spaceQueue);
        queue_init(&charQueue);
        queue_push(&spaceQueue, begin);
    }

    size_t last = begin;
    size_t spaceaccu = begin;
    size_t characcu = 0;

    while(end < image->cols) {
        if (learn == 0)
            characcu += end - begin - 1;

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

        Image resizeCharImage = resizeWithProp(&charImage, 32);
        //test trouver le nombre de pixels max de la ligne
        /*tab_for_space=malloc(sizeof(size_t)*resizeCharImage.lines);
        getHistoCols(&resizeCharImage,tab_for_space);
        size_t len_max=0;
        size_t len=0;

        for(size_t i=0;i<tab_for_space.lines){
            if (tab_for_space[i]==1){
                len=len+1;
            }
            else{
                if(len_max<len){
                    len_max=len;
                }
                len=0;
            }
        }*/
        // end test  il faut maintenant calculer l'écart type de len

        if (learn == 3) {
            char imageValue = allchar[i];

            if (file == NULL)
                errx(1, "ERROR THE FILE IS NULL");

            for (size_t i = 0; i < resizeCharImage.lines; i++) {
                for (size_t j = 0; j < resizeCharImage.cols; j++)
                    fprintf(file, "%zu", resizeCharImage.matrix[i][j]);
            }

            fprintf(file, ">");


            for (size_t i=0;i<network->layersSizes[network->hiddenLayers+1];
                i++) {
                fprintf(file,"%d",network->outTrans[i]==imageValue?1:0);
            }

            fprintf(file, "\n");
        }
        else if (learn == 1) {
            char imageValue = 0;
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

            fprintf(file, ">");


            for (size_t i=0;i<network->layersSizes[network->hiddenLayers+1];
                i++) {
                fprintf(file,"%d",network->outTrans[i]==imageValue?1:0);
            }

            fprintf(file, "\n");
        } else if (learn == 2)
            display_image(imageToSDLSurface(&resizeCharImage));
        else if (learn == 0) {
            double * inputs = 
            malloc(sizeof(double)*resizeCharImage.lines*resizeCharImage.cols);

            for(unsigned int i = 0; i < resizeCharImage.lines; i++){
                //printf("\n");
                for(unsigned int j = 0; j < resizeCharImage.cols; j++){
                    inputs[j+i*resizeCharImage.cols] = 
                        resizeCharImage.matrix[i][j];
                    //printf("%zu", resizeCharImage.matrix[i][j]);
                }
            }
            //printf("\n");



            double *res = forwardProp(network,inputs);

            double max = pow(res[0], 2);
            size_t imax = 0;
            for (size_t j=1;j<network->layersSizes[network->hiddenLayers+1];
                j++){
                double cur = pow(res[j], 2);
                if (cur > max) {
                    max = cur;
                    imax = j;
                }
            }

            //fprintf(file,"%c",  network->outTrans[imax]);
            queue_push(&charQueue, network->outTrans[imax]);

            /*double errorMax = 0.1;

            for(unsigned int i = 0; i < network->layersSizes[
                            network->hiddenLayers + 1]; i++){
                if (res[i] + errorMax >= 1)
                    fprintf(file,"%c",  network->outTrans[i]);
            }*/

            free(inputs);
            free(res);
        }
        size_t newBegin = findBegin(tab, end + 1, 1);
        size_t newEnd = findEnd(tab, newBegin, 1);

        /*if (learn == 0 && newBegin-end>=(len)-3) {
            fprintf(file, " ");
        }*/

        if (learn == 0) {
            queue_push(&spaceQueue, newBegin - end - 1);
            spaceaccu += newBegin - end - 1;
            last = newBegin - end - 1;
        }

        begin = newBegin;
        end = newEnd;
        killImage(&newImage);
        killImage(&charImage);
        killImage(&resizeCharImage);
        free(subtab);
        i++;
    }



    if (learn == 0) {
        spaceaccu -= last;
        double spacemoy = spaceaccu / i;
        double charmoy = characcu / i;

        double moy = (spacemoy + charmoy) / 3;

        while (spaceQueue.size > 1 && charQueue.size > 0) {
            size_t space = queue_pop(&spaceQueue);
            char c = queue_pop(&charQueue);

            for (size_t k = 1; k < space / moy; k++) {
                //fprintf(file, " ");
                queue_push(qfile, ' ');
            }

            //fprintf(file, "%c", c);
            queue_push(qfile, c);

            //printf("%zu ", space);
        }
        queue_pop(&spaceQueue);
        //printf(" spacemoy : %f | charmoy : %f\n", spacemoy, charmoy);
    } else if (learn == 3) {
        printf("chars : %zu ", i);
    }

    free(tab);
}

void redress(Image *image){
    int best_angle_1;
    size_t best_score=0;
    int best_angle_2;
    Image *img_redress;
    for(int i=0;i<360;i++){
        img_redress=rotation(image,i);
        size_t *tab;
        tab = malloc(sizeof(size_t)*img_redress->lines);
        getHistoLines(img_redress, tab);
        size_t nb_white_lines=0;
        for(size_t j=0;j<img_redress->lines;j++){
            if(tab[j]==0){
                nb_white_lines+=1;
            }
        }
        if(best_score==nb_white_lines){
            best_angle_2=i;
        }
        else{
            if(best_score<nb_white_lines){
                best_angle_1=i;
                best_angle_2=-1;
            }
        }
    }
    killImage(img_redress);
    Image *img_redress_1=rotation(image,best_angle_1);
    if(best_angle_2!=-1){
        Image *img_redress_2=rotation(image,best_angle_2);
        if(nb_black_pixels_on_the_first_5_cols(img_redress_1)>
            nb_black_pixels_on_the_first_5_cols(img_redress_2)){
            image=img_redress_1;
            killImage(img_redress_2);
            // KILL une autre image?
        }
        else{
            image=img_redress_2;
            killImage(img_redress_1);
        }  

    }
    else{
        image=img_redress_1;
    }
}

size_t nb_black_pixels_on_the_first_5_cols(Image *img){
    size_t nb_black_pixels=0;
    size_t *tab;
    tab= malloc(sizeof(size_t) * img->lines);
    getHistoCols(img,tab);
    size_t nb_black_cols=0;
    for(size_t y=0;y<img->lines && nb_black_cols<5;y++){
        if(tab[y]!=0){
            nb_black_pixels+=tab[y];
            nb_black_cols+=1;
        }
    }
    return nb_black_pixels;     
}
