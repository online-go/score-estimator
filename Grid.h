#pragma once

#include "constants.h"
#include "Point.h"
#include "Vec.h"
#include <string.h>
#include <stdio.h>


/* Simple 2d array used for various purposes while tracking game and estimation state */

template<typename T=int, int MAX_W=MAX_WIDTH, int MAX_H=MAX_HEIGHT>
class Grid {
    public:
        int width;
        int height;

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


        /* Flood matches all similar values starting at the starting_point, writes value to
         * the corresponding coordinates int he destination grid. */
        void traceGroup(const Point &starting_point, Grid &destination, const T &value) {
            Vec         tocheck;
            NeighborVec neighbors;
            Grid<int>   visited(width, height);
            T           matching_value = (*this)[starting_point];

            tocheck.push(starting_point);
            visited[starting_point] = 1;

            while (tocheck.size) {
                Point p = tocheck.remove(0);
                if ((*this)[p] == matching_value) {
                    destination[p] = value;
                    getNeighbors(p, neighbors);
                    for (int i=0; i < neighbors.size; ++i) {
                        Point neighbor = neighbors[i];
                        if (visited[neighbor]) {
                            continue;
                        }
                        visited[neighbor] = 1;
                        tocheck.push(neighbor);
                    }
                }
            }
        }

        /* Flood matches all similar values starting at starting_point, writing all points
         * within the flood match to group, and all neighboring points to neighbors */
        void groupAndNeighbors(const Point &starting_point, Vec &group, Vec &out_neighbors) {
            Vec         tocheck;
            NeighborVec neighbors;
            Grid<int>   visited(width, height);
            T           matching_value = (*this)[starting_point];

            tocheck.push(starting_point);
            visited[starting_point] = 1;

            while (tocheck.size) {
                Point p = tocheck.remove(0);
                if ((*this)[p] == matching_value) {
                    group.push(p);
                    getNeighbors(p, neighbors);
                    for (int i=0; i < neighbors.size; ++i) {
                        Point neighbor = neighbors[i];
                        if (visited[neighbor]) {
                            continue;
                        }
                        visited[neighbor] = 1;
                        tocheck.push(neighbor);
                    }
                } else {
                    out_neighbors.push(p);
                }
            }
        }

        /* Sets all points in group to the provided value */
        void set(const Vec &group, const T &value) {
            for (int i=0; i< group.size; ++i) {
                (*this)[group[i]] = value;
            }
        }

        /* Writes all valid neighboring points into output */
        void getNeighbors(const Point &pt, NeighborVec &output) {
            output.size = 0;
            if (pt.x > 0)        output.push(Point(pt.x-1, pt.y));
            if (pt.x+1 < width)  output.push(Point(pt.x+1, pt.y));
            if (pt.y > 0)        output.push(Point(pt.x, pt.y-1));
            if (pt.y+1 < height) output.push(Point(pt.x, pt.y+1));
        }


    private:
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
                printf("%2d|", height-y);
                for (int x=0; x < width; ++x) {
                    printf(" %c", _data[y][x] == 0 ? blank : (_data[y][x] == 1 ? black: white));
                }
                printf(" |%-2d\n", height-y);
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


        void printInts(unsigned char black='X', unsigned char white='o', unsigned char blank='.') {
            printf("    ");
            for (int x=0; x < width; ++x) {
                printf("  %c", board_letters[x]);
            }
            printf("  \n");

            printf("  ");
            for (int x=0; x <= width; ++x) {
                printf("%c", x == 0 ? '+' : '-');
                printf("%c", '-');
                printf("%c", '-');
            }
            printf("+  \n");

            for (int y=0; y < height; ++y) {
                printf("%2d| ", height-y);
                for (int x=0; x < width; ++x) {
                    printf(" %2d", (int)_data[y][x]);
                }
                printf(" |%-2d\n", height-y);
            }

            printf("  ");
            for (int x=0; x <= width; ++x) {
                printf("%c", x == 0 ? '+' : '-');
                printf("%c", '-');
                printf("%c", '-');
            }
            printf("+  \n");

            printf("    ");
            for (int x=0; x < width; ++x) {
                printf("  %c", board_letters[x]);
            }
            printf("  \n");
        }

#endif
};

