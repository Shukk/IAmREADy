#include <time.h>
#include"neural.h"

Network newNetwork(
    size_t *layersSizes,
    size_t hiddenLayers,
    size_t logisticId) {
    Network network;

    network.hiddenLayers = hiddenLayers;
    network.layersSizes = layersSizes;

    network.neurons = malloc(hiddenLayers + 2*sizeof(Neuron));
    if (network.neurons == NULL)
        errx(1, "ALLOC ERROR network neurons");
    for (size_t i = 0; i < (hiddenLayers + 2); i++) {
        network.neurons[i] = malloc(layersSizes[i]*sizeof(Neuron));
        if (network.neurons[i] == NULL)
            errx(1, "ALLOC ERROR network neurons i:%zu", i);
        for (size_t j = 0; j < layersSizes[i]; j++) {
            network.neurons[i][j] = newNeuron();
        }
    }

    network.synapses = malloc(hiddenLayers + 1*sizeof(Synapse));
    if (network.synapses == NULL)
        errx(1, "ALLOC ERROR network synapses");
    for (size_t i = 0; i < hiddenLayers + 1; i++) {
        network.synapses[i] = malloc(layersSizes[i]*sizeof(Synapse));
        if (network.synapses[i] == NULL)
            errx(1, "ALLOC ERROR network synapses i:%zu", i);
        for (size_t j = 0; j < layersSizes[i]; j++) {
            network.synapses[i][j] = malloc(layersSizes[i+1]*sizeof(Synapse));
            if (network.synapses[i][j] == NULL)
                errx(1, "ALLOC ERROR network synapses i:%zu j:%zu", i, j);
            for (size_t k = 0; k < layersSizes[i + 1]; k++)
                network.synapses[i][j][k] = newSynapse();
        }
    }

    network.logisticId = logisticId;
    network.logistic = getFunc(logisticId);
    network.logisticPrime = getFuncPrime(logisticId);

    return network;
}

void reinitNetwork(Network *network) {
    for (size_t i = 0; i < (network->hiddenLayers + 2); i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            network->neurons[i][j] = newNeuron();
        }
    }

    for (size_t i = 0; i < network->hiddenLayers + 1; i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++)
                network->synapses[i][j][k] = newSynapse();
        }
    }
}

void killNetwork(Network *network) {
    for (size_t i = 0; i < (network->hiddenLayers + 2); i++) {
        free(network->neurons[i]);
        network->neurons[i] = NULL;
    }
    free(network->neurons);
    network->neurons = NULL;

    for (size_t i = 0; i < network->hiddenLayers + 1; i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            free(network->synapses[i][j]);
            network->synapses[i][j] = NULL;
        }
        free(network->synapses[i]);
        network->synapses[i] = NULL;
    }
    free(network->synapses);
    network->synapses = NULL;
}

double forwardProp(Network *network, double *inputs) {
    for (size_t i = 0; i < network->layersSizes[0]; i++)
        network->neurons[0][i].value = inputs[i];

    double sum;

    for (size_t i = 1; i < (network->hiddenLayers + 2); i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            sum = 0.0;
            for (size_t k = 0; k < network->layersSizes[i - 1]; k++) {
                sum += network->neurons[i-1][k].value
                    * network->synapses[i-1][k][j].weight;
            }
            network->neurons[i][j].z = sum+network->neurons[i][j].bias.weight;
            network->neurons[i][j].value =
                network->logistic(network->neurons[i][j].z);
        }
    }

    return network->neurons[network->hiddenLayers + 1][0].value;
}

void backProp(Network *network, double wantedOutput, double output) {

    network->neurons[network->hiddenLayers+1][0].delta =
        network->logisticPrime(network->neurons[network->hiddenLayers+1][0].z)
        *(wantedOutput - output);

    double sum;
    for (size_t i = network->hiddenLayers; i >= 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            sum = 0.0;
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {
                sum += network->synapses[i][j][k].weight
                    * network->neurons[i + 1][k].delta;
            }
            network->neurons[i][j].delta =
                network->logisticPrime(network->neurons[i][j].z) * sum;
        }
    }
}

void updateWeightsDelta(Network *network) {
    for (size_t i = network->hiddenLayers; i+1 > 0; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {
                network->synapses[i][j][k].delta =
                    network->synapses[i][j][k].delta
                    * network->momentum
                    + network->learningRate
                    * network->neurons[i+1][k].delta
                    * network->neurons[i][j].value;
            }
        }
    }
    for (size_t i = network->hiddenLayers+1; i > 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            network->neurons[i][j].bias.delta =
                network->neurons[i][j].bias.delta
                * network->momentum
                + network->learningRate
                * network->neurons[i][j].delta
                * network->neurons[i][j].value;
        }
    }
}

void updateWeights(Network *network) {
    for (size_t i = network->hiddenLayers; i+1 > 0; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {
                network->synapses[i][j][k].weight +=
                    network->synapses[i][j][k].delta;
            }
        }
    }
    for (size_t i = network->hiddenLayers+1; i > 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            network->neurons[i][j].bias.weight +=
                network->neurons[i][j].bias.delta;
        }
    }
}


void learning(
    Network *network,
    size_t nbData,
    double **in,
    double *out,
    double learningRate,
    double errorMax,
    double momentum,
    size_t maxEpochs) {

    network->learningRate = learningRate;
    network->errorMax = errorMax;
    network->momentum = momentum;
    network->maxEpochs = maxEpochs;

    double result;
    size_t epochs = 0;

    size_t nbBP = 0;
    for (size_t i = 0; epochs < maxEpochs && i < nbData; i++) {
        result = forwardProp(network, in[i]);
        //printf("%f\n", fabs(out[i] - result));
        if (fabs(out[i] - result) > network->errorMax) {
            backProp(network, out[i], result);
            updateWeightsDelta(network);
            nbBP++;
        }

        if (i == nbData - 1 && nbBP > 0) {
            i = -1;
            nbBP = 0;
            updateWeights(network);
        }

        epochs++;

        if (epochs >= maxEpochs) {
            i = -1;
            epochs = 0;
            nbBP = 0;
            reinitNetwork(network);
            //printf("FAIL TRY OTHER WEIGHTS\n");
        }
    }

    printf("\nepochs : %zu",epochs);
}

void learningFile(Network *network, char path[]) {
    FILE *file = NULL;

    file = fopen(path, "r");
    if (file == NULL)
        errx(1, "CAN NOT OPEN THE FILE");

    int achar = 0;
    size_t passdot = 0;
    size_t ord = 10;


    int line = 0;

    size_t nbData = 0;
    double learningRate = 0.0;
    double errorMax = 0.0;
    double momentum = 0.0;
    size_t maxEpochs = 0;

    double **in;
    double *out;

    do {
        achar = fgetc(file);

        if (achar == 10) {
            if (line == 0) {
                in = malloc(sizeof(double) * nbData);
                for (size_t i = 0; i < nbData; i++)
                    in[i] = malloc(sizeof(double) * network->layersSizes[0]);

                out = malloc(sizeof(double) * nbData);
            }

            if (line >= 4)
                ord = 0;
            else
                ord = 10;

            line++;
            passdot = 0;
            continue;
        }

        switch (line) {
            case 0:
                nbData = nbData * 10 + achar - 48;
                break;

            case 1:
                if (achar == 46)
                    passdot = 1;
                else if (passdot == 1) {
                    learningRate += (achar - 48.0) / ord;
                    ord *= 10;
                } else
                    learningRate = learningRate * 10 + achar - 48;
                break;

            case 2:
                if (achar == 46)
                    passdot = 1;
                else if (passdot == 1) {
                    errorMax += (achar - 48.0) / ord;
                    ord *= 10;
                } else
                    errorMax = errorMax * 10 + achar - 48;
                break;

            case 3:
                if (achar == 46)
                    passdot = 1;
                else if (passdot == 1) {
                    momentum += (achar - 48.0) / ord;
                    ord *= 10;
                } else
                    momentum = momentum * 10 + achar - 48;
                break;

            case 4:
                maxEpochs = maxEpochs * 10 + achar - 48;
                break;

            break;

            default:
                if (achar == EOF)
                    continue;
                else if (achar == 62)
                    passdot = 1;
                else if (passdot == 1)
                    out[line-5] = achar - 48;
                else {
                    in[line-5][ord] = achar - 48;
                    ord++;
                }
                break;

        }

    } while (achar != EOF);

    printf("nbData : %zu\n", nbData);
    printf("learningRate : %f\n", learningRate);
    printf("errorMax : %f\n", errorMax);
    printf("momentum : %f\n", momentum);
    printf("maxEpochs : %zu\n", maxEpochs);

    for (size_t i = 0; i < nbData; i++) {
        for (size_t j = 0; j < network->layersSizes[0]; j++)
            printf("%f,", in[i][j]);
        printf("->%f\n", out[i]);
    }


    clock_t t;
    t = clock();

    learning(
        network,
        nbData,
        in,
        out,
        learningRate,
        errorMax,
        momentum,
        maxEpochs);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    size_t print = 1;

    for (int i = 0; i < 4; i++) {
        out[i] = forwardProp(network, in[i]);
    }

    if (print == 1) {

        printWeights(network);

        printf("\n%fs\n", time_taken);

        for (int i = 0; i < 4; i++)
            printf("i:%d -> %f\n", i, out[i]);
    }
    printf("\n");

    fclose(file);
    for (size_t i = 0; i < nbData; i++)
        free(in[i]);
    free(in);
    free(out);
}

void createTraining(char path[]) {
    FILE* file = NULL;
 
    file = fopen(path, "r");

    if (file == NULL) {
        printf("the file does not exist. Creating file ...\n");

    }
    else {
        fclose(file);
        file = fopen(path, "a+");
    }
}


void printNetwork(Network *network) {
    printf("NEURONS : \n");
    printf("[\n");
    for (size_t i = 0; i < (network->hiddenLayers + 2); i++) {
        printf("    [");
        for (size_t j = 0; j < network->layersSizes[i] - 1; j++)
            printf("%f,", network->neurons[i][j].value);
        printf("%f", network->neurons[i][network->layersSizes[i] - 1].value);
        printf("]\n");
    }
    printf("]\n");

    printf("WEIGHTS : \n");
    printf("[\n");
    for (size_t i = 0; i < network->hiddenLayers + 1; i++) {
        printf("    [\n");
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            printf("        [");
            for (size_t k = 0; k < network->layersSizes[i + 1] - 1; k++)
                printf("%f,", network->synapses[i][j][k].weight);
            printf("%f",
                network->synapses[i][j][network->layersSizes[i+1]-1].weight);
            printf("]\n");
        }
        printf("    ]\n");
    }
    printf("]\n\n");
}

void printWeights(Network *network) {
    printf("\n");
    for (size_t i = 0; i < network->hiddenLayers + 1; i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++)
                printf("%f | ", network->synapses[i][j][k].weight);
        }
    }
}


//TODELETE FUNCTION ITS JUST A COPY FROM THE PERSONAL MAIN
void testXOR() {

    /*int seed = time(NULL);
    srand(seed);*/

    size_t layersSize[] = {2, 3, 1};
    Network network = newNetwork(layersSize, 1, 2);

    //printNetwork(&network);
    printWeights(&network);
    //printf("\nOUI\n");

    double *in[] =
        {(double[]){0.0, 0.0},
        (double[]){0.0, 1.0},
        (double[]){1.0, 0.0},
        (double[]){1.0, 1.0}};
    double wout[4] = {0.0, 1.0, 1.0, 0.0};
    double out[4] = {-1.0, -1.0, -1.0, -1.0};

    clock_t t;
    t = clock();

    learning(&network, 4, in, wout, 0.03, 0.001, 0.9, 900000);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

    size_t print = 1;
    //printWeights(&network);
    //printf("\n%fs\n                                                               ", time_taken);

    for (int i = 0; i < 4; i++) {
        out[i] = forwardProp(&network, in[i]);
        //printf("i:%d -> %f | ", i, out[i]);
        /*if (out[i] != out[i])
            print = 1;*/
    }
    //printf("\n");

    if (print == 1) {

        printWeights(&network);

        printf("\n%fs\n", time_taken);

        for (int i = 0; i < 4; i++)
            printf("                                                               i:%d -> %f\n", i, out[i]);
    }
    printf("\n");
    //printNetwork(&network);

    killNetwork(&network);
}

void testAND() {
    size_t layersSize[] = {2, 2, 1};
    Network network =
        newNetwork(layersSize, 1, 2);

    printNetwork(&network);

    double *in[] =
        {(double[]){0.0, 0.0},
        (double[]){0.0, 1.0},
        (double[]){1.0, 0.0},
        (double[]){1.0, 1.0}};
    double wout[4] = {0.0, 0.0, 0.0, 1.0};
    double out[4] = {-1.0, -1.0, -1.0, -1.0};

    clock_t t;
    t = clock();

    learning(&network, 4, in, wout, 0.03, 0.1, 0.9, 9000);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

    printf("%fs\n", time_taken);

    for (int i = 0; i < 4; i++) {
        out[i] = forwardProp(&network, in[i]);
        printf("i:%d -> %f\n", i, out[i]);
    }
    printNetwork(&network);

    killNetwork(&network);
}
