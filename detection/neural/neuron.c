#include"neural.h"

Neuron newNeuron() {
	Neuron neuron;

	neuron.value = 0.0;
	neuron.z = 0.0;
	neuron.delta = 0.0;
	neuron.bias = newSynapse();

	return neuron;
}

Synapse newSynapse() {
	Synapse synapse;

	//synapse.weight = randomizeDouble(-1.0, 1.0);
	synapse.weight = gaussRandom(-1.0, 1.0);
	synapse.delta = 0.0;

	return synapse;
}