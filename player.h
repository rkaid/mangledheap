/*
 * Player specific stuff
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _PLAYER_H
#define _PLAYER_H

#include "actor.h"

#define PLAYER_STARTX (MAP_W / 2)
#define PLAYER_STARTY (MAP_H - 4)


class Player: public Actor {
        public:
                Player();
                ~Player();
                bool is_player() { return true; }
                void look();
                void create();      // generate your player character.
                void endturn();
                void die();
                void use_stairs();
                void set_in_combat();
        private:
                // Stats
                //int body;         // 1-20, how physically strong you are
                //int mind;         // 1-20, how mentally strong you are
                //int soul;         // 1-20, how spiritually strong you are (does that make sense?!)

                // "Resources"
                //int sanity;       // 0-100, where 0 is completely bonkers and 100 is a mental health extravaganza
                //int fear;         // 0-100, where 0 is zen master in nirvana, 100 is FUCKING SCARED MESSED MY PANTS
                //int health;       // 0-100, where 0 is totally dead and gone, 100 is healthiest fucking person evar
};

#endif
// vim: fdm=syntax guifont=Terminus\ 8
