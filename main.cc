#define DEBUG 1

#include "Goban.h"
#if USE_THREADS
#  include "ThreadPool.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;


Goban check_stone_removal(const Goban &goban, const Goban &est, const Goban &removal);
int scan1(FILE *fp, const char *fmt);

struct Result {
    const char *filename;
    int errct;
    int player_to_move;
    Goban goban;
    Goban removal;
    Goban est;
    Goban errors;
    long elapsed;
};

int main(int argn, const char *args[]) {
    srand(time(NULL));
    int trials = 10000;
    float tolerance = 0.35f;

    if (argn < 2) {
        fprintf(stderr, "Usage: estimator <file.game> ...\n");
        return -1;
    }

    int num_errors = 0;
    int num_passes = 0;

#if USE_THREADS
    int num_threads = std::thread::hardware_concurrency();
#endif


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
            } else {
                trials = atol(args[arg]);
                if (trials < 1) {
                    fprintf(stderr, "Invalid value for trials\n");
                    return -2;
                }
            }

            continue;
        }
    }


    printf("###\n");
#if USE_THREADS
    printf("### Using %d threads\n", num_threads);
#endif
    printf("### Trials per test: %d\n", trials);
    printf("### Scoring tolerance: %.4f\n", tolerance);
    printf("###\n");
    printf("\n");

#if USE_THREADS
    ThreadPool tp(num_threads);
    vector<future<Result>> results;
#else
    vector<Result> results;
#endif

    for (int arg=1; arg < argn; ++arg) {
        if (!strstr(args[arg], "game")) {
            continue;
        }

#if USE_THREADS
        results.push_back(tp.enqueue([args, arg, trials, tolerance]() -> Result {
#endif
            Result result;

            result.filename = args[arg];

            FILE *fp = fopen(args[arg], "r");
            if (!fp) {
                fprintf(stderr, "Failed to open file %s\n", result.filename);
                throw runtime_error("Failed to open input file");
            }

            char buf[256];
            if (!fgets(buf, sizeof(buf), fp) || buf[0] != '#') {
                fprintf(stderr, "Invalid game file\n");
                throw runtime_error("Invalid game file");
            }

            result.removal.height = result.goban.height = scan1(fp, "height %d\n");
            result.removal.width  = result.goban.width  = scan1(fp, "width %d\n");
            result.player_to_move = scan1(fp, "player_to_move %d\n");

            if (result.player_to_move != 1 && result.player_to_move != -1) {
                throw runtime_error("Invalid player to move");
            }

            for (int y=0; y < result.goban.height; ++y) {
                for (int x=0; x < result.goban.width; ++x) {
                    result.goban.board[y][x] = scan1(fp, "%d");
                }
            }

            for (int y=0; y < result.removal.height; ++y) {
                for (int x=0; x < result.removal.width; ++x) {
                    result.removal.board[y][x] = scan1(fp, "%d");
                }
            }

            fclose(fp);

            auto start = chrono::high_resolution_clock::now();
            //result.est = result.goban.estimate(WHITE, trials, tolerance);
            result.est = result.goban.estimate(result.player_to_move == 1 ? BLACK : WHITE, trials, tolerance);
            auto stop = chrono::high_resolution_clock::now();
            result.elapsed = chrono::duration_cast<chrono::milliseconds>(stop - start).count();

            result.errors = check_stone_removal(result.goban, result.est, result.removal);
            result.errct = 0;
            for (int y=0; y < result.removal.height; ++y) {
                for (int x=0; x < result.removal.width; ++x) {
                    result.errct += result.errors.board[y][x];
                }
            }

#if USE_THREADS
            return result;
        }));

    }

    for (auto &res_future : results) {
        res_future.wait();
        Result result = res_future.get();
#endif

        if (result.errct) {
            printf("### ERROR %s had %d incorrectly scored points [%ld ms]\n", result.filename, result.errct, result.elapsed);
            printf(" height: %d\n", result.goban.height);
            printf(" width: %d\n", result.goban.width);
            printf(" player to move: %d\n", result.player_to_move);
            printf("\n");
            result.goban.board.print('X', 'o', '.');
            printf("\n");
            result.removal.board.print('r', 'o', '.');
            printf("\n");
            result.est.board.print('#', '_', '.');
            printf("\n");
            result.errors.board.print('E', 'E', '.');
            ++num_errors;
        } else {
            printf("### PASS %s [%ld ms]\n", result.filename, result.elapsed);
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
