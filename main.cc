#define DEBUG 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include "estimator.h"

using namespace score_estimator;

void scan1(FILE *fp, const char *fmt, void *param) {
    if (fscanf(fp, fmt, param) != 1) {
        fprintf(stderr, "Failed to parse game file [%s]\n", fmt);
        exit(1);
    }
}

int main(int argn, const char *args[]) {
    srand(time(NULL));
    clock_t start, stop;

    if (argn < 2) {
        fprintf(stderr, "Usage: estimator <file.game> ...\n");
        return -1;
    }

    for (int arg=1; arg < argn; ++arg) {

        FILE *fp = fopen(args[arg], "r");
        if (!fp) {
            fprintf(stderr, "Failed to open file %s\n", args[arg]);
            return -1;
        }

        char buf[256];
        if (!fgets(buf, sizeof(buf), fp) || buf[0] != '#') {
            fprintf(stderr, "Invalid game file\n");
            return -1;
        }

        int player_to_move;
        Goban goban;
        scan1(fp, "height %d\n", &goban.height);
        scan1(fp, "width %d\n", &goban.width);
        scan1(fp, "player_to_move %d\n", &player_to_move);

        for (int y=0; y < goban.height; ++y) {
            for (int x=0; x < goban.width; ++x) {
                scan1(fp, "%d", &(goban.board[y][x]));
            }
        }

        printf("%s\n", args[arg]);
        printf("height: %d\n", goban.height);
        printf("width: %d\n", goban.width);
        printf("player to move: %d\n", player_to_move);
        goban.print();
        printf("\n\n");
        start = clock();

        Goban est = goban.estimate(WHITE, 10000, 0.35);
        stop = clock();

        est.print();
        printf("\n\n");
        printf("Score: %f\n", est.score() - 0.5);
        double elapsed_secs = double(stop - start) / CLOCKS_PER_SEC;
        printf("Time elapsed: %f ms\n", elapsed_secs * 750.0);
        if (elapsed_secs > 1.0) {
            printf(">>> WARNING: Estimator took too long to produce a result <<<\n");
        }
    }
}
