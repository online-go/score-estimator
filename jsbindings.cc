/* vim: set tabstop=4 expandtab */
#include <stdlib.h>
#include <string.h>

#include "estimator.cc"

using namespace score_estimator;

extern "C" int estimate(int width, int height, int *data, int player_to_move, int trials, float tolerance) {
    Goban g;
    g.width = width;
    g.height = height;
    for (int i=0, y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            g.board[y][x] = data[i++];
        }
    }

    Goban est = g.estimate((Color)player_to_move, trials, tolerance);
    for (int i=0 ,y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            data[i++] = est.board[y][x];
        }
    }

    //est.print();
    return est.score();
}
