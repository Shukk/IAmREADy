#include"neural.h"

Neuron newNeuron(size_t nbInputs) {
    Neuron neuron;

    neuron.value = 0.0;
    neuron.z = 0.0;
    neuron.delta = 0.0;
    neuron.bias = newSynapse(nbInputs);
    neuron.bias.weight = 1.0;

    return neuron;
}

Synapse newSynapse(size_t nbInputs) {
    Synapse synapse;

    //synapse.weight = randomizeDouble(-°.0, 1.0);

    synapse.weight = randDouble(-(1 / sqrt(nbInputs)), (1/sqrt(nbInputs)));

    //synapse.weight = randDouble(-1.0, 1.0);
    //nbInputs++;
    /*synapse.weight = gaussRandom(-1.0, 1.0);*/

    
    //synapse.weight = 0.0;
    synapse.delta = 0.0;
    synapse.sumDelta = 0.0;

    return synapse;
}


Neuron copyNeuron(Neuron n) {
    Neuron neuron;

    neuron.value = n.value;
    neuron.z = n.z;
    neuron.delta = n.delta;

    return neuron;
}

Synapse copySynapse(Synapse s) {
    Synapse synapse;

    synapse.weight = s.weight;

    synapse.delta = s.delta;
    synapse.sumDelta = s.delta;

    return synapse;
}
