#include "main.h"
#include "ai.h"

int main(int argc, char *argv[]) {
    int exit_status = EXIT_SUCCESS;

    struct AIConfig ai_config = {
        .ai_on = false,
        .ai_train = false,
        .gfx_on = true,
        .thread_count = THREADS,
        .trial_count = TOTAL_TRIALS,
        .variant_count = TOTAL_VARIANTS,
        .generation_count = TOTAL_GENS,
        .top_variant_count = TOP_VARIANTS,
    };

    int argument = 1;
    while (argument < argc) {
        if (strcmp(argv[argument], "--ai_on") == 0) {
            ai_config.ai_on = true;
        } else if (strcmp(argv[argument], "--ai_train") == 0) {
            ai_config.ai_on = true;
            ai_config.ai_train = true;
        } else if (strcmp(argv[argument], "--gfx_off") == 0) {
            ai_config.gfx_on = false;
        } else if (strcmp(argv[argument], "--threads") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (num > 0 && num <= 20) {
                    ai_config.thread_count = num;
                    ai_config.ai_on = true;
                    ai_config.ai_train = true;
                    ai_config.gfx_on = false;
                }
            }
        } else if (strcmp(argv[argument], "--generations") == 0) {
            argument++;
            if (argument < argc) {
                int num = atoi(argv[argument]);
                if (num > 0) {
                    ai_config.generation_count = num;
                }
            }
        }
        argument++;
    }

    ai_config.game_variant_count =
        ai_config.variant_count / ai_config.thread_count;

    printf("ai_on: %d\n", ai_config.ai_on);
    printf("ai_train: %d\n", ai_config.ai_train);
    printf("gfx_on: %d\n", ai_config.gfx_on);
    printf("threads: %d\n", ai_config.thread_count);
    printf("trails: %d\n", ai_config.trial_count);
    printf("generations: %d\n", ai_config.generation_count);

    struct AI *ai = NULL;

    if (!ai_new(&ai, ai_config)) {
        exit_status = EXIT_FAILURE;
    } else {
        if (!ai_run(ai)) {
            exit_status = EXIT_FAILURE;
        }
    }

    ai_free(&ai);

    return exit_status;
}
