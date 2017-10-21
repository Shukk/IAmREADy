#ifndef _NEURAL_H_                                                 
#define _NEURAL_H_ 

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<err.h>

#include"../detection.h"

//--------------------
//       NEURON
//--------------------

typedef struct Synapse Synapse;
struct Synapse {
    double weight;
    double delta;
};

//return a new and fresh synapse
Synapse newSynapse();


typedef struct Neuron Neuron;
struct Neuron {
    //represent a
    double value;
    double z;
    double delta;
    Synapse bias;
};

//return a new and fresh neuron
Neuron newNeuron();


//--------------------
//		NETWORK
//--------------------

//function "structure" in order to add functions like sigmoid
typedef double (*func_t)(double);

//NETWORK structure
typedef struct Network Network;
struct Network {
    double learningRate;
    double errorMax;
    double momentum;
    size_t maxEpochs;

    size_t hiddenLayers;

    //in order to loop in layersSizes you need to add 2 to hiddenLayers
    //due to the input layer
    size_t *layersSizes;

    Neuron **neurons;
    Synapse ***synapses;

    size_t logisticId;

    func_t logistic;
    func_t logisticPrime;
};

//return a new network
//first element of layersSize is the number of inputs
//all the element after the first one are the nb of neurons by hidden layers
//the last element is the number of output
//parameter hiddenLayers represent the number of hidden Layers
//EXAMPLE :
//  double sigmoid(double x) {return 1 / (1 + exp(-x))};
//  size_t tab[2] = {2,1,1};
//  Network net = newNetwork(&tab, 1, sigmoid);
Network newNetwork(
    size_t *layersSizes, 
    size_t hiddenLayers, 
    size_t logisticId);

void reinitNetwork(Network *network);

//free all memory spaces use by the Network so kill it
//EXAMPLE :
// Network network = newNetwork(...);
// killNetwork(&network);
void killNetwork(Network *network);

//execute a forward propagation with the given inputs
//EXAMPLE in a network with 2 inputs:
//  size_t inputs[2] = {1, 1};
//  double result = forwardProp(&network, &inputs);
double forwardProp(Network *network, double *inputs);

//execute the backpropagation
//outpout is the outpout of a forward prop
//EXAMPLE
// backprob(&network, 0, 1)
void backProp(Network *network, double wantedOutput, double output);

void updateWeightsDelta(Network *network);
void updateWeights(Network *network);

void learning(
    Network *network, 
    size_t nbData, 
    double **in, 
    double *out,
    double learningRate,
    double errorMax,
    double momentum,
    size_t maxEpochs);

void learningFile(Network *network, char path[]);

void createTraining(char filePath[], char imagePath[]);

//print the network in the console
void printNetwork(Network *network);
void printWeights(Network *network);

//TODELETE
void testXOR();
void testAND();


//--------------------
//     FUNCTIONS
//--------------------

//ID = 0
double sigmoid(double x);
double sigmoidPrime(double x);

//ID = 1
double linear(double x);
double linearPrime(double x);

//ID = 2
double tanh(double x);
double tanhPrime(double x);

func_t getFunc(size_t id);
func_t getFuncPrime(size_t id);

#endif