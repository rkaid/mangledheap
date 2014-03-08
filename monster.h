/*
 * Monster specific stuff
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _Monster_H
#define _Monster_H

#include "libtcod.hpp"
#include "actor.h"

enum goal_type_e {
    no_goal = 0,
    move_random,
    move_upstairs,
    move_downstairs,
    kill_player,
};

class Monster: public Actor {
    public:
        Monster();
        ~Monster();
        bool is_player() { return false; }
        void setai(int which);
        void ai();
        void set_goal(coord_t c) { goal = c; has_goal = true; };
        void clear_goal() { has_goal = false; goal_type = no_goal; };
        void use_stairs();
        void set_in_combat();

        int  which_ai;
        TCODPath *path;
        bool has_goal;
        bool in_combat;
        goal_type_e goal_type;
    private:
        void random_ai();
        void path_ai();
        void set_random_goal();

        coord_t goal;
};

#endif
// vim: fdm=syntax guifont=Terminus\ 8

