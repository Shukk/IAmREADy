#include<stdlib.h>
#include<stdio.h>
#include"detection/neural/neural.h"
#include"detection/detection.h"
#include"process/process.h"
#include<time.h>

int main() {
    /*init_sdl();
    SDL_Surface *surface = load_image("tst.png");
    otsu(surface);
    Image image = binarize(surface);
    cutLines(&image);*/
    /*Image newImage = resizeWithProp(&image, 100);
    SDL_SaveBMP(imageToSDLSurface(&newImage), "newTST.jpg");*/

    //killImage(&image);
    //killImage(&newImage);
    srand ( time(NULL) );

    for (int i = 0; i < 1; i++) {
    	testAND();
    	//printf("%f\n", gaussRandom(-1.0, 1.0));
    }
	//testAND();

    return 0;
}
