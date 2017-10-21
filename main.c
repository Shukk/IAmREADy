#include<stdlib.h>
#include<stdio.h>
#include"detection/neural/neural.h"
#include"detection/detection.h"
#include"process/process.h"
#include<time.h>

int main() {
    init_sdl();
    SDL_Surface *surface = load_image("numbers.png");
    otsu(surface);
    Image image = binarize(surface);
    cutLines(&image, 0, NULL);
    /*Image newImage = resizeWithProp(&image, 100);
    SDL_SaveBMP(imageToSDLSurface(&newImage), "newTST.jpg");*/

    //killImage(&image);
    //killImage(&newImage);
    /*srand ( time(NULL) );

    for (int i = 0; i < 1; i++) {
        testXOR();
        //printf("%f\n", gaussRandom(-1.0, 1.0));
    }*/
    //testAND();

    /*size_t layersSize[] = {784, 15, 1};
    Network network = newNetwork(layersSize, 1, 0);

    learningFile(&network, "detection/neural/trainings/numbers.trd");

    killNetwork(&network);*/

    //createTraining("detection/neural/trainings/numbers.trd", "numbers.png");

    return 0;
}
