#include "neural_network.h"

void variant_rand(struct NeuralNetwork *n) {
    for (int i = 0; i < FIRST_HIDDEN; i++) {
        for (int j = 0; j < INPUTS; j++) {
            n->first_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
        n->first_bias[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }

    for (int i = 0; i < SECOND_HIDDEN; i++) {
        for (int j = 0; j < FIRST_HIDDEN; j++) {
            n->second_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
        n->second_bias[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }

    for (int i = 0; i < OUTPUTS; i++) {
        for (int j = 0; j < SECOND_HIDDEN; j++) {
            n->final_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
        n->final_bias[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }
}

void variant_mutate(struct NeuralNetwork *n) {
    n->generation++;
    for (int i = 0; i < FIRST_HIDDEN; i++) {
        for (int j = 0; j < INPUTS; j++) {
            if ((double)rand() / RAND_MAX < MUTATION_PROB) {
                n->first_weights[i][j] +=
                    ((double)rand() / RAND_MAX) * 2 * MUTATION_RATE -
                    MUTATION_RATE;
            }
        }
        if ((double)rand() / RAND_MAX < MUTATION_PROB) {
            n->first_bias[i] +=
                ((double)rand() / RAND_MAX) * 2 * MUTATION_RATE - MUTATION_RATE;
        }
    }

    for (int i = 0; i < SECOND_HIDDEN; i++) {
        for (int j = 0; j < FIRST_HIDDEN; j++) {
            if ((double)rand() / RAND_MAX < MUTATION_PROB) {
                n->second_weights[i][j] +=
                    ((double)rand() / RAND_MAX) * 2 * MUTATION_RATE -
                    MUTATION_RATE;
            }
        }
        if ((double)rand() / RAND_MAX < MUTATION_PROB) {
            n->second_bias[i] +=
                ((double)rand() / RAND_MAX) * 2 * MUTATION_RATE - MUTATION_RATE;
        }
    }

    for (int i = 0; i < OUTPUTS; i++) {
        for (int j = 0; j < SECOND_HIDDEN; j++) {
            if ((double)rand() / RAND_MAX < MUTATION_PROB) {
                n->final_weights[i][j] +=
                    ((double)rand() / RAND_MAX) * 2 * MUTATION_RATE -
                    MUTATION_RATE;
            }
        }
        if ((double)rand() / RAND_MAX < MUTATION_PROB) {
            n->final_bias[i] +=
                ((double)rand() / RAND_MAX) * 2 * MUTATION_RATE - MUTATION_RATE;
        }
    }
}

void network_update(struct NeuralNetwork *n) {

    // Calculate first hidden layer.
    for (int neuron = 0; neuron < FIRST_HIDDEN; neuron++) {
        double output = 0;
        for (int input = 0; input < INPUTS; input++) {
            output += n->inputs[input] * n->first_weights[neuron][input];
        }
        output += n->first_bias[neuron];

        // ReLU (Rectified Linear Unit) activation function. Just 0 if negative.
        n->first_output[neuron] = (output > 0) ? output : 0;
    }

    // Calculate second hidden layer.
    for (int neuron = 0; neuron < SECOND_HIDDEN; neuron++) {
        double output = 0;
        for (int input = 0; input < FIRST_HIDDEN; input++) {
            output += n->first_output[input] * n->second_weights[neuron][input];
        }
        output += n->second_bias[neuron];

        // ReLU (Rectified Linear Unit) activation function. Just 0 if negative.
        n->second_output[neuron] = (output > 0) ? output : 0;
    }

    // Calculate output layer.
    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        double output = 0;
        for (int input = 0; input < SECOND_HIDDEN; input++) {
            output += n->second_output[input] * n->final_weights[neuron][input];
        }
        output += n->final_bias[neuron];

        // Hard Sigmoid or Step Function, activation function. 1 for over 0
        // otherwise 0.
        n->final_output[neuron] = (output > 0) ? true : false;
    }
}
