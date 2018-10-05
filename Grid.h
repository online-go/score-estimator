#pragma once

#include "constants.h"
#include "Point.h"
#include <string.h>


/* Simple 2d array used for various purposes while tracking game and estimation state */

template<typename T=int, int MAX_W=MAX_WIDTH, int MAX_H=MAX_HEIGHT>
class Grid {
    public:
        Grid() {
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
        T   _data[MAX_H][MAX_W];
};
