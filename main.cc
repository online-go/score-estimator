#define DEBUG 1

#include "Goban.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <chrono>

using namespace std;

Goban check_stone_removal(const Goban &goban, const Goban &est, const Goban &removal);
int scan1(FILE *fp, const char *fmt);

int main(int argn, const char *args[]) {
    srand(time(NULL));
    int trials = 1000;
    float tolerance = 0.35f;

    if (argn < 2) {
        fprintf(stderr, "Usage: estimator <file.game> ...\n");
        return -1;
    }

    int num_errors = 0;
    int num_passes = 0;

    for (int arg=1; arg < argn; ++arg) {
        if (strstr(args[arg], "help")) {
            printf("Usage: estimate [# trials] [tolerance] file1.game file2.game ...\n");
            return 0;
        }
        if (!strstr(args[arg], "game")) {
            if (strstr(args[arg], ".")) {
                tolerance = atof(args[arg]);
                if (tolerance <= 0) {
                    fprintf(stderr, "Invalid value for tolerance\n");
                    return -2;
                }
                printf("Tolerance: %f\n", tolerance);
            } else {
                trials = atol(args[arg]);
                if (trials < 1) {
                    fprintf(stderr, "Invalid value for trials\n");
                    return -2;
                }
                printf("Trials: %d\n", trials);
            }

            continue;
        }


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
        Goban removal;
        removal.height = goban.height = scan1(fp, "height %d\n");
        removal.width  = goban.width  = scan1(fp, "width %d\n");
        player_to_move = scan1(fp, "player_to_move %d\n");

        for (int y=0; y < goban.height; ++y) {
            for (int x=0; x < goban.width; ++x) {
                goban.board[y][x] = scan1(fp, "%d");
            }
        }

        for (int y=0; y < removal.height; ++y) {
            for (int x=0; x < removal.width; ++x) {
                removal.board[y][x] = scan1(fp, "%d");
            }
        }


        auto start = chrono::high_resolution_clock::now();
        Goban est = goban.estimate(WHITE, trials, tolerance);
        auto stop = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(stop - start).count();

        Goban errors = check_stone_removal(goban, est, removal);
        int errct = 0;
        for (int y=0; y < removal.height; ++y) {
            for (int x=0; x < removal.width; ++x) {
                errct += errors.board[y][x];
            }
        }

        if (errct) {
            printf("### ERROR %s had %d incorrectly scored points [%ld ms]\n", args[arg], errct, elapsed);
            printf(" height: %d\n", goban.height);
            printf(" width: %d\n", goban.width);
            printf(" player to move: %d\n", player_to_move);
            printf("\n");
            goban.showBoard('X', 'o', '.');
            printf("\n");
            removal.showBoard('r', 'o', '.');
            printf("\n");
            est.showBoard('#', '_', '.');
            printf("\n");
            errors.showBoard('E', 'E', '.');
            ++num_errors;
        } else {
            printf("### PASS %s [%ld ms]\n", args[arg], elapsed);
            ++num_passes;
        }
    }

    printf("\n");
    printf("###\n");
    if (num_errors) {
        printf("### %d passes %d failures\n", num_passes, num_errors);
    } else {
        printf("### ALL %d TESTS PASSED!\n", num_passes);
    }
    printf("###\n");
    printf("\n");
    return num_errors;
}


Goban check_stone_removal(const Goban &goban, const Goban &est, const Goban &removal) {
    Goban errors;
    errors.width = goban.width;
    errors.height = goban.height;

    for (int y=0; y < removal.height; ++y) {
        for (int x=0; x < removal.width; ++x) {
            if (removal.board[y][x]) {
                if (goban.board[y][x] == 0) {
                    /* dame */
                    errors.board[y][x] = est.board[y][x] == 0 ? 0 : 1; /* error if it's still not not scored */
                } else {
                    /* removed */
                    errors.board[y][x] = goban.board[y][x] != - est.board[y][x];
                }
            } else {
                if (goban.board[y][x] == 0) {
                    /* not dame, will assume it's correctly filled in with appropriate territory then */
                } else {
                    /* non removed stone, make sure it didn't change */
                    errors.board[y][x] = goban.board[y][x] != est.board[y][x];
                }
            }
        }
    }

    return errors;
}


int scan1(FILE *fp, const char *fmt) {
    int input;
    if (fscanf(fp, fmt, &input) != 1) {
        fprintf(stderr, "Failed to parse game file [%s]\n", fmt);
        exit(1);
    }
    return input;
}
