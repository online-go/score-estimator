#pragma once

#include "constants.h"
#include "Point.h"
#include <string.h>
#include <stdio.h>


/* Simple 2d array used for various purposes while tracking game and estimation state */

template<typename T=int, int MAX_W=MAX_WIDTH, int MAX_H=MAX_HEIGHT>
class Grid {
    public:
        Grid(int width, int height) : width(width), height(height) {
            clear();
        }

        inline T* operator[](const int &y) { return _data[y]; }
        inline const T* operator[](const int &y) const { return _data[y]; }
        inline T operator[](const Point &p) const { return _data[p.y][p.x]; }
        inline T& operator[](const Point &p) { return _data[p.y][p.x]; }

        void clear(const T &value=0) {
            for (int y=0; y < MAX_H; ++y) {
                for (int x=0; x < MAX_W; ++x) {
                    _data[y][x] = value;
                }
            }
        }
    private:
        int width;
        int height;
        T   _data[MAX_H][MAX_W];

#ifdef DEBUG
    public:
        void print(unsigned char black='X', unsigned char white='o', unsigned char blank='.') {
            printf("    ");
            for (int x=0; x < width; ++x) {
                printf("%c ", board_letters[x]);
            }
            printf("  \n");

            printf("  ");
            for (int x=0; x <= width; ++x) {
                printf("%c", x == 0 ? '+' : '-');
                printf("%c", '-');
            }
            printf("+  \n");

            for (int y=0; y < height; ++y) {
                printf("%2d|", 19-y);
                for (int x=0; x < width; ++x) {
                    printf(" %c", _data[y][x] == 0 ? blank : (_data[y][x] == 1 ? black: white));
                }
                printf(" |%-2d\n", 19-y);
            }

            printf("  ");
            for (int x=0; x <= width; ++x) {
                printf("%c", x == 0 ? '+' : '-');
                printf("%c", '-');
            }
            printf("+  \n");

            printf("    ");
            for (int x=0; x < width; ++x) {
                printf("%c ", board_letters[x]);
            }
            printf("  \n");
        }
#endif
};

