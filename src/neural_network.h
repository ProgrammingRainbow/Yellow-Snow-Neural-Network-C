#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "main.h"

struct NeuralNetwork {
        double inputs[INPUTS];
        double first_weights[FIRST_HIDDEN][INPUTS];
        double first_bias[FIRST_HIDDEN];
        double first_output[FIRST_HIDDEN];
        double second_weights[SECOND_HIDDEN][FIRST_HIDDEN];
        double second_bias[SECOND_HIDDEN];
        double second_output[SECOND_HIDDEN];
        double final_weights[OUTPUTS][SECOND_HIDDEN];
        double final_bias[OUTPUTS];
        bool final_output[OUTPUTS];
        int fitness;
        int generation;
};

void network_update(struct NeuralNetwork *neural_network);
void variant_mutate(struct NeuralNetwork *n);
void variant_rand(struct NeuralNetwork *n);

#endif
