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

        Grid<int> global_visited;
        int       last_visited_counter;

        Goban();
        void setBoardSize(int width, int height); 
        Goban estimate(Color player_to_move, int trials, float tolerance);
        Point generateMove(Color player, int trials, float tolerance);
        inline int operator[](const Point &p) const { return board[p]; }
        inline int& operator[](const Point &p) { return board[p]; }
        int score();
        void setSize(int width, int height);
        void clearBoard();
        void play_out_position(Color player_to_move, const Grid<int> &life_map);
        Result place_and_remove(Point move, Color player, Vec &possible_moves);

        /** Marks each location with the positive or negative size of the territory
         * (negative for white, postive for black), or zero if the location is not
         * territory. */
        Grid<int> computeTerritory();

        /** Uniquely labels strings of groups on the board. */
        Grid<int> computeGroupMap();

        /**
         * Computes the liberties for any groups on the boards. For empty
         * spaces, computes the number of blank minus number of white stones
         * touching the group.
         */
        Grid<int> computeLiberties(const Grid<int> &group_map);

        /** 
         * Flags spaces that are part of a string of like colored stone strings and territory 
         * so long as the stone strings have a combined two or more territory 
         */ 
        Grid<int> computeStrongLife(const Grid<int> &groups, const Grid<int> &territory, const Grid<int> &liberties);

    private:
        bool has_liberties(const Point &pt);
        int  remove_group(Point move, Vec &possible_moves);
        bool is_eye(Point move, Color player);
        bool is_territory(Point pt, Color player);
        void fill_territory(Point pt, Color player);
        void synchronize_tracking_counters(Grid<int> &track, Goban &visited, Point &p);
};
