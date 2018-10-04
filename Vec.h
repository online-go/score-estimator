#pragma once

#include "Point.h"
#include "constants.h"

template<int SIZE>
class Vec_t {
    public:
        Point points[SIZE];
        int size;

    public:
        Vec_t() {
            size = 0;
        }
        Point operator[](const int &i) const { 
            return points[i]; 
        }
        Point& operator[](const int &i) { 
            return points[i]; 
        }
        void push(const Point &p) {
            points[size++] = p;
        }
        Point remove(int idx) {
            Point ret = points[idx];
            points[idx] = points[--size];
            return ret;
        }
};


typedef Vec_t<MAX_SIZE> Vec;
typedef Vec_t<4>        NeighborVec;
