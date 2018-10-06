/* vim: set tabstop=4 expandtab */
#include "Goban.h"
#ifndef EMSCRIPTEN
#  include <string.h>
#  include <stdlib.h>
#  include <stdio.h>
#endif
#ifdef DEBUG
#  include <stdio.h>
#  include <ctype.h>
#  include <stdlib.h>
#  include <iostream>

using namespace std; 
#endif


Goban::Goban() 
    : width(19)
    , height(19) 
    , board(width, height) 
    , global_visited(width, height) 
    , last_visited_counter(1)
{
}

void Goban::setBoardSize(int width, int height) {
    this->width = width;
    this->height = height;
    board.width = width;
    board.height = height;
    global_visited.width = width;
    global_visited.height = height;
}
Goban Goban::estimate(Color player_to_move, int trials, float tolerance) {
    Goban ret(*this);
    Grid<int> track(width, height);



    /* Bias our scoring towards trusting the player's area is theirs */
    {
        Goban t(*this);

        /* fill in territory */
        for (int y=0; y < height; ++y) {
            for (int x=0; x < width; ++x) {
                Point p(x,y);
                if (t[p] == 0) {
                    if (t.is_territory(p, BLACK)) {
                        t.fill_territory(p, BLACK);
                    }
                    if (t.is_territory(p, WHITE)) {
                        t.fill_territory(p, WHITE);
                    }
                }
            }
        }

        for (int y=0; y < height; ++y) {
            for (int x=0; x < width; ++x) {
                //if (board[y][x] == 0) {
                track[y][x] += t.board[y][x] * trials * (tolerance/2);
                //}
            }
        }
    }


    Grid<int> territory_map = computeTerritory();
    Grid<int> group_map = computeGroupMap();
    Grid<int> liberty_map = computeLiberties(group_map);
    Grid<int> strong_life = computeStrongLife(group_map, territory_map, liberty_map);

#ifndef EMSCRIPTEN
    printf("\nBias map:\n");
    track.printInts();

    printf("\nTerritory map:\n");
    territory_map.printInts();
    printf("\nGroup map:\n");
    group_map.printInts();
    printf("\nLiberty map:\n");
    liberty_map.printInts();
    printf("\nLife map:\n");
    strong_life.printInts();
#endif

    for (int i=0; i < trials; ++i) {
        /* Play out a random game */
        Goban t(*this);

        t.play_out_position(player_to_move, strong_life);

        /* fill in territory */
        for (int y=0; y < height; ++y) {
            for (int x=0; x < width; ++x) {
                Point p(x,y);
                if (t[p] == 0) {
                    if (t.is_territory(p, BLACK)) {
                        t.fill_territory(p, BLACK);
                    }
                    if (t.is_territory(p, WHITE)) {
                        t.fill_territory(p, WHITE);
                    }
                }
            }
        }

        /* track how many times each spot was white or black */
        for (int y=0; y < height; ++y) {
            for (int x=0; x < width; ++x) {
                track[y][x] += t.board[y][x];
            }
        }
    }

    printf("\nTrack map:\n");
    track.printInts();


    /* For each stone group, find the maximal track counter and set
     * all stones in that group to that level */
    Goban visited;
    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (!visited[p]) {
                synchronize_tracking_counters(track, visited, p);
            }
        }
    }


    /* Create a result board based off of how many times each spot
     * was which color. */
    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            /* If we're pretty confident we know who the spot belongs to, mark it */
            if (track[y][x] > trials*tolerance) {
                ret.board[y][x] = 1;
            } else if (track[y][x] < trials*-tolerance) {
                ret.board[y][x] = -1;
            /* if that fails, it's probably just dame */
            } else {
                ret.board[y][x] = 0;
            }
        }
    }


    /* TODO: Foreach hole, if it can only reach one color, color it that */
    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (ret[p] == 0) {
                if (ret.is_territory(p, BLACK)) {
                    ret.fill_territory(p, BLACK);
                }
                if (ret.is_territory(p, WHITE)) {
                    ret.fill_territory(p, WHITE);
                }
            }
        }
    }


    return ret;
}

Grid<int> Goban::computeGroupMap() {
    Grid<int> ret(width, height);
    int cur_group = 1;

    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (ret[p] == 0) {
                board.traceGroup(p, ret, cur_group++);
            }
        }
    }

    return ret;
}
Grid<int> Goban::computeTerritory() {
    Grid<int> ret(width, height);

    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (ret[p]) {
                continue;
            }

            Vec group, neighbors;
            if (board[p] == 0 && is_territory(p, BLACK)) {
                board.groupAndNeighbors(p, group, neighbors);
                ret.set(group, group.size * BLACK);
            }
            if (board[p] == 0 && is_territory(p, WHITE)) {
                board.groupAndNeighbors(p, group, neighbors);
                ret.set(group, group.size * WHITE);
            }
        }
    }

    return ret;
}
Grid<int> Goban::computeLiberties(const Grid<int> &group_map) {
    Grid<int> ret(width, height);
    Grid<int> visited(width, height);

    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (visited[p]) { 
                continue;
            }

            Vec group;
            Vec neighbors;
            board.groupAndNeighbors(p, group, neighbors);
            visited.set(group, 1);


            int liberty_count = 0;
            if ((*this)[p] == 0) {
                /* sum of all ajacent black - all ajacent white */
                for (int i=0; i < neighbors.size; ++i) {
                    liberty_count += (*this)[neighbors[i]];
                }
            } else {
                /* liberties of stone group */
                for (int i=0; i < neighbors.size; ++i) {
                    if ((*this)[neighbors[i]] == 0) {
                        ++liberty_count;
                    }
                }
            }

            ret.set(group, liberty_count);
        }
    }

    return ret;
}
Grid<int> Goban::computeStrongLife(const Grid<int> &groups, const Grid<int> &territory, const Grid<int> &liberties) {
    Grid<int> ret(width, height);
    Grid<int> visited(width, height);

    Grid<int> unified_territory_and_stones(width, height);
    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            unified_territory_and_stones[p] = board[p] == 0 ? (territory[p] <= -1 ? -1 : territory[p] >= 1 ? 1 : 0) : board[p];
        }
    }


    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (visited[p]) { 
                continue;
            }

            Vec group;
            Vec neighbors;
            unified_territory_and_stones.groupAndNeighbors(p, group, neighbors);

            int num_eyes = 0;
            int num_territory = 0;
            for (int i=0; i < group.size; ++i) {
                if (visited[group[i]]) { 
                    continue;
                }
                if (territory[group[i]]) {
                    Vec territory_group; 
                    Vec territory_neighbors;
                    board.groupAndNeighbors(group[i], territory_group, territory_neighbors);
                    visited.set(territory_group, 1);
                    num_eyes += 1;
                    num_territory += territory_group.size;
                }
            }

            visited.set(group, 1);
            if (num_eyes >= 2 || num_territory >= 5) {
                ret.set(group, num_territory);
            }
        }
    }

    return ret;
}

void Goban::synchronize_tracking_counters(Grid<int> &track, Goban &visited, Point &p) {
    Vec         tocheck;
    NeighborVec neighbors;
    int         my_color        = (*this)[p];
    int         max_track_value = track[p.y][p.x];
    Vec         to_synchronize;

    tocheck.push(p);
    visited[p] = true;

    if (my_color == 0) {
        return;
    }

    while (tocheck.size) {
        Point p = tocheck.remove(0);
        to_synchronize.push(p);
        max_track_value = max_track_value < 0 ?
                            MIN(track[p.y][p.x], max_track_value) :
                            MAX(track[p.y][p.x], max_track_value);
        board.getNeighbors(p, neighbors);
        for (int i=0; i < neighbors.size; ++i) {
            Point neighbor = neighbors[i];
            if ((*this)[neighbor] == my_color) {
                if (visited[neighbor]) continue;
                visited[neighbor] = true;
                tocheck.push(neighbor);
            }
        }
    }

    for (int i=0; i < to_synchronize.size; ++i) {
        Point p = to_synchronize[i];
        track[p.y][p.x] = max_track_value;
    }
}
bool Goban::is_territory(Point pt, Color player) {
    Vec         tocheck;
    NeighborVec neighbors;
    int         visited_counter = ++last_visited_counter;

    tocheck.push(pt);
    global_visited[pt] = visited_counter;

    while (tocheck.size) {
        Point p = tocheck.remove(0);
        if ((*this)[p] == 0) {
            board.getNeighbors(p, neighbors);
            for (int i=0; i < neighbors.size; ++i) {
                Point neighbor = neighbors[i];
                if (global_visited[neighbor] == visited_counter) continue;
                global_visited[neighbor] = visited_counter;
                tocheck.push(neighbor);
            }
        } else {
            if ((*this)[p] != player) {
                return false;
            }
        }
    }

    return true;;
}
void Goban::fill_territory(Point pt, Color player) {
    Vec         tocheck;
    NeighborVec neighbors;
    int         visited_counter = ++last_visited_counter;

    tocheck.push(pt);
    global_visited[pt] = visited_counter;

    while (tocheck.size) {
        Point p = tocheck.remove(0);
        if ((*this)[p] == 0) {
            (*this)[p] = player;
            board.getNeighbors(p, neighbors);
            for (int i=0; i < neighbors.size; ++i) {
                Point neighbor = neighbors[i];
                if (global_visited[neighbor] == visited_counter) continue;
                global_visited[neighbor] = visited_counter;
                tocheck.push(neighbor);
            }
        }
    }
}
void Goban::play_out_position(Color player_to_move, const Grid<int> &life_map) {
    do_ko_check = 0;
    possible_ko = Point(-1,-1);

    Vec possible_moves;
    Vec illegal_moves;

    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            Point p(x,y);
            if (board[p] == 0 && life_map[p] == 0) {
                possible_moves.push(Point(x,y));
            }
        }
    }

    int sanity = 1000;
    while (possible_moves.size > 0 && --sanity > 0) {
        int move_idx = rand() % possible_moves.size;
        Point mv(possible_moves[move_idx]);

        if (is_eye(mv, player_to_move)) {
            illegal_moves.push(possible_moves.remove(move_idx));
            continue;
        }

        int result = place_and_remove(mv, player_to_move, possible_moves);
        if (result == OK) {
            possible_moves.remove(move_idx);
            player_to_move = (Color)-player_to_move;
            for (int i=0; i < illegal_moves.size; ++i) {
                possible_moves.push(illegal_moves[i]);
            }
            illegal_moves.size = 0;
            continue;
        }
        else if (result == ILLEGAL) {
            illegal_moves.push(possible_moves.remove(move_idx));

            continue;
        }
    }
}
Goban::Result Goban::place_and_remove(Point move, Color player, Vec &possible_moves) {
    if (do_ko_check) {
        if (move == possible_ko) {
            return ILLEGAL;
        }
    }

    bool        reset_ko_check = true;
    bool        removed        = false;
    NeighborVec neighbors;

    board.getNeighbors(move, neighbors);

    (*this)[move] = player;
    ++last_visited_counter;
    for (int i=0; i < neighbors.size; ++i) {
        if ((*this)[neighbors[i]] == -player) {
            if (
                /* it's common that a previous has_liberties covers what we're
                 * about to test, so don't double test */
                global_visited[neighbors[i]] != last_visited_counter
                && !has_liberties(neighbors[i])
            ) {
                if (remove_group(neighbors[i], possible_moves) == 1) {
                    reset_ko_check = false;
                    do_ko_check = 1;
                    possible_ko = neighbors[i];
                }
                removed = true;
            }
        }
    }
    if (!removed) {
        if (!has_liberties(move)) {
            (*this)[move] = 0;
            return ILLEGAL;
        }
    }

    if (reset_ko_check) {
        do_ko_check = false;
    }
    return OK;
}
bool Goban::has_liberties(const Point &pt) {
    Vec tocheck;
    int w_1                = width-1;
    int h_1                = height-1;
    int my_color           = (*this)[pt];
    int my_visited_counter = ++last_visited_counter;

    tocheck.push(pt);
    global_visited[pt] = my_visited_counter;

    while (tocheck.size) {
        Point p = tocheck.remove(tocheck.size-1);

        if (p.x > 0) {
            Point &neighbor = tocheck[tocheck.size];
            neighbor.x = p.x-1;
            neighbor.y = p.y;
            int c = board[neighbor.y][neighbor.x];
            if (c == 0) {
                return true;
            }
            if (c == my_color && global_visited[neighbor.y][neighbor.x] != my_visited_counter) {
                global_visited[neighbor.y][neighbor.x] = my_visited_counter;
                ++tocheck.size;
            }
        }
        if (p.x < w_1) {
            Point &neighbor = tocheck[tocheck.size];
            neighbor.x = p.x+1;
            neighbor.y = p.y;
            int c = board[neighbor.y][neighbor.x];
            if (c == 0) {
                return true;
            }
            if (c == my_color && global_visited[neighbor.y][neighbor.x] != my_visited_counter) {
                global_visited[neighbor.y][neighbor.x] = my_visited_counter;
                ++tocheck.size;
            }
        }
        if (p.y > 0) {
            Point &neighbor = tocheck[tocheck.size];
            neighbor.x = p.x;
            neighbor.y = p.y-1;
            int c = board[neighbor.y][neighbor.x];
            if (c == 0) {
                return true;
            }
            if (c == my_color && global_visited[neighbor.y][neighbor.x] != my_visited_counter) {
                global_visited[neighbor.y][neighbor.x] = my_visited_counter;
                ++tocheck.size;
            }
        }
        if (p.y < h_1) {
            Point &neighbor = tocheck[tocheck.size];
            neighbor.x = p.x;
            neighbor.y = p.y+1;
            int c = board[neighbor.y][neighbor.x];
            if (c == 0) {
                return true;
            }
            if (c == my_color && global_visited[neighbor.y][neighbor.x] != my_visited_counter) {
                global_visited[neighbor.y][neighbor.x] = my_visited_counter;
                ++tocheck.size;
            }
        }
    }


    return false;
}
int  Goban::remove_group(Point move, Vec &possible_moves) {
    Goban       visited;
    Vec         tocheck;
    NeighborVec neighbors;
    int         n_removed = 0;
    int         my_color  = (*this)[move];

    tocheck.push(move);
    visited[move] = true;

    while (tocheck.size) {
        Point p = tocheck.remove(0);

        (*this)[p] = 0;
        possible_moves.push(p);
        n_removed++;

        board.getNeighbors(p, neighbors);

        for (int i=0; i < neighbors.size; ++i) {
            Point neighbor = neighbors[i];
            if (visited[neighbor]) continue;
            visited[neighbor] = true;

            int c = (*this)[neighbor];
            if (c == my_color) {
                tocheck.push(neighbor);
            }
        }
    }

    return n_removed;;
}
bool Goban::is_eye(Point pt, Color player) {
    if ((pt.x == 0        || board[pt.y][pt.x-1] == player) &&
        (pt.x == width-1  || board[pt.y][pt.x+1] == player) &&
        (pt.y == 0        || board[pt.y-1][pt.x] == player) &&
        (pt.y == height-1 || board[pt.y+1][pt.x] == player))
    {
        return true;
    }
    return false;
}
int  Goban::score() {
    int ret = 0;
    for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
            ret += board[y][x];
        }
    }
    return ret;
}
void Goban::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}
void Goban::clearBoard() {
    board.clear();
}
