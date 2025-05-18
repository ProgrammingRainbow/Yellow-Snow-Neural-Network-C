#include "main.h"
#include "ai.h"

bool is_number(const char *str);
bool is_file_name(const char *str);
bool parse_arguments(int argc, char *argv[], struct AIConfig *config);

bool is_number(const char *str) {
    if (*str == '+' || *str == '-') {
        str++;
    }
    do {
        if (*str < '0' || *str > '9') {
            return false;
        }
        str++;
    } while (*str != '\0');

    return true;
}

bool is_file_name(const char *str) {
    while (*str != '\0') {
        if (*str < 'a' || *str > 'z') {
            if (*str < 'A' || *str > 'Z') {
                if (*str < '0' || *str > '9') {
                    if (*str != '-' && *str != '_') {
                        return false;
                    }
                }
            }
        }
        str++;
    }

    return true;
}

bool parse_arguments(int argc, char *argv[], struct AIConfig *config) {
    config->filename = FILE_NAME;
    config->white_inc = WHITE_INC;
    config->yellow_inc = YELLOW_INC;
    config->mut_rate = MUT_RATE / 100.0;
    config->int_dev = INT_DEV / 100.0;
    config->gau_dev = GAU_DEV / 100.0;
    config->max_ticks = TICKS;
    config->thread_count = THREADS;
    config->trial_count = TRIALS;
    config->generation_count = GENS;
    config->variant_count = VARIANTS;
    config->top_variant_count = TOP_VARIANTS;

    int argument = 1;
    while (argument < argc) {
        if (strcmp(argv[argument], "--ai_on") == 0) {
            config->ai_on = true;
            config->generation_count = 1;
        } else if (strcmp(argv[argument], "--ai_train") == 0) {
            config->ai_on = true;
            config->ai_train = true;
        } else if (strcmp(argv[argument], "--gfx_off") == 0) {
            config->gfx_off = true;
            config->ai_on = true;
            config->ai_train = true;
        } else if (strcmp(argv[argument], "--fit_ticks") == 0) {
            config->fit_style = FIT_TICKS;
            config->ai_on = true;
            config->ai_train = true;
        } else if (strcmp(argv[argument], "--fit_timed") == 0) {
            config->fit_style = FIT_TIMED;
            config->ai_on = true;
            config->ai_train = true;
        } else if (strcmp(argv[argument], "--filename") == 0) {
            argument++;
            if (argument < argc) {
                if (!is_file_name(argv[argument])) {
                    fprintf(stderr, "--filename %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->filename = argv[argument];
                config->ai_on = true;
            }
        } else if (strcmp(argv[argument], "--white_inc") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument])) {
                    fprintf(stderr, "--white_inc %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->white_inc = num;
            }
        } else if (strcmp(argv[argument], "--yellow_inc") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument])) {
                    fprintf(stderr, "--yellow_inc %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->yellow_inc = num;
            }

        } else if (strcmp(argv[argument], "--mut_rate") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RATE) {
                    fprintf(stderr, "--mut_rate %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->mut_rate = num / 100.0;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--int_dev") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_DEV) {
                    fprintf(stderr, "--int_dev %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->int_dev = num / 100.0;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--gau_dev") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_DEV) {
                    fprintf(stderr, "--gau_dev %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->gau_dev = num / 100.0;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--trials") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_TRIALS) {
                    fprintf(stderr, "--trials %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->trial_count = num;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--max_ticks") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_TICKS) {
                    fprintf(stderr, "--max_ticks %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->max_ticks = num;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--threads") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 ||
                    num > MAX_THREADS) {
                    fprintf(stderr, "--threads %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->thread_count = num;
                config->ai_on = true;
                config->ai_train = true;
                config->gfx_off = true;
            }
        } else if (strcmp(argv[argument], "--top_variants") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num > config->variant_count ||
                    num > MAX_TOP_VARIANTS) {
                    fprintf(stderr, "--top_variants %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->top_variant_count = num;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--variants") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < config->thread_count ||
                    num > MAX_VARIANTS) {
                    fprintf(stderr, "--variants %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->variant_count = num;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--gens") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_GENS) {
                    fprintf(stderr, "--gens %s is not valid.\n",
                            argv[argument]);
                    return false;
                }
                config->generation_count = num;
                config->ai_on = true;
                config->ai_train = true;
            }
        } else if (strcmp(argv[argument], "--new") == 0) {
            while (argument + 1 < argc && config->layer_count < LAYERS &&
                   is_number(argv[argument + 1])) {
                argument++;
                int num = atoi(argv[argument]);
                if (num < OUTPUTS || num > LAYER_SIZE) {
                    fprintf(stderr, "layer size of %s is not valid.\n",
                            argv[argument]);
                    return false;
                }

                config->layer_sizes[config->layer_count] = num;
                config->layer_count++;
            }

            if (config->layer_count == 0) {
                fprintf(stderr, "no suitable layers were set.\n");
                return false;
            }
            config->ai_on = true;
            config->ai_train = true;
        }
        argument++;
    }

    if (config->thread_count > config->variant_count) {
        fprintf(stderr,
                "Error: Threads %d cannot be larger than variants %d.\n",
                config->thread_count, config->variant_count);
        return false;
    }

    if (config->top_variant_count > config->variant_count) {
        fprintf(
            stderr,
            "Error: Top Variants %d cannot be larger than top_variants %d.\n",
            config->top_variant_count, config->variant_count);
        return false;
    }

    config->game_variant_count =
        (int)ceil(config->variant_count / (double)config->thread_count);

    if (config->layer_count) {
        printf("Generating new network with layers:");
        for (int i = 0; i < config->layer_count; i++) {
            printf(" %i", config->layer_sizes[i]);
        }
        printf("\n");
    }

    if (config->ai_on) {
        printf("filename: %s\n", config->filename);
        printf("ai_on: %s\n", (config->ai_on) ? "true" : "false");
        if (config->ai_train) {
            printf("ai_train: %s\n", (config->ai_train) ? "true" : "false");
            printf("gfx_off: %s\n", (config->gfx_off) ? "true" : "false");
            switch (config->fit_style) {
            case FIT_TIMED:
                printf("fit_style: fit_timed\n");
                printf("max_ticks: %d\n", config->max_ticks);
                printf("white_inc %d, yellow_inc %d\n", config->white_inc,
                       config->yellow_inc);
                break;
            case FIT_SCORE:
                printf("fit_style: fit_score\n");
                break;
            case FIT_TICKS:
                printf("fit_style: fit_ticks\n");
                break;
            default:
                break;
            }
            printf("mut_rate: %g\n", config->mut_rate);
            printf("int_dev: %g\n", config->int_dev);
            printf("gau_dev: %g\n", config->gau_dev);
            printf("threads: %d\n", config->thread_count);
            printf("trials: %d\n", config->trial_count);
            printf("variants: %d\n", config->variant_count);
            printf("top_variants: %d\n", config->top_variant_count);
            printf("gens: %d\n", config->generation_count);
        }
        printf("\n");
    }

    return true;
}

int main(int argc, char *argv[]) {
    int exit_status = EXIT_FAILURE;

    struct AIConfig ai_config = {0};

    if (!parse_arguments(argc, argv, &ai_config)) {
        return exit_status;
    }

    struct AI *ai = NULL;

    if (ai_new(&ai, &ai_config)) {
        if (ai_run(ai, &ai_config)) {
            exit_status = EXIT_SUCCESS;
        }
    }

    ai_free(&ai, &ai_config);

    return exit_status;
}
