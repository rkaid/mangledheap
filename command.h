/*
 *
 * Handle commands / input
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _COMMAND_H
#define _COMMAND_H

using namespace std;

#include <vector>
#include "libtcod.hpp"

enum command_type {
        cmd_nothing = 0,
        cmd_move_left,
        cmd_move_right,
        cmd_move_up,
        cmd_move_down,
        cmd_move_nw,
        cmd_move_ne,
        cmd_move_sw,
        cmd_move_se,
        cmd_exit,
        cmd_wait,
        cmd_close_door,
        cmd_stairs,
        cmd_activate,
        cmd_special_1,
        cmd_special_2,
        cmd_special_3,
        cmd_special_4,
        cmd_special_5,
        cmd_special_6,
        cmd_special_7,
        cmd_special_8,
        cmd_special_9,

        // debug / development commands
        cmd_all_visible,
};

struct command_t {
        TCOD_key_t key;
        command_type cmd;
};

class Command {
        public:
                Command();
                ~Command();
                void add_command(TCOD_key_t key, command_type cmd);
                command_type get_command();
        private:
                vector <struct command_t> command_list;
};

// prototypes

void init_commands(Command *c);

#endif

// vim: fdm=syntax guifont=Terminus\ 8
