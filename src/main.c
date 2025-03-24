#include "main.h"
#include "ai.h"

bool is_number(const char *str);
bool is_file_name(const char *str);
bool get_layers(struct AIConfig *config, const char *str);

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

int main(int argc, char *argv[]) {
    int exit_status = EXIT_FAILURE;

    struct AIConfig ai_config = {0};
    ai_config.filename = FILE_NAME;
    ai_config.white_inc = WHITE_INC;
    ai_config.yellow_inc = YELLOW_INC;
    ai_config.mut_rate = MUT_RATE;
    ai_config.mut_rang = MUT_RANG;
    ai_config.max_ticks = TICKS;
    ai_config.thread_count = THREADS;
    ai_config.trial_count = TRIALS;
    ai_config.generation_count = GENS;
    ai_config.variant_count = VARIANTS;
    ai_config.top_variant_count = TOP_VARIANTS;

    int argument = 1;
    while (argument < argc) {
        if (strcmp(argv[argument], "--ai_on") == 0) {
            ai_config.ai_on = true;
            ai_config.generation_count = 1;
        } else if (strcmp(argv[argument], "--ai_train") == 0) {
            ai_config.ai_on = true;
            ai_config.ai_train = true;
        } else if (strcmp(argv[argument], "--gfx_off") == 0) {
            ai_config.gfx_off = true;
            ai_config.ai_on = true;
            ai_config.ai_train = true;
        } else if (strcmp(argv[argument], "--fit_ticks") == 0) {
            ai_config.fit_ticks = true;
            ai_config.ai_on = true;
            ai_config.ai_train = true;
        } else if (strcmp(argv[argument], "--filename") == 0) {
            argument++;
            if (argument < argc) {
                if (!is_file_name(argv[argument])) {
                    fprintf(stderr, "--filename %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.filename = argv[argument];
                ai_config.ai_on = true;
            }
        } else if (strcmp(argv[argument], "--white_inc") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument])) {
                    fprintf(stderr, "--white_inc %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.white_inc = num;
            }
        } else if (strcmp(argv[argument], "--yellow_inc") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument])) {
                    fprintf(stderr, "--yellow_inc %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.yellow_inc = num;
            }

        } else if (strcmp(argv[argument], "--mut_rate") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RATE) {
                    fprintf(stderr, "--mut_rate %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.mut_rate = num / 100.0;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--mut_rang") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_RANG) {
                    fprintf(stderr, "--mut_rang %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.mut_rang = num / 100.0;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--trials") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_TRIALS) {
                    fprintf(stderr, "--trials %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.trial_count = num;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--max_ticks") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_TICKS) {
                    fprintf(stderr, "--max_ticks %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.max_ticks = num;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--threads") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 ||
                    num > MAX_THREADS) {
                    fprintf(stderr, "--threads %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.thread_count = num;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
                ai_config.gfx_off = true;
            }
        } else if (strcmp(argv[argument], "--top_variants") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) ||
                    num < ai_config.variant_count || num > MAX_TOP_VARIANTS) {
                    fprintf(stderr, "--top_variants %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.top_variant_count = num;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--variants") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) ||
                    num < ai_config.thread_count || num > MAX_VARIANTS) {
                    fprintf(stderr, "--variants %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.variant_count = num;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--gens") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (!is_number(argv[argument]) || num < 1 || num > MAX_GENS) {
                    fprintf(stderr, "--gens %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }
                ai_config.generation_count = num;
                ai_config.ai_on = true;
                ai_config.ai_train = true;
            }
        } else if (strcmp(argv[argument], "--new") == 0) {
            while (argument + 1 < argc && ai_config.layer_count < LAYERS &&
                   is_number(argv[argument + 1])) {
                argument++;
                int num = atoi(argv[argument]);
                if (num < OUTPUTS || num > LAYER_SIZE) {
                    fprintf(stderr, "layer size of %s is not valid.\n",
                            argv[argument]);
                    return exit_status;
                }

                ai_config.layer_sizes[ai_config.layer_count] = num;
                ai_config.layer_count++;
            }

            if (ai_config.layer_count == 0) {
                fprintf(stderr, "no suitable layers were set.\n");
                return exit_status;
            }
            ai_config.ai_on = true;
            ai_config.ai_train = true;
        }
        argument++;
    }

    if (ai_config.thread_count > ai_config.variant_count) {
        fprintf(stderr,
                "Error: Threads %d cannot be larger than variants %d.\n",
                ai_config.thread_count, ai_config.variant_count);
        return exit_status;
    }

    if (ai_config.top_variant_count > ai_config.variant_count) {
        fprintf(
            stderr,
            "Error: Top Variants %d cannot be larger than top_variants %d.\n",
            ai_config.top_variant_count, ai_config.variant_count);
        return exit_status;
    }

    ai_config.game_variant_count =
        ai_config.variant_count / ai_config.thread_count;

    if (ai_config.layer_count) {
        printf("Generating new network with layers:");
        for (int i = 0; i < ai_config.layer_count; i++) {
            printf(" %i", ai_config.layer_sizes[i]);
        }
        printf("\n");
    }

    if (ai_config.ai_on) {
        printf("white_inc %d, yellow_inc %d\n", ai_config.white_inc,
               ai_config.yellow_inc);
        printf("filename: %s\n", ai_config.filename);
        printf("ai_on: %s\n", (ai_config.ai_on) ? "true" : "false");
        printf("ai_train: %s\n", (ai_config.ai_train) ? "true" : "false");
        printf("gfx_off: %s\n", (ai_config.gfx_off) ? "true" : "false");
        if (ai_config.ai_train) {
            printf("mut_rate: %g\n", ai_config.mut_rate);
            printf("mut_rang: %g\n", ai_config.mut_rang);
            printf("max_ticks: %d\n", ai_config.max_ticks);
            printf("threads: %d\n", ai_config.thread_count);
            printf("trials: %d\n", ai_config.trial_count);
            printf("variants: %d\n", ai_config.variant_count);
            printf("top_variants: %d\n", ai_config.top_variant_count);
            printf("gens: %d\n", ai_config.generation_count);
        }
        printf("\n");
    }

    // return EXIT_SUCCESS;

    struct AI *ai = NULL;

    if (ai_new(&ai, &ai_config)) {
        if (ai_run(ai, &ai_config)) {
            exit_status = EXIT_SUCCESS;
        }
    }

    ai_free(&ai, &ai_config);

    return exit_status;
}
