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
            variant_rand(&a->top[variant], config->int_dev);
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
    for (int variant = 0; variant < config->variant_count; variant++) {
        int game = variant / config->game_variant_count;
        int game_var = variant % config->game_variant_count;
        int top_var = variant % config->top_variant_count;
        a->games[game].networks[game_var] = a->top[top_var];
        a->games[game].networks[game_var].fitness = 0;
        a->games[game].game_variant_count = game_var;
        if (variant < config->top_variant_count) {
            a->games[game].networks[game_var].mutate = false;
        } else {
            a->games[game].networks[game_var].mutate = true;
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
    struct NeuralNetwork *top_vars[config->top_variant_count];

    for (int i = 0; i < config->top_variant_count; i++) {
        top_vars[i] = NULL;
    }

    for (int variant = 0; variant < config->variant_count; variant++) {
        int game = variant / config->game_variant_count;
        int game_var = variant % config->game_variant_count;
        double fitness = a->games[game].networks[game_var].fitness;

        for (int top_var = 0; top_var < config->top_variant_count; top_var++) {
            if (top_vars[top_var]) {
                if (fitness > top_vars[top_var]->fitness) {
                    for (int i = config->top_variant_count - 1; i > top_var;
                         i--) {
                        if (top_vars[i - 1]) {
                            top_vars[i] = top_vars[i - 1];
                        }
                    }
                    top_vars[top_var] = &a->games[game].networks[game_var];
                    break;
                }
            } else {
                top_vars[top_var] = &a->games[game].networks[game_var];
            }
        }
    }

    for (int i = 0; i < config->top_variant_count; i++) {
        if (top_vars[i]) {
            a->top[i] = *top_vars[i];
        }
    }
}

bool ai_run(struct AI *a, const struct AIConfig *config) {
    if (config->ai_train) {
        pthread_t threads[config->thread_count];

        for (int gen = 0; gen < config->generation_count; gen++) {
            networks_populate(a, config);
            if (config->gfx_off || config->thread_count > 1) {
                for (int thread = 0; thread < config->thread_count; thread++) {
                    pthread_create(&threads[thread], NULL, game_run_multi,
                                   &a->games[thread]);
                }

                for (int thread = 0; thread < config->thread_count; thread++) {
                    pthread_join(threads[thread], NULL);
                }
            } else {
                game_run(&a->games[0]);
            }

            networks_get_top(a, config);

            int print_top =
                20 < config->top_variant_count ? 20 : config->top_variant_count;
            for (int i = 0; i < print_top; i++) {
                printf("fitness: %f, generation: %d\n", a->top[i].fitness,
                       a->top[i].generation);
            }

            printf("generation: %d\n", gen + 1);
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
