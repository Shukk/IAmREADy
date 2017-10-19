#include <string.h>
#include "neural.h"

/*void SaveNetwork(Network *network, char path[])
{
	FILE * file = fopen(path, "w");
	if (file == NULL){
		printf("ERROR: WRONG PATH\n");
	}
	fprintf(file, "{");
	fprintf(file, "%ld", network->hiddenLayers);
	fprintf(file, "<");

	for(unsigned int i = 0; i < (network->hiddenLayers + 2) ; i++){
		for(unsigned int j = 0; j < network->layersSize[i]){
		fprintf(file, "%ld", network->hiddenLayers[i]);
		fprintf(file, ";");
		}
	} 
	fprintf(file, ">");
	for(unsigned int i = 0; i < (network->hiddenLayers + 2); i++){
		int trigger = 1;
		for(unsigned int j = 0; j < network->layersSizes[i]; j++){
//			if (trigger){
//				fprintf(file, "%ld", network->layersSizes[i]);
//				trigger = 0;
//			}
//			fprintf(file, "(");
			for(unsigned int k = 0; k < network->layersSizes[i+1];
						k++){
			fprintf(file, "%f", network->synapses[i][j][k]).weight;
			fprintf(file, ";");
			}
			fprintf(file, ")");
		}
	}
	fprintf(file, "}");
}

Network LoadNetwork(char path[]){
	FILE *file = fopen(path, "r");
	If (file == NULL){
		printf("ERROR: WRONG PATH");
	}
	fseek(file, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(file);
	char tmp[len];	
	fscanf(file, "%c", res);
	unsigned int hiddenLayers = 2;
	unsigned int *index = 1;
	if (res[0] == "{"){
		hiddenLayers += charToInt(tmp[], index);
		unsigned int *layersSizes[hiddenLayers];	
		unsigned int i = 0;
		while (res[index] != ">"){
			layersSize[i] = charToInt(tmp[], index);
			i += 1;
			index += 1;
		}
		i = 0;
		Network network = newNetwork(layersSizes, hiddenLayers)
		while (res[index] != "}"){
			for (unsigned int j = 0; j < layersSizes[i]; j++){
				for (unsigned int k = 0; k < layersSizes[i+1];
							k++){
					network->synapses[i][j][k].weight =
						charToInt(tmp[], index);
				}
			}
//			i = (res[index] in [")",";"])? i : i + 1;
			i += 1;
			index += 1;
		}
	}
}
//char sep = ";"
double charToInt(char tmp[], unsigned int *index,)
{
	double res = 0;
	int ord = 1;
	int isFloat = 0;
	//sep,
	while (tmp[index] in ["(",")",">","}"]){
		if (res[index] == ".")
			isFloat = 1;
		else {
			res = (isFloat)? res + (tmp[index] - "0") / ord 
					: res * ord + (tmp[index] - "0");
		}
		ord *= 10;
		index += 1;
	}
	return res;
}
*/




















