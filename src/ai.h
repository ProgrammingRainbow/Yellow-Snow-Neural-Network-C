#ifndef AI_H
#define AI_H

#include "game.h"
#include "neural_network.h"

struct AI {
        struct NeuralNetwork *top;
        struct NeuralNetwork *new_top;
        struct Game *games;
        int current_generation;
};

bool ai_new(struct AI **ai, const struct AIConfig *config);
void ai_free(struct AI **ai, const struct AIConfig *config);
bool ai_run(struct AI *a, const struct AIConfig *config);

#endif
