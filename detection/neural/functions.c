#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"neural.h"

double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

double sigmoidPrime(double x) {
    return exp(-x) / pow((1 + exp(-x)),2);
}

double linear(double x) {
    return x;
}

double linearPrime(double x) {
    return x / x;
}

double tannh(double x) {
    //return (exp(x) - exp(-x)) / (exp(x) + exp(-x));
    return tanh(x);
}

double tanhPrime(double x) {
    return 1 - pow(tanh(x), 2);
}

func_t getFunc(size_t id) {
    if (id == 1)
        return linear;
    else if (id == 2)
        return tannh;
    else
        return sigmoid;
}

func_t getFuncPrime(size_t id) {
    if (id == 1)
        return linearPrime;
    else if (id == 2)
        return tanhPrime;
    else
        return sigmoidPrime;
}
