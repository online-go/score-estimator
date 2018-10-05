#pragma once

#include "Color.h"
#include "Point.h"
#include "Vec.h"
#include "Grid.h"

class Goban {
    public:
        enum Result {
            OK = 0,
            ILLEGAL = 1,
        };

    public:
        int       width;
        int       height;
        Grid<int> board;
        int       do_ko_check;
        Point     possible_ko;

        Goban();
        Goban(const Goban &other);
        Goban estimate(Color player_to_move, int trials, float tolerance);
        Point generateMove(Color player, int trials, float tolerance);
        inline int operator[](const Point &p) const { return board[p]; }
        inline int& operator[](const Point &p) { return board[p]; }
        int score();
        void setSize(int width, int height);
        void clearBoard();
        void play_out_position(Color player_to_move);
        Result place_and_remove(Point move, Color player, Vec &possible_moves);

    private:
        void init();
        void get_neighbors(const Point &pt, NeighborVec &output);
        bool has_liberties(const Point &pt);
        int  remove_group(Point move, Vec &possible_moves);
        bool is_eye(Point move, Color player);
        bool is_territory(Point pt, Color player);
        void fill_territory(Point pt, Color player);
        void synchronize_tracking_counters(Grid<int> &track, Goban &visited, Point &p);


#ifdef DEBUG
    public:
        void showBoard(unsigned char black='X', unsigned char white='o', unsigned char blank='.');
        Point pointFromStr(const char *str);
#endif
};
