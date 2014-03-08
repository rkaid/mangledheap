/*
 * The Game!
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _GAME_H
#define _GAME_H

#include "common.h"
#include "command.h"
#include "display.h"
#include "item.h"

class Game {
        public:
                Game();
                ~Game();
                void loop();
                bool is_running();
                void endthegame();
                void intro();
                void end_turn();
                
                const char *name;  // the name of the game!
                const char *version;
                bool wizmode;
                Clock clock;
                vector<Item> itemdef;
                int monster_counter;
        protected:
        private:
                Command cmd;
                bool running;
};

#endif

// vim: fdm=syntax guifont=Terminus\ 8
