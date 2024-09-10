#ifndef AI_H
#define AI_H

#include "game.h"
#include "neural_network.h"

struct AI {
        struct NeuralNetwork *top;
        struct NeuralNetwork *new_top;
        struct Game *games;
        int thread_count;
        int variant_count;
        int generation_count;
        int top_variant_count;
        int game_variant_count;
        int current_generation;
        bool gfx_on;
        bool ai_train;
};

bool ai_new(struct AI **ai, struct AIConfig config);
void ai_free(struct AI **ai);
bool ai_run(struct AI *a);

#endif
