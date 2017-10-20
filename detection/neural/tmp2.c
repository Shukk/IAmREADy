#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "neural.h"

void saveNetwork(Network *network, char path[])
{
        FILE * file = fopen(path, "w");
        if (file == NULL)
                printf("ERROR: WRONG PATH\n");
        fprintf(file, "%ld", network->hiddenLayers);
        for(unsigned int i = 0; i < (network->hiddenLayers + 2) ; i++){
//              for(unsigned int j = 0; j < network->layersSizes[i]; j++){
        fprintf(file, "/");
                fprintf(file, "%ld", network->layersSizes[i]);
//              }
    }
        fprintf(file, "/");
        for(unsigned int i = 0; i < (network->hiddenLayers + 1); i++){
                for(unsigned int j = 0; j < network->layersSizes[i]; j++){
                     fprintf(file, "/");
                        for(unsigned int k = 0; k < network->layersSizes[i+1];
                                                k++){
                        fprintf(file, "%f",network->synapses[i][j][k].weight);
                        fprintf(file, "/");
                        }
                }
        }
        fprintf(file, "\\");
    for(unsigned int i = 0; i < (network->hiddenLayers + 2); i++){
        for(unsigned int j = 0; j < network->layersSizes[i]; j++){
            fprintf(file, "%f",
                network->neurons[i][j].bias.weight);
            fprintf(file, "/");
        }
    }
    fprintf(file, "\\");
    fclose(file);
}

double charToInt(char *file, int *index){
    double res = 0;
    int ord = 1;
    int isFloat = 0;
printf("1\n");
//    if (0 == 1){
    while (file[*index] != '2'){
printf("2\n");
        if (file[*index] == '.')
            isFloat = 1;
        res = (isFloat)? res + (file[*index] - '0') / ord :
                res * ord + (file[*index] - '0');
        ord *=10;
        *index += 1;
printf("%d\n", *index);
    }
printf("3\n");
    *index = *index + 1;
printf("4\n");
//    }
    printf("*index = %d\n", *index);
    printf("index = %ls", index);
    printf("\n");
    return res;
}

char *getFile(const char *path){
    FILE *file = fopen(path, "r");

    if (file == NULL)
        errx(1, "file not found");
    fseek(file, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(file);
//    char tmp[len];
//    unsigned long len = 500;
    char *tmp = malloc(len);
//    file = file;
    fscanf(file, "%c", tmp);
//    printf("%ld", len);
//    char *tmp = malloc(sizeof(char)*1000);
    fclose(file);
    return tmp;
}

Network loadNetwork(char *file){
    int index = 0;
//    unsigned int hiddenLayers = (size_t)charToInt(file, &index);
    unsigned int hiddenLayers = 1;
    size_t layersSizes[hiddenLayers + 2];
printf("1\n");
    for(unsigned int i = 0; i < hiddenLayers; i++){
        if(file[index] != '\\'){
            layersSizes[i] = charToInt(file, &index);
        }
    }
    Network network = newNetwork( layersSizes, hiddenLayers, 2);
printf("2\n");
    for(unsigned int i = 0; i < hiddenLayers + 1; i++){
printf("3\n");
        for(unsigned int j = 0; j < layersSizes[i]; j++){
printf("4\n");
            for(unsigned int k = 0; k < layersSizes[i + 1]; k++){
printf("5\n");
                if(file[index] != '\\'){
                    network.synapses[i][j][k].weight =
                            charToInt(file,&index);
                }
                else { break;}
            }
        }
    }
printf("6\n");
    for(unsigned int i = 0; i < hiddenLayers + 2; i++){
printf("7\n");
        for(unsigned int j = 0; j < layersSizes[i]; j++){
printf("8\n");
             if(file[index] != '\\'){
                network.neurons[i][j].bias.weight = charToInt(
                                file,&index);
            }
        }
    }
printf("9\n");
    free(file);
    return network;
}

//network->synapses[i][j][k].weight
