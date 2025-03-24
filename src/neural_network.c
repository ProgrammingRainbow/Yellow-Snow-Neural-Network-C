#include "neural_network.h"

bool mutate_rate(double mut_rate);
double mutate_rang(double mut_rang);
double mutate_rand(void);

bool mutate_rate(double mut_rate) {
    return ((double)rand() / RAND_MAX < mut_rate);
}

double mutate_rang(double mut_rang) {
    return ((double)rand() / RAND_MAX) * 2 * mut_rang - mut_rang;
}

double mutate_rand(void) { return ((double)rand() / RAND_MAX) * 2 - 1; }

void variant_rand(struct NeuralNetwork *n) {
    int input_size = INPUTS;

    for (int layer = 0; layer < n->layers; layer++) {
        int layer_size = n->hidden[layer].size;
        for (int neuron = 0; neuron < layer_size; neuron++) {
            for (int input = 0; input < input_size; input++) {
                n->hidden[layer].weights[neuron][input] = mutate_rand();
            }
            n->hidden[layer].bias[neuron] = mutate_rand();
        }
        input_size = layer_size;
    }

    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        for (int input = 0; input < input_size; input++) {
            n->final_weights[neuron][input] = mutate_rand();
        }
        n->final_bias[neuron] = mutate_rand();
    }
}
void variant_mutate(struct NeuralNetwork *n, double mut_rate, double mut_rang) {
    n->generation++;
    int input_size = INPUTS;

    for (int layer = 0; layer < n->layers; layer++) {
        int layer_size = n->hidden[layer].size;
        for (int neuron = 0; neuron < layer_size; neuron++) {
            for (int input = 0; input < input_size; input++) {
                if (mutate_rate(mut_rate)) {
                    n->hidden[layer].weights[neuron][input] +=
                        mutate_rang(mut_rang);
                }
            }
            if (mutate_rate(mut_rate)) {
                n->hidden[layer].bias[neuron] += mutate_rang(mut_rang);
            }
        }
        input_size = layer_size;
    }

    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        for (int input = 0; input < input_size; input++) {
            if (mutate_rate(mut_rate)) {
                n->final_weights[neuron][input] += mutate_rang(mut_rang);
            }
        }
        if (mutate_rate(mut_rate)) {
            n->final_bias[neuron] += mutate_rang(mut_rang);
        }
    }
}

void network_update(struct NeuralNetwork *n) {
    int input_size = INPUTS;
    double *inputs = n->inputs;
    for (int layer = 0; layer < n->layers; layer++) {
        int layer_size = n->hidden[layer].size;
        for (int neuron = 0; neuron < layer_size; neuron++) {
            double output = 0;
            for (int input = 0; input < input_size; input++) {
                output +=
                    inputs[input] * n->hidden[layer].weights[neuron][input];
                // printf("neuron = %i\n", neuron);
                // printf("input = %i\n", input);
            }
            output += n->hidden[layer].bias[neuron];

            // ReLU (Rectified Linear Unit) activation function. Just 0 if
            // negative.
            n->hidden[layer].output[neuron] = (output > 0) ? output : 0;
        }
        input_size = layer_size;
        inputs = n->hidden[layer].output;
    }

    // Calculate output layer.
    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        double output = 0;
        for (int input = 0; input < input_size; input++) {
            // printf("neuron = %i\n", neuron);
            // printf("input = %i\n", input);
            // printf("inputs[input] = %f\n", inputs[input]);
            // printf("n->final_weights[neuron][input] = %f\n",
            //        n->final_weights[neuron][input]);
            output += inputs[input] * n->final_weights[neuron][input];
        }
        output += n->final_bias[neuron];

        // Hard Sigmoid or Step Function, activation function. 1 for over 0
        // otherwise 0.
        n->final_output[neuron] = output;
    }

    n->output[0] = false;
    n->output[1] = false;
    if (n->final_output[0] > 0 || n->final_output[1] > 0) {
        if (n->final_output[0] > n->final_output[1]) {
            n->output[0] = true;
        } else {
            n->output[1] = true;
        }
    }
}
