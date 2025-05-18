#include "neural_network.h"

bool mutate_rate(double mut_rate);
double random_gauss(double std_dev);

bool mutate_rate(double mut_rate) {
    return ((double)rand() / RAND_MAX < mut_rate);
}

double random_gauss(double std_dev) {
    // Generate two uniform random numbers
    double u1 = (rand() + 1.0) / (RAND_MAX + 1.0);
    double u2 = (rand() + 1.0) / (RAND_MAX + 1.0);

    // Box-Muller transform
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

    return z0 * std_dev;
}

void variant_rand(struct NeuralNetwork *n, double int_dev) {
    int input_size = INPUTS;

    for (int layer = 0; layer < n->layers; layer++) {
        int layer_size = n->hidden[layer].size;
        for (int neuron = 0; neuron < layer_size; neuron++) {
            for (int input = 0; input < input_size; input++) {
                n->hidden[layer].weights[neuron][input] = random_gauss(int_dev);
            }
            n->hidden[layer].bias[neuron] = random_gauss(int_dev);
        }
        input_size = layer_size;
    }

    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        for (int input = 0; input < input_size; input++) {
            n->final_weights[neuron][input] = random_gauss(int_dev);
        }
        n->final_bias[neuron] = random_gauss(int_dev);
    }
}

void variant_mutate(struct NeuralNetwork *n, double mut_rate, double gau_dev) {
    n->generation++;
    int input_size = INPUTS;

    for (int layer = 0; layer < n->layers; layer++) {
        int layer_size = n->hidden[layer].size;
        for (int neuron = 0; neuron < layer_size; neuron++) {
            for (int input = 0; input < input_size; input++) {
                if (mutate_rate(mut_rate)) {
                    n->hidden[layer].weights[neuron][input] +=
                        random_gauss(gau_dev);
                }
            }
            if (mutate_rate(mut_rate)) {
                n->hidden[layer].bias[neuron] += random_gauss(gau_dev);
            }
        }
        input_size = layer_size;
    }

    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        for (int input = 0; input < input_size; input++) {
            if (mutate_rate(mut_rate)) {
                n->final_weights[neuron][input] += random_gauss(gau_dev);
            }
        }
        if (mutate_rate(mut_rate)) {
            n->final_bias[neuron] += random_gauss(gau_dev);
        }
    }

    n->mutate = false;
}

void network_update(struct NeuralNetwork *n) {
    int input_size = INPUTS;
    double *inputs = n->inputs;
    for (int layer = 0; layer < n->layers; layer++) {
        int layer_size = n->hidden[layer].size;

        // Process each neuron in the current layer.
        for (int neuron = 0; neuron < layer_size; neuron++) {
            double output = 0;

            // Multiply inputs by weights and it output.
            // Each input contributes to the neuron's weighted sum.
            for (int input = 0; input < input_size; input++) {
                output +=
                    inputs[input] * n->hidden[layer].weights[neuron][input];
            }

            // Add the bias to the weighted sum.
            // Bias shifts the weighted sum to improve learning flexibility.
            output += n->hidden[layer].bias[neuron];

            // Apply the ReLU (Rectified Linear Unit) activation function.
            // ReLU outputs 0 if the output is negative, otherwise the value
            // itself.
            n->hidden[layer].output[neuron] = (output > 0) ? output : 0;
        }

        // The current layers size is the next layers input size.
        input_size = layer_size;
        inputs = n->hidden[layer].output;
    }

    // Calculate output layer.
    for (int neuron = 0; neuron < OUTPUTS; neuron++) {
        double output = 0;
        for (int input = 0; input < input_size; input++) {
            output += inputs[input] * n->final_weights[neuron][input];
        }
        output += n->final_bias[neuron];

        // No longer a Hard Sigmoid or Step Function, activation function.
        n->final_output[neuron] = output;
        // printf("Output %d: %f\n", neuron, n->final_output[neuron]);
    }

    // binary selection mechanism. for better performance over hard sigmoid.
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
