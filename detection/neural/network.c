#include <time.h>
#include <math.h>
#include"neural.h"
#include<omp.h>

Network newNetwork(
    size_t *layersSizes,
    size_t hiddenLayers,
    size_t logisticId,
    char *outTrans) {
    Network network;

    network.hiddenLayers = hiddenLayers;
    network.layersSizes = layersSizes;

    network.neurons = malloc((hiddenLayers + 2)*sizeof(Neuron));
    if (network.neurons == NULL)
        errx(1, "ALLOC ERROR network neurons");
    for (size_t i = 0; i < (hiddenLayers + 2); i++) {
        network.neurons[i] = malloc(layersSizes[i]*sizeof(Neuron));
        if (network.neurons[i] == NULL)
            errx(1, "ALLOC ERROR network neurons i:%zu", i);
        for (size_t j = 0; j < layersSizes[i]; j++) {
            network.neurons[i][j] = newNeuron(layersSizes[0]);
        }
    }

    network.synapses = malloc((hiddenLayers + 1)*sizeof(Synapse));
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
                network.synapses[i][j][k] = newSynapse(layersSizes[0]);
        }
    }

    network.outTrans = outTrans;

    network.logisticId = logisticId;
    network.logistic = getFunc(logisticId);
    network.logisticPrime = getFuncPrime(logisticId);

    return network;
}

void copyNetwork(Network *network, Network *net) {
    //return newNetwork(net->layersSizes, net->hiddenLayers, net->logisticId, net->outTrans);
    //Network network;

    network->learningRate = net->learningRate;
    network->errorMax = net->errorMax;
    network->momentum = net->momentum;
    network->maxEpochs = net->maxEpochs;

    network->hiddenLayers = net->hiddenLayers;

    network->logisticId = net->logisticId;
    
    network->logistic = net->logistic;
    network->logisticPrime = net->logisticPrime;

    network->layersSizes = net->layersSizes;

    network->outTrans = net->outTrans;


    network->neurons = malloc((net->hiddenLayers + 2)*sizeof(Neuron));
    if (network->neurons == NULL)
        errx(1, "ALLOC ERROR network neurons");
    for (size_t i = 0; i < (net->hiddenLayers + 2); i++) {
        network->neurons[i] = malloc(net->layersSizes[i]*sizeof(Neuron));
        if (network->neurons[i] == NULL)
            errx(1, "ALLOC ERROR network neurons i:%zu", i);
        for (size_t j = 0; j < net->layersSizes[i]; j++) 
            network->neurons[i][j] = copyNeuron(net->neurons[i][j]);
    }

    network->synapses = malloc((net->hiddenLayers + 1)*sizeof(Synapse));
    if (network->synapses == NULL)
        errx(1, "ALLOC ERROR network synapses");
    for (size_t i = 0; i < net->hiddenLayers + 1; i++) {
        network->synapses[i] = malloc(net->layersSizes[i]*sizeof(Synapse));
        if (network->synapses[i] == NULL)
            errx(1, "ALLOC ERROR network synapses i:%zu", i);
        for (size_t j = 0; j < net->layersSizes[i]; j++) {
            network->synapses[i][j] = malloc(net->layersSizes[i+1]*sizeof(Synapse));
            if (network->synapses[i][j] == NULL)
                errx(1, "ALLOC ERROR network synapses i:%zu j:%zu", i, j);
            for (size_t k = 0; k < net->layersSizes[i + 1]; k++)
                network->synapses[i][j][k] = copySynapse(net->synapses[i][j][k]);
        }
    }


}

void reinitNetwork(Network *network) {
    for (size_t i = 0; i < (network->hiddenLayers + 2); i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            network->neurons[i][j] = newNeuron(network->layersSizes[0]);
        }
    }

    for (size_t i = 0; i < network->hiddenLayers + 1; i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++)
                network->synapses[i][j][k]=newSynapse(network->layersSizes[0]);
        }
    }
}

void reinitSumDelta(Network *network) {
    for (size_t i = network->hiddenLayers; i+1 > 0; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++)
                network->synapses[i][j][k].sumDelta = 0;
        }
    }
    for (size_t i = network->hiddenLayers+1; i > 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++)
            network->neurons[i][j].bias.sumDelta = 0;
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

    free(network->layersSizes);
    free(network->outTrans);
}

void subKillNetwork(Network *network) {
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

double* forwardProp(Network *network, double *inputs) {
    for (size_t i = 0; i < network->layersSizes[0]; i++) 
        network->neurons[0][i].value = inputs[i];

    double sum;

    for (size_t i = 1; i < network->hiddenLayers + 2; i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            sum = 0.0;
            for (size_t k = 0; k < network->layersSizes[i - 1]; k++) {
                sum += network->neurons[i-1][k].value
                    * network->synapses[i-1][k][j].weight;
            }
            network->neurons[i][j].z=sum/*+network->neurons[i][j].bias.weight * 0*/;
            network->neurons[i][j].value =
                network->logistic(network->neurons[i][j].z);
        }
    }

    double *out =
        malloc(sizeof(double) * network->layersSizes[network->hiddenLayers+1]);

    for (size_t i = 0; i < network->layersSizes[network->hiddenLayers+1]; i++)
        out[i] = network->neurons[network->hiddenLayers+1][i].value;

    return out;
}

void backProp(Network *network, double *wantedOutput, double *output) {

    for (size_t i = 0; i < network->layersSizes[network->hiddenLayers+1]; i++){
        /*network->neurons[network->hiddenLayers+1][i].delta =
            network->logisticPrime(
                network->neurons[network->hiddenLayers+1][i].z)
            * (wantedOutput[i] - output[i]);*/
        
        double sum = 0.0;
        for (size_t j = 0; j < network->layersSizes[network->hiddenLayers+1]; j++)
            sum += exp(network->neurons[network->hiddenLayers+1][j].value);

        network->neurons[network->hiddenLayers+1][i].delta = 
            (exp(network->neurons[network->hiddenLayers+1][i].value) / sum)
            * (wantedOutput[i] - output[i]);
    }

    double sum;
    for (size_t i = network->hiddenLayers; i >= 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            sum = 0.0;
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {
                sum = sum + network->synapses[i][j][k].weight
                    * network->neurons[i + 1][k].delta;
            }
            network->neurons[i][j].delta =
                network->logisticPrime(network->neurons[i][j].z) * sum;
        }
    }
    
    updateWeightsDelta(network);
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
               
                /*network->synapses[i][j][k].sumDelta +=
                    network->synapses[i][j][k].delta;*/
            }
        }
    }
    /*for (size_t i = network->hiddenLayers+1; i > 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            network->neurons[i][j].bias.delta =
                network->neurons[i][j].bias.delta
                * network->momentum
                + network->learningRate
                * network->neurons[i][j].delta
                * network->neurons[i][j].value;

            network->neurons[i][j].bias.sumDelta =
                    network->neurons[i][j].bias.sumDelta
                    + network->neurons[i][j].bias.delta;
        }
    }*/
}

void updateWeights(Network *network) {
    for (size_t i = network->hiddenLayers; i+1 > 0; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {
                network->synapses[i][j][k].weight =
                    network->synapses[i][j][k].weight
                    + network->synapses[i][j][k].sumDelta;
            }
        }
    }
    /*for (size_t i = network->hiddenLayers+1; i > 1; i--) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            network->neurons[i][j].bias.weight =
                network->neurons[i][j].bias.weight
                + network->neurons[i][j].bias.sumDelta;
        }
    }*/
}

size_t learning(
    Network *network,
    size_t nbData,
    double **in,
    double **out,
    double learningRate,
    double errorMax,
    double momentum,
    size_t maxEpochs) {

    network->learningRate = learningRate;
    network->errorMax = errorMax;
    network->momentum = momentum;
    network->maxEpochs = maxEpochs;

    //double *results;
    double error = 0.0;

    size_t epochs = 0;

    size_t ii = 0;

    /*size_t bestE = 0;
    double bestError = -1.0;*/

    //printWeights(network);

    size_t last = 0;

    for (; epochs < maxEpochs; epochs++) {
        error = 0.0;
        //shuffleArray(in, in + network->layersSizes[0]);

        size_t nbGood = 0;

        #pragma omp parallel for
        for (size_t i = 0; i < nbData; i++) {
            Network *localNetwork = malloc(sizeof(struct Network));
            copyNetwork(localNetwork, network);

            double *results = forwardProp(localNetwork, in[i]);
            backProp(localNetwork, out[i], results);
           
            for (size_t i = network->hiddenLayers; i+1 > 0; i--) {
                for (size_t j = 0; j < network->layersSizes[i]; j++) {
                    for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {           
                        network->synapses[i][j][k].sumDelta +=
                            localNetwork->synapses[i][j][k].delta;
                   }
                }
            }

            subKillNetwork(localNetwork);
            free(localNetwork);
            
            
            size_t imax = 0;
            size_t wi = 0;
            double maxR = -1.0;

            /*for (size_t j=0;j<network->layersSizes[network->hiddenLayers+1];j++)
                error += pow(out[i][j] - results[j], 2) / 2;*/
            
            //#pragma omp parallel for reduction(+:error)
            for (size_t j=0;j<network->layersSizes[network->hiddenLayers+1];j++){  
                error += out[i][j] * log(results[j]) + (1 - out[i][j]) * log(1 - results[j]);
                if (out[i][j] == 1) wi = j;
                if (maxR == -1.0 || results[j] > maxR) {
                    maxR = results[j];
                    imax = j;
                }
            }

            if (wi == imax) nbGood++;


            free(results);
            //killNetwork(&localNetwork);
        }
        

        //error = error / nbData;
        error = -error / nbData;
        
        size_t percent = (100*nbGood)/nbData;
        printf("epoch : %zu \t\terror : %lf \t\tsucess : %zu%%\n", epochs, error, percent);
        
        if (percent > last && (percent > 85 || percent % 5 == 0)) {
            last = percent;
            saveNetwork(network, "FINAL.net");
            printf("SAVE YEAH !\n");           
        }

        /*if (bestError == -1.0 || error < bestError) {
            bestError = error;
            bestE = epochs;
        }*/

        if (error <= errorMax)
            break;

      
        updateWeights(network);
        reinitSumDelta(network);

       
        if (epochs == maxEpochs-1 && ii < 10) {
            break;
            ii ++;
            reinitNetwork(network);

            //printWeights(network);

            epochs = -1;
        }
    }

    printf("\nEpochs : %zu (%zu)\n", epochs, ii);
    return ii;
}

void learningFile(Network *network, char netPath[], char path[], size_t learn) {
    FILE *file = NULL;

    file = fopen(path, "r");
    if (file == NULL)
        errx(1, "CAN NOT OPEN THE FILE");

    int achar = 0;
    size_t passdot = 0;
    size_t ord = 0;

    int line = 0;

    size_t nbData = 0;
    double learningRate = 0.0;
    double errorMax = 0.0;
    double momentum = 0.0;
    size_t maxEpochs = 0;

    size_t learLearn = 0;
    double pad = 0.01;
    double bestLR = 0.0;
    double bestM = 0.0;
    size_t bestE = 0;

    double **in;
    double **out;

    do {
        achar = fgetc(file);
        if (achar == 10)
            nbData++;
    } while (achar != EOF);

    nbData = nbData - 4;

    achar = 0;
    rewind(file);

    fscanf(file, "%lf\n%lf\n%lf\n%zu\n",
        &learningRate, &errorMax, &momentum, &maxEpochs);

    in = malloc(sizeof(double) * nbData);
    for (size_t i = 0; i < nbData; i++)
        in[i] = malloc(sizeof(double) * network->layersSizes[0]);

    out = malloc(sizeof(double) * nbData);
    for (size_t i = 0; i < nbData; i++) {
        out[i] = malloc(
                    sizeof(double)
                    * network->layersSizes[network->hiddenLayers+1]);
    }

    do {
        achar = fgetc(file);
        if (achar == EOF)
            break;
        else if (achar == 10) {
            line++;
            ord = 0;
            passdot = 0;
        } else if (achar == 62) {
            passdot = 1;
            ord = 0;
        } else if (passdot == 1) {
            out[line][ord] = achar - 48;
            ord++;
        }
        else {
            in[line][ord] = achar - 48;
            ord++;
        }
    } while (achar != EOF);

    if (learn == 1 && learningRate == 0.0 && momentum == 0.0) {
        learLearn = 1;
        printf("\nBRUTE FORCE ACTIVATED - try to find best parameters\n");
        learningRate = pad;
        momentum = pad;
        bestLR = learningRate;
        bestM = momentum;
        bestE = maxEpochs + 1;

        for (; learningRate < 1.0 - pad; learningRate += pad) {
            momentum = pad;
            for (; momentum < 1.0 - pad; momentum += pad) {
                Network newnetwork = loadNetwork(netPath);
                //network = &newnetwork;
                size_t epochs = learning(
                                    &newnetwork,
                                    nbData,
                                    in,
                                    out,
                                    learningRate,
                                    errorMax,
                                    momentum,
                                    maxEpochs);

                printf("\tLR : %f \tM : %f", learningRate, momentum);

                if (epochs < bestE) {
                    bestLR = learningRate;
                    bestM = momentum;
                    bestE = epochs;
                }
                killNetwork(&newnetwork);
            }
        }

        learningRate = bestLR;
        momentum = bestM;

        printf("\n\nBest Learning Rate : %f\n", learningRate);
        printf("Best Momentum : %f\n", momentum);
    } 
    else if (learn == 1) {
        
        printf("\nLEARNING DATA : \n");
        printf("    Number of Data :    %zu\n", nbData);
        printf("    Learning Rate :     %f\n", learningRate);
        printf("    Error Maximum :     %f\n", errorMax);
        printf("    Momentum :          %f\n", momentum);
        printf("    Maximum of Epochs : %zu\n", maxEpochs);
        
        printf("\nSTART LEARNING");
        clock_t t;
        t = clock();

        size_t epochs = learning(
                            network,
                            nbData,
                            in,
                            out,
                            learningRate,
                            errorMax,
                            momentum,
                            maxEpochs);

        //printf("\nEpochs : %zu", epochs);
        epochs++;

        t = clock() - t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC;

        printf("\nSTOP LEARNING\n");

        printf("Time taken : %fs\n", time_taken);
    }

    if (learLearn == 0) {

        size_t nbGood = 0;

        printf("\nRESULTS : \n");

        for (size_t i = 0; i < nbData; i++) {
            printf("    ");
            
            /*for (size_t j = 0; j < network->layersSizes[0]; j++)
                printf("%d ", (int)in[i][j]);*/
            size_t reali = 0;
            for (size_t j = 0; j < network->layersSizes[network->hiddenLayers+1]; j++)
                if (out[i][j] == 1) reali = j;

            printf("%c ", network->outTrans[reali]);

            printf("-> ");

            double *results = forwardProp(network, in[i]);

            double max = pow(results[0], 2);
            size_t imax = 0;
            for (size_t j=1; j<network->layersSizes[network->hiddenLayers+1];j++){
                double cur = pow(results[j], 2);
                if (cur > max) {
                    max = cur;
                    imax = j;
                }
            }

            if (reali == imax) nbGood++;

            printf("%c",  network->outTrans[imax]);

            /*for (size_t j=0; j<network->layersSizes[network->hiddenLayers+1]-1;j++)
                printf("%f ", results[j]);
            printf("%f", results[network->layersSizes[network->hiddenLayers+1]-1]);*/

            printf("\n");
            free(results);
        }

        printf("SUCESS : %f%%\n", (100.0*nbGood)/nbData);
    }

    fclose(file);
    for (size_t i = 0; i < nbData; i++) {
        free(in[i]);
        free(out[i]);
    }
    free(in);
    free(out);
}

void createTraining(char filePath[], char netPath[], char imagePath[]) {
    FILE* file = NULL;

    Network network = loadNetwork(netPath);

    //printf("ah %zu\n", network.hiddenLayers);

    file = fopen(filePath, "r");

    if (file == NULL) {
        printf("the file does not exist. Creating file : \n");
        //fclose(file);
        file = fopen(filePath, "w+");

        double learningRate = 0.0;
        double errorMax = 0.0;
        double momentum = 0.0;
        size_t maxEpochs = 0;

        printf("Enter Learning Rate (double) : \n");
        scanf("%lf", &learningRate);
        fprintf(file, "%lf\n", learningRate);

        printf("Enter Error Maximum (double) : \n");
        scanf("\n%lf", &errorMax);
        fprintf(file, "%lf\n", errorMax);

        printf("Enter Momentum (double) : \n");
        scanf("\n%lf", &momentum);
        fprintf(file, "%lf\n", momentum);

        printf("Enter Maximum of Epochs (unsigned long) : \n");
        scanf("\n%zu", &maxEpochs);
        fprintf(file, "%zu\n", maxEpochs);

    } else {
        fclose(file);
        file = fopen(filePath, "a+");
    }

    init_sdl();
    SDL_Surface *surface = load_image(imagePath);
    otsu(surface);
    Image image = binarize(surface);
    SDL_FreeSurface(surface);
    cutLines(&image, 1, file, &network);
    fclose(file);
    killNetwork(&network);
}

void createFinalTraining(char filePath[], char netPath[]) {
    init_sdl();
    TTF_Init();

    Network network = loadNetwork(netPath);
    network = network;

    size_t fonts = 131;
    char *paths[] = {
        "./ttfs/Andale Mono.ttf",
        "./ttfs/AppleGothic.ttf",
        "./ttfs/Arial Black.ttf",
        "./ttfs/Arial Bold Italic.ttf",
        "./ttfs/Arial Bold.ttf",
        "./ttfs/Arial Italic.ttf",
        "./ttfs/Arial Narrow Bold Italic.ttf",
        "./ttfs/Arial Narrow Bold.ttf",
        "./ttfs/Arial Narrow Italic.ttf",
        "./ttfs/Arial Narrow.ttf",
        "./ttfs/Arial Rounded Bold.ttf",
        "./ttfs/Arial Unicode.ttf",
        "./ttfs/Arial.ttf",
        "./ttfs/Bradley Hand Bold.ttf",
        "./ttfs/COLLEGE.ttf",
        "./ttfs/COLLEGEB.ttf",
        "./ttfs/COLLEGEC.ttf",
        "./ttfs/COLLEGES.ttf",
        "./ttfs/Calibri Bold Italic.ttf",
        "./ttfs/Calibri Bold.ttf",
        "./ttfs/Calibri Italic.ttf",
        "./ttfs/Cambria Bold Italic.ttf",
        "./ttfs/Cambria Bold.ttf",
        "./ttfs/Cambria Italic.ttf",
        "./ttfs/Cambria.ttf",
        "./ttfs/Candara Bold Italic.ttf",
        "./ttfs/Candara Bold.ttf",
        "./ttfs/Candara Italic.ttf",
        "./ttfs/Candara.ttf",
        "./ttfs/Comic Sans MS Bold.ttf",
        "./ttfs/Comic Sans MS.ttf",
        "./ttfs/Consolas Bold Italic.ttf",
        "./ttfs/Consolas Bold.ttf",
        "./ttfs/Consolas Italic.ttf",
        "./ttfs/Constantia Bold Italic.ttf",
        "./ttfs/Constantia Bold.ttf",
        "./ttfs/Corbel Bold Italic.ttf",
        "./ttfs/Corbel Bold.ttf",
        "./ttfs/Corbel Italic.ttf",
        "./ttfs/Courier New Bold Italic.ttf",
        "./ttfs/Courier New Bold.ttf",
        "./ttfs/Courier New.ttf",
        "./ttfs/DIN Alternate Bold.ttf",
        "./ttfs/DejaVuMathTeXGyre.ttf",
        "./ttfs/DejaVuSans-Bold.ttf",
        "./ttfs/DejaVuSans-BoldOblique.ttf",
        "./ttfs/DejaVuSans-Oblique.ttf",
        "./ttfs/DejaVuSans.ttf",
        "./ttfs/DejaVuSansCondensed-Bold.ttf",
        "./ttfs/DejaVuSansCondensed-BoldOblique.ttf",
        "./ttfs/DejaVuSansCondensed-Oblique.ttf",
        "./ttfs/DejaVuSansCondensed.ttf",
        "./ttfs/DejaVuSansMono-Bold.ttf",
        "./ttfs/DejaVuSansMono-BoldOblique.ttf",
        "./ttfs/DejaVuSansMono-Oblique.ttf",
        "./ttfs/DejaVuSansMono.ttf",
        "./ttfs/DejaVuSerif-Bold.ttf",
        "./ttfs/DejaVuSerif-BoldItalic.ttf",
        "./ttfs/DejaVuSerif-Italic.ttf",
        "./ttfs/DejaVuSerif.ttf",
        "./ttfs/DejaVuSerifCondensed-Bold.ttf",
        "./ttfs/DejaVuSerifCondensed-BoldItalic.ttf",
        "./ttfs/DejaVuSerifCondensed-Italic.ttf",
        "./ttfs/DejaVuSerifCondensed.ttf",
        "./ttfs/Georgia Bold Italic.ttf",
        "./ttfs/Georgia Bold.ttf",
        "./ttfs/Georgia.ttf",
        "./ttfs/Herculanum.ttf",
        "./ttfs/Impact.ttf",
        "./ttfs/Keyboard.ttf",
        "./ttfs/Khmer Sangam MN.ttf",
        "./ttfs/Krungthep.ttf",
        "./ttfs/Lao Sangam MN.ttf",
        "./ttfs/Microsoft Sans Serif.ttf",
        "./ttfs/SFNSDisplay.ttf",
        "./ttfs/SFNSText.ttf",
        "./ttfs/SFNSTextItalic.ttf",
        "./ttfs/Silom.ttf",
        "./ttfs/Tahoma Bold.ttf",
        "./ttfs/Tahoma.ttf",
        "./ttfs/Trebuchet MS Bold Italic.ttf",
        "./ttfs/Trebuchet MS Bold.ttf",
        "./ttfs/Trebuchet MS Italic.ttf",
        "./ttfs/Trebuchet MS.ttf",
        "./ttfs/Verdana Bold Italic.ttf",
        "./ttfs/Verdana Bold.ttf",
        "./ttfs/Verdana Italic.ttf",
        "./ttfs/Verdana.ttf",
        "./ttfs/AmericanTypewriter.ttc",
        "./ttfs/AquaKana.ttc",
        "./ttfs/Athelas.ttc",
        "./ttfs/Avenir Next Condensed.ttc",
        "./ttfs/Avenir Next.ttc",
        "./ttfs/Bangla Sangam MN.ttc",
        "./ttfs/Chalkboard.ttc",
        "./ttfs/ChalkboardSE.ttc",
        "./ttfs/Charter.ttc",
        "./ttfs/Copperplate.ttc",
        "./ttfs/Devanagari Sangam MN.ttc",
        "./ttfs/EuphemiaCAS.ttc",
        "./ttfs/Futura.ttc",
        "./ttfs/Gurmukhi Sangam MN.ttc",
        "./ttfs/Helvetica.ttc",
        "./ttfs/HelveticaNeue.ttc",
        "./ttfs/HelveticaNeueDeskInterface.ttc",
        "./ttfs/Hiragino Sans GB.ttc",
        "./ttfs/Hoefler Text.ttc",
        "./ttfs/InaiMathi-MN.ttc",
        "./ttfs/Kannada Sangam MN.ttc",
        "./ttfs/Kefa.ttc",
        "./ttfs/Malayalam Sangam MN.ttc",
        "./ttfs/Marion.ttc",
        "./ttfs/MarkerFelt.ttc",
        "./ttfs/Menlo.ttc",
        "./ttfs/Myanmar Sangam MN.ttc",
        "./ttfs/Noteworthy.ttc",
        "./ttfs/Oriya Sangam MN.ttc",
        "./ttfs/PTMono.ttc",
        "./ttfs/PTSans.ttc",
        "./ttfs/PTSerifCaption.ttc",
        "./ttfs/Palatino.ttc",
        "./ttfs/Phosphate.ttc",
        "./ttfs/PingFang.ttc",
        "./ttfs/STHeiti Light.ttc",
        "./ttfs/STHeiti Medium.ttc",
        "./ttfs/Shree714.ttc",
        "./ttfs/Sinhala Sangam MN.ttc",
        "./ttfs/Songti.ttc",
        "./ttfs/SuperClarendon.ttc",
        "./ttfs/Tamil Sangam MN.ttc",
        "./ttfs/Telugu Sangam MN.ttc"
    };


    FILE* file = NULL;
    file = fopen(filePath, "r");
    if (file == NULL) {
        printf("the file does not exist. Creating file : \n");
        //fclose(file);
        file = fopen(filePath, "w+");

        double learningRate = 0.0;
        double errorMax = 0.0;
        double momentum = 0.0;
        size_t maxEpochs = 0;

        printf("Enter Learning Rate (double) : \n");
        scanf("%lf", &learningRate);
        fprintf(file, "%lf\n", learningRate);

        printf("Enter Error Maximum (double) : \n");
        scanf("\n%lf", &errorMax);
        fprintf(file, "%lf\n", errorMax);

        printf("Enter Momentum (double) : \n");
        scanf("\n%lf", &momentum);
        fprintf(file, "%lf\n", momentum);

        printf("Enter Maximum of Epochs (unsigned long) : \n");
        scanf("\n%zu", &maxEpochs);
        fprintf(file, "%zu\n", maxEpochs);

    } else {
        fclose(file);
        file = fopen(filePath, "a+");
    }

    SDL_Color white = {255, 255, 255, 0};
    SDL_Color black = {0, 0, 0, 0};

    for (size_t i = 0; i < fonts; i++) {
        TTF_Font *font = NULL;
        SDL_Surface *surface = NULL;
        font = TTF_OpenFont(paths[i], 70);

        surface = TTF_RenderText_Shaded(font, "!   #   $   %   &   (   )   *   +   -   .   /   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?   @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z   [   \\   ]   ^   _   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o   p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~", black, white);
        //greyscale(surface);
        //otsu(surface);

        /*display_image(surface);
        display_image(surface);*/

        Image image = binarize(surface);
        printf("CUT : %s\t\t\t", paths[i]);
        cutLines(&image, 3, file, &network);

        TTF_CloseFont(font);
        SDL_FreeSurface(surface);
        killImage(&image);
    }

    TTF_Quit();
    SDL_Quit();
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
    //double sum = 0.0;
    for (size_t i = 0; i < network->hiddenLayers + 1; i++) {
        for (size_t j = 0; j < network->layersSizes[i]; j++) {
            for (size_t k = 0; k < network->layersSizes[i + 1]; k++) {
                //sum += network->synapses[i][j][k].weight;
                printf("%f\t", network->synapses[i][j][k].weight);
            }
        }
    }
    //printf(":: %f\t", sum);
}

void saveNetwork(Network *network, char path[]) {
    FILE * file = fopen(path, "w");
    if (file == NULL)
        printf("ERROR: WRONG PATH\n");

    fprintf(file, "%ld\n", network->hiddenLayers);

    fprintf(file, "%ld\n", network->logisticId);

    for(unsigned int i = 0; i < (network->hiddenLayers + 2) ; i++)
        fprintf(file, "%ld/", network->layersSizes[i]);

    fprintf(file,"\n");

    for(unsigned int i = 0; i < network->hiddenLayers + 1; i++){
        for(unsigned int j = 0; j < network->layersSizes[i]; j++){
            for(size_t  k = 0; k < network->layersSizes[i+1]; k++)
                fprintf(file, "/%f", network->synapses[i][j][k].weight);
        }
    }

    fprintf(file, "\n");

    for(unsigned int i = 0; i < (network->hiddenLayers + 2); i++){
        for(unsigned int j = 0; j < network->layersSizes[i]; j++)
            fprintf(file, "%f/",network->neurons[i][j].bias.weight);
    }
    fprintf(file,"\n");

    for(unsigned int i = 0;
        i < network->layersSizes[network->hiddenLayers + 1]; i++)
        fprintf(file, "%c", network->outTrans[i]);
    fclose(file);
}

Network loadNetwork(char* path){
    unsigned int hiddenLayers = 0;
    unsigned int id = 0;
    FILE * file = fopen(path,"r");
    if(file == NULL)
        errx(1, "file not found");
    fscanf(file,"%d\n", &hiddenLayers);
    fscanf(file,"%d\n", &id);
    size_t * layersSizes = malloc(sizeof(size_t)*(hiddenLayers+2));

    for(unsigned int i = 0; i < hiddenLayers + 1; i++)
        fscanf(file,"%zu/",&layersSizes[i]);

    fscanf(file, "%zu/\n",&layersSizes[hiddenLayers + 1]);
    char *outTrans = malloc(sizeof(char)*layersSizes[hiddenLayers + 1]);

    Network network = newNetwork(layersSizes, hiddenLayers, id, outTrans);
    for(size_t i = 0; i < hiddenLayers + 1; i++){
        for(size_t j = 0; j < layersSizes[i];j++){
            for(size_t k = 0; k < layersSizes[i+1]; k++)
                fscanf(file,"/%lf", &network.synapses[i][j][k].weight);
        }
    }
    fscanf(file, "\n%lf/", &network.neurons[0][0].bias.weight);
    for(unsigned int j = 1; j < layersSizes[0]; j++)
        fscanf(file, "%lf/", &network.neurons[0][j].bias.weight);
    for(unsigned int i = 1; i < hiddenLayers + 2; i++){
        for(unsigned int j = 0; j < layersSizes[i]; j++)
            fscanf(file, "%lf/",&network.neurons[i][j].bias.weight);
    }
    fscanf(file, "\n%c", &network.outTrans[0]);
    for(unsigned int i = 1; i < layersSizes[hiddenLayers+1]; i++)
        fscanf(file, "%c", &network.outTrans[i]);
    fclose(file);
    return network;
}
