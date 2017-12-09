#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include"process/process.h"
#include"detection/detection.h"
#include"detection/neural/neural.h"

int main(int argc, char *argv[]) {
    /*createFinalTraining("okokokokok.trd", "rfinal.net");
    return 0;*/

    srand(time(NULL));

    if (argc == 3 && strtoul(argv[1], NULL, 10) == 1) {
        init_sdl();

        SDL_Surface *surface = load_image(argv[2]);
        display_image(surface);

        /*contrast(surface);
        display_image(surface);*/

        gaussian(surface);
        display_image(surface);

        greyscale(surface);
        display_image(surface);

        otsu(surface);
        display_image(surface);

        Image image = binarize(surface);
        SDL_Surface *surfacebin = imageToSDLSurface(&image);
        display_image(surfacebin);

        SDL_FreeSurface(surface);
        SDL_FreeSurface(surfacebin);
        killImage(&image);
    } else if (argc == 5 && strtoul(argv[1], NULL, 10) == 2) {
        init_sdl();

        SDL_Surface *surface = load_image(argv[2]);
        gaussian(surface);
        greyscale(surface);
        otsu(surface);
        Image image = binarize(surface);

        //cutLines(&image, 2, NULL, NULL);

        FILE *file = fopen(argv[4], "w+");
        Network network = loadNetwork(argv[3]);
        cutLines(&image, 0, file, &network);
        fclose(file);

        SDL_FreeSurface(surface);
        killNetwork(&network);
        killImage(&image);
    } else if (argc == 3 && strtoul(argv[1], NULL, 10) == 3)  {
        size_t hiddenLayers;
        printf("Hidden layers : \n");
        scanf("%zu", &hiddenLayers);
        size_t *layersSizes = malloc(sizeof(size_t)*(hiddenLayers+2));

        printf("Inputs : \n");
        scanf("\n%zu", &layersSizes[0]);

        for (size_t i = 1; i < hiddenLayers + 1; i++) {
            printf("Neurons in layer %zu : \n", i);
            scanf("\n%zu", &layersSizes[i]);
        }

        printf("Outputs : \n");
        scanf("\n%zu", &layersSizes[hiddenLayers+1]);

        size_t logisticId;
        printf("Activation function (0:sigmoid / 1:linear / 2:tanh) :\n");
        scanf("\n%zu", &logisticId);

        char *outTrans = calloc(layersSizes[hiddenLayers+1], sizeof(char));
        for (size_t i = 0; i < layersSizes[hiddenLayers+1]; i++) {
            printf("Translation for the %zu output : \n", i);
            scanf("\n%c", &outTrans[i]);
        }

        Network network = newNetwork(
                            layersSizes,
                            hiddenLayers,
                            logisticId,
                            outTrans);

        saveNetwork(&network, argv[2]);

        killNetwork(&network);

    } else if (argc >= 5 && strtoul(argv[1], NULL, 10) == 4) {
        createTraining(argv[4], argv[3], argv[2]);
    } else if (argc >= 4 && strtoul(argv[1], NULL, 10) == 5) {
        Network network = loadNetwork(argv[2]);
        //reinitNetwork(&network);
        learningFile(&network, argv[2], argv[3], 1);
        saveNetwork(&network, argv[2]);
        killNetwork(&network);
    } else if (argc >= 4 && strtoul(argv[1], NULL, 10) == 6) {
        Network network = loadNetwork(argv[2]);
        learningFile(&network, argv[2], argv[3], 0);
        killNetwork(&network);
    } else {
        printf("HELP : \n");
        printf(". image process :     1 {src}\n");
        printf(". cut image :         2 {src} {net} {dest}\n");
        printf(". create network :    3 {dest}\n");
        printf(". create training :   4 {src} {net} {dest}\n");
        printf(". network training :  5 {net} {data}\n");
        printf(". execute network :   6 {net} {data}\n");
    }
    return 0;
}
