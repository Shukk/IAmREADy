#include"neural.h"

Neuron newNeuron(size_t nbInputs) {
    Neuron neuron;

    neuron.value = 0.0;
    neuron.z = 0.0;
    neuron.delta = 0.0;
    neuron.bias = newSynapse(nbInputs);

    return neuron;
}

Synapse newSynapse(size_t nbInputs) {
    Synapse synapse;

    //synapse.weight = randomizeDouble(-1.0, 1.0);
    synapse.weight = gaussRandom(-(1 / sqrt(nbInputs)), (1 / sqrt(nbInputs)));
    synapse.delta = 0.0;

    return synapse;
}
