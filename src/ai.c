#include "ai.h"

bool networks_load(struct AI *a, const struct AIConfig *config);
void networks_populate(struct AI *a, const struct AIConfig *config);
void networks_save(struct AI *a, const struct AIConfig *config);
void networks_get_top(struct AI *a, const struct AIConfig *config);

bool ai_new(struct AI **ai, const struct AIConfig *config) {
    *ai = calloc(1, sizeof(struct AI));
    if (!*ai) {
        fprintf(stderr, "Error in calloc of AI!\n");
        return false;
    }
    struct AI *a = *ai;

    a->top =
        calloc((size_t)config->top_variant_count, sizeof(struct NeuralNetwork));
    if (!a->top) {
        fprintf(stderr, "Error in calloc of NeuralNetwork array!\n");
        return false;
    }

    a->games = calloc((size_t)config->thread_count, sizeof(struct Game));
    if (!a->games) {
        fprintf(stderr, "Error in calloc of Game array!\n");
        return false;
    }

    if (config->ai_on) {
        if (!networks_load(a, config)) {
            return false;
        }
        SDL_Delay(INFO_DELAY);
    }

    for (int thread = 0; thread < config->thread_count; thread++) {
        if (!game_new(&a->games[thread], config)) {
            return false;
        }
    }

    return true;
}

void ai_free(struct AI **ai, const struct AIConfig *config) {
    if (*ai) {
        struct AI *a = *ai;

        for (int i = 0; i < config->thread_count; i++) {
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

bool networks_load(struct AI *a, const struct AIConfig *config) {
    if (config->layer_count) {
        printf("Generating new Neural Networks\n");
        printf("With layout:");
        for (int i = 0; i < config->layer_count; i++) {
            printf(" %d", config->layer_sizes[i]);
        }
        printf("\n");

        for (int variant = 0; variant < config->top_variant_count; variant++) {
            a->top[variant].layers = config->layer_count;
            for (int layer = 0; layer < config->layer_count; layer++) {
                a->top[variant].hidden[layer].size = config->layer_sizes[layer];
            }
            variant_rand(&a->top[variant]);
        }
    } else {
        FILE *file = fopen(config->filename, "rb");
        if (file == NULL) {
            printf("File %s cannot be opened. Please use --new to generate "
                   "a new one.\n",
                   config->filename);
            return false;
        }

        // Calculate how many NeuralNetwork structs are in the file
        fseek(file, 0, SEEK_END);
        size_t file_size = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);
        size_t network_count = file_size / sizeof(struct NeuralNetwork);

        // Read the data into the array
        fread(a->top, sizeof(struct NeuralNetwork), network_count, file);
        fclose(file);

        printf("Loaded %lu networks from the file %s.\n", network_count,
               config->filename);
        printf("With layout:");
        for (int layer = 0; layer < a->top->layers; layer++) {
            printf(" %d", a->top->hidden[layer].size);
        }
        printf("\n");
    }

    return true;
}

void networks_populate(struct AI *a, const struct AIConfig *config) {
    int top_per_thread = config->top_variant_count / config->thread_count;
    int var_per_top_per_thread = config->game_variant_count / top_per_thread;

    int top_var = 0;
    // for each game.
    for (int game = 0; game < config->thread_count; game++) {
        // for top_vars / game.
        int game_var = 0;
        for (int i = 0; i < top_per_thread; i++) {
            a->games[game].networks[game_var] = a->top[top_var];
            a->games[game].networks[game_var].fitness = 0;
            game_var++;

            for (int j = 1; j < var_per_top_per_thread; j++) {
                a->games[game].networks[game_var] = a->top[top_var];
                a->games[game].networks[game_var].fitness = 0;
                variant_mutate(&a->games[game].networks[game_var],
                               config->mut_rate, config->mut_rang);
                game_var++;
            }

            top_var++;
        }
    }
}

void networks_save(struct AI *a, const struct AIConfig *config) {
    FILE *file = fopen(config->filename, "wb");
    if (file != NULL) {
        fwrite(a->top, sizeof(struct NeuralNetwork),
               (size_t)config->top_variant_count, file);
        fclose(file);
        printf("\n");
        printf("Saved %d to file %s.\n", config->top_variant_count,
               config->filename);
        printf("With layout:");
        for (int layer = 0; layer < a->top->layers; layer++) {
            printf(" %d", a->top->hidden[layer].size);
        }
        printf("\n");
    } else {
        fprintf(stderr, "Error opening %s for writing\n", config->filename);
    }
}

void networks_get_top(struct AI *a, const struct AIConfig *config) {
    memset(a->top, -10000,
           (size_t)config->top_variant_count * sizeof(struct NeuralNetwork));
    for (int game = 0; game < config->thread_count; game++) {
        for (int game_var = 0; game_var < config->game_variant_count;
             game_var++) {
            int fitness = a->games[game].networks[game_var].fitness;
            for (int top_var = 0; top_var < config->top_variant_count;
                 top_var++) {
                if (fitness > a->top[top_var].fitness) {
                    for (int i = config->top_variant_count - 1; i > top_var;
                         i--) {
                        a->top[i] = a->top[i - 1];
                    }
                    a->top[top_var] = a->games[game].networks[game_var];
                    break;
                }
            }
        }
    }
}

bool ai_run(struct AI *a, const struct AIConfig *config) {
    if (config->ai_train) {
        pthread_t threads[config->thread_count];

        for (int gen = 0; gen < config->generation_count; gen++) {
            networks_populate(a, config);
            if (!config->gfx_off) {
                game_run(&a->games[0]);
            } else {
                for (int thread = 0; thread < config->thread_count; thread++) {
                    pthread_create(&threads[thread], NULL, game_run_multi,
                                   &a->games[thread]);
                }

                for (int thread = 0; thread < config->thread_count; thread++) {
                    pthread_join(threads[thread], NULL);
                }
            }

            if (config->ai_train) {
                networks_get_top(a, config);

                for (int i = 0; i < config->top_variant_count; i++) {
                    printf("fitness: %d, generation: %d\n", a->top[i].fitness,
                           a->top[i].generation);
                }

                printf("generation: %d\n", gen + 1);
            }
        }

        networks_save(a, config);
    } else if (config->ai_on) {
        networks_populate(a, config);
        game_run(&a->games[0]);
    } else {
        game_run(&a->games[0]);
    }

    return true;
}
