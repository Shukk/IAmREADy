#include"detection.h"

double randomizeDouble() {
    /*int seed = time(NULL);
    srand(seed);*/
    return ((double)rand() / (RAND_MAX + 1.0))/* * 2 - 1*/;
}

double gaussRandom(double min, double max) {
    double mu = 0.0;
    double sigma = 2.3;

    double u1, u2, rand_std_normal, rand_normal;

    do {
        //srand ( time(NULL) );
        u1 = randomizeDouble();
        u2 = randomizeDouble();

        rand_std_normal = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2);

        /*if (randomizeDouble() >= 0.5)
            rand_std_normal = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2);
        else
            rand_std_normal = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);*/

        rand_normal = 1 - (mu + sigma * rand_std_normal);

        //printf("OK%f\n",rand_normal);
    } while (rand_normal >= max || rand_normal <= min || rand_normal < 0.1);

    //printf("OOOOOOOOOOOOOK%f\n",rand_normal);
    return rand_normal;
}

void swap(int *a, int *b) {
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}