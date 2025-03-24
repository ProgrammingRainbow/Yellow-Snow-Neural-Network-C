#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "main.h"

struct Layer {
        int size;
        double weights[LAYER_SIZE][LAYER_SIZE];
        double bias[LAYER_SIZE];
        double output[LAYER_SIZE];
};

struct NeuralNetwork {
        double inputs[INPUTS];

        struct Layer hidden[LAYERS];

        double final_weights[OUTPUTS][LAYER_SIZE];
        double final_bias[OUTPUTS];
        double final_output[OUTPUTS];
        bool output[OUTPUTS];

        int layers;
        int fitness;
        int generation;
};

void variant_rand(struct NeuralNetwork *n);
void variant_mutate(struct NeuralNetwork *n, double mut_rate, double mut_rang);
void network_update(struct NeuralNetwork *n);

#endif
