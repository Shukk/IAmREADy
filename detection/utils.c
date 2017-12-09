#include"detection.h"

double randomizeDouble() {
    /*int seed = time(NULL);
    srand(seed);*/
    return ((double)rand() / (RAND_MAX + 1.0))/* * 2 - 1*/;
}

double randDouble(double min, double max) {
    return (rand() / ((double) RAND_MAX)) * (max - min) + min;
}

double gaussRandom(double min, double max) {
    double mu = 0.0;
    double sigma = 1.0;

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
    } while (
        rand_normal >= max ||
        rand_normal <= min ||
        fabs(rand_normal) < 0.1);

    //printf("OOOOOOOOOOOOOK%f\n",rand_normal);
    return rand_normal;
}

void swap(double **a, double **b) {
    double *tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void shuffleArray(double **begin, double **end) {
    int j;
    for (; end >= begin; --end) {
        j = rand()%(end - begin + 1);
        swap(end, begin + j);
    }
}

size_t compareTwoString(char *str1, char *str2) {

    if (sizeof(str1) != sizeof(str2))
        return 0;

    size_t len = *(&str1 + 1) - str1;

    for (size_t i = 0; i < len; i++) {
        printf("%zu\n", i);
        if (str1[i] != str2[i])
            return 0;
    }

    return 1;
}



void queue_init(struct queue *queue) {
    queue->store = NULL;
    queue->size = 0;
}
 
int queue_is_empty(struct queue *queue) {
    return queue->size == 0;
}
 
void queue_push(struct queue *queue, size_t elm) {
    struct list *l = malloc(sizeof(struct list));
    
    if (l == NULL) return;

    l->data = elm;

    if (queue_is_empty(queue))
        l->next = l;
    else {
        l->next = queue->store->next;
        queue->store->next = l;
    }

    queue->store = l;
    queue->size += 1;
}
 
 
size_t queue_pop(struct queue *queue) {
    struct list *elm = queue->store->next;
    size_t res = elm->data;

    queue->store->next = elm->next;
    free(elm);

    queue->size -= 1;
    return res;
}