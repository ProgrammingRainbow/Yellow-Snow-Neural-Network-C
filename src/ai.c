#include "ai.h"

void networks_populate(struct AI *a);
void networks_load(struct AI *a);
void networks_save(struct AI *a);
void networks_get_top(struct AI *a);

bool ai_new(struct AI **ai, struct AIConfig ai_config) {
    *ai = calloc(1, sizeof(struct AI));
    if (!*ai) {
        fprintf(stderr, "Error in calloc of AI!\n");
        return false;
    }
    struct AI *a = *ai;

    a->thread_count = ai_config.thread_count;
    a->variant_count = ai_config.variant_count;
    a->generation_count = ai_config.generation_count;
    a->top_variant_count = ai_config.top_variant_count;
    a->game_variant_count = ai_config.game_variant_count;
    a->gfx_on = ai_config.gfx_on;
    a->ai_train = ai_config.ai_train;

    a->top = calloc((size_t)a->top_variant_count, sizeof(struct NeuralNetwork));
    if (!a->top) {
        fprintf(stderr, "Error in calloc of NeuralNetwork array!\n");
        return false;
    }

    a->games = calloc((size_t)a->thread_count, sizeof(struct Game));
    if (!a->games) {
        fprintf(stderr, "Error in calloc of Game array!\n");
        return false;
    }

    networks_load(a);

    SDL_Delay(1000);

    for (int thread = 0; thread < a->thread_count; thread++) {
        if (!game_new(&a->games[thread], ai_config)) {
            return false;
        }
    }

    return true;
}

void ai_free(struct AI **ai) {
    if (*ai) {
        struct AI *a = *ai;

        for (int i = 0; i < a->thread_count; i++) {
            game_free(&a->games[i]);
        }

        if (a->top) {
            free(a->top);
            a->top = NULL;
        }

        if (a->games) {
            free(a->games);
            a->games = NULL;
        }

        free(a);
        a = NULL;
        *ai = NULL;
    }
}

void networks_load(struct AI *a) {
    FILE *file = fopen(SAVED_NETWORKS, "rb");
    if (file != NULL) {

        // Calculate how many NeuralNetwork structs are in the file
        fseek(file, 0, SEEK_END);
        size_t file_size = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);
        size_t network_count = file_size / sizeof(struct NeuralNetwork);

        // Read the data into the array
        fread(a->top, sizeof(struct NeuralNetwork), network_count, file);
        fclose(file);

        printf("Loaded %lu networks from the file %s.\n", network_count,
               SAVED_NETWORKS);

        /*network_populate(a, network_count);*/

    } else {
        printf("File %s cannot be opened. Generating new Neural Networks\n",
               SAVED_NETWORKS);

        for (int i = 0; i < a->top_variant_count; i++) {
            variant_rand(&a->top[i]);
        }
    }
}

void networks_populate(struct AI *a) {

    int top_per_thread = a->top_variant_count / a->thread_count;
    int var_per_top_per_thread = a->game_variant_count / top_per_thread;

    int top_var = 0;
    // for each game.
    for (int game = 0; game < a->thread_count; game++) {
        // for top_vars / game.
        int game_var = 0;
        for (int i = 0; i < top_per_thread; i++) {
            a->games[game].networks[game_var] = a->top[top_var];
            a->games[game].networks[game_var].fitness = 0;
            game_var++;

            for (int j = 1; j < var_per_top_per_thread; j++) {
                a->games[game].networks[game_var] = a->top[top_var];
                a->games[game].networks[game_var].fitness = 0;
                variant_mutate(&a->games[game].networks[game_var]);
                game_var++;
            }

            top_var++;
        }
    }
}

void networks_save(struct AI *a) {
    FILE *file = fopen(SAVED_NETWORKS, "wb");
    if (file != NULL) {
        fwrite(a->top, sizeof(struct NeuralNetwork),
               (size_t)a->top_variant_count, file);
        fclose(file);
        printf("Saved %d to file %s.\n", a->top_variant_count, SAVED_NETWORKS);
    } else {
        fprintf(stderr, "Error opening %s for writing\n", SAVED_NETWORKS);
    }
}

void networks_get_top(struct AI *a) {
    memset(a->top, 0,
           (size_t)a->top_variant_count * sizeof(struct NeuralNetwork));
    for (int game = 0; game < a->thread_count; game++) {
        for (int game_var = 0; game_var < a->game_variant_count; game_var++) {
            int fitness = a->games[game].networks[game_var].fitness;
            for (int top_var = 0; top_var < a->top_variant_count; top_var++) {
                if (fitness > a->top[top_var].fitness) {
                    for (int i = a->top_variant_count - 1; i > top_var; i--) {
                        a->top[i] = a->top[i - 1];
                    }
                    a->top[top_var] = a->games[game].networks[game_var];
                    break;
                }
            }
        }
    }
}

bool ai_run(struct AI *a) {
    pthread_t threads[a->thread_count];

    for (int gen = 0; gen < a->generation_count; gen++) {
        networks_populate(a);
        if (a->gfx_on) {
            /*a->games[0].networks[0] = a->top[0];*/
            game_run(&a->games[0]);
        } else {
            for (int thread = 0; thread < a->thread_count; thread++) {
                /*printf("game %d\n", game);*/
                pthread_create(&threads[thread], NULL, game_run_multi,
                               &a->games[thread]);
            }

            for (int thread = 0; thread < a->thread_count; thread++) {
                pthread_join(threads[thread], NULL);
            }
        }

        if (a->ai_train) {
            networks_get_top(a);

            for (int i = 0; i < a->top_variant_count; i++) {
                printf("fitness: %d, generation: %d\n", a->top[i].fitness,
                       a->top[i].generation);
            }

            printf("generation: %d\n", gen + 1);
        }
    }

    if (a->ai_train || !a->gfx_on) {
        networks_save(a);
    }

    return true;
}
