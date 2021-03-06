/*
 * Roguelike Engine in C++
 *
 * Player specific stuff.
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */


using namespace std;

#include <iostream>
#include <sstream>
#include <string>

#include "libtcod.hpp"
#include "game.h"
#include "actor.h"
#include "player.h"
#include "common.h"
#include "display.h"
#include "world.h"

extern World   *world;
extern Player  *player;
extern Display *display;
extern Game g;

Player::Player()
{
        setchar('@');
        setcolors(TCODColor::white, TCODColor::red);
        setfovradius(9);
        mangled = new Stat(100,100);
        heapiness = new Stat(100,100);
}

Player::~Player()
{
        //dbg("Goodbye, cruel world.");
}

void Player::look()
{
        cell_type type;

        type = world->get_cell_type(player->area, player->getxy());
        switch(type) {
                case stairs_up:
                        display->messagec(COLOR_INFO, "There is a staircase leading up here.");
                        break;
                case stairs_down:
                        display->messagec(COLOR_INFO, "There is a staircase leading down here.");
                        break;
                default:
                        break;
        }
        if(player->area->cell[this->getx()][this->gety()].item)
            display->messagec(COLOR_GOOD, "There is a %s here.", player->area->cell[this->getx()][this->gety()].item->get_name());
}

void Player::die()
{
        display->messagec(COLOR_FATAL, "You have died...");
        display->update();
        g.endthegame();
}

void Player::create()
{
        char *name_c;
        string input;
        char c, d;
        int sstr;

        for(int a=0;a<10;a++)
            cout << endl;
#ifdef DEVELOPMENT_MODE
        cout << "Welcome to " << g.name << " - v" << g.version << endl << endl;
#else
        cout << "Welcome to " << g.name << " - v" << g.v_major << "." << g.v_minor << endl << endl;
#endif
        cout << "First, you will need to create your player character using this very simple character generator." <<endl;
        cout << "What's your name? ";
        getline(cin, input);
        name_c = new char[input.size()+1];
        name_c[input.size()] = 0;
        memcpy(name_c, input.c_str(), input.size());
        setname(name_c);
        cout << "Now we will randomly generate your stats:" << endl;
again:
        if(0) {            // do each one 3 times and take average result - makes it less likely to get stats in the upper or lower end of the scale. might be good for a "difficult mode"?
                sstr = 0;
                sstr += 10 + ri(-9, 10);
                sstr += 10 + ri(-9, 10);
                sstr += 10 + ri(-9, 10);
                sstr /= 3;
        } else {
                sstr = 10 + ri(-9, 10);
                sstr = 10 + ri(-9, 10);
                sstr = 10 + ri(-9, 10);
        }

        cout << endl;
        cout << "Strength: " << sstr << endl;
        cout << "Are you happy with these results (Y/n)? ";
        c = getchar();
        d = c;
        if(c == '\n')
                goto finish;
        c = getchar();
        if(d == 'n')
                goto again;
finish:

        this->str->set(sstr);
        this->str->setmax(sstr);
        return;
}

void Player::use_stairs()
{
        if(player->area->cell[player->getx()][player->gety()].get_type() == stairs_up) {
                world->clear_inhabitant(player->area, player->getxy());
                world->current_area--;
                world->a = &world->area[world->current_area];
                player->area = world->a;
                player->setxy(player->area->stairs_down);
                world->set_inhabitant(player);
                display->clear();
                display->touch();
                display->update();
                player->moved();
        } else if(player->area->cell[player->getx()][player->gety()].get_type() == stairs_down) {
                world->clear_inhabitant(player->area, player->getxy());
                world->current_area++;
                world->a = &world->area[world->current_area];
                player->area = world->a;
                player->setxy(player->area->stairs_up);
                world->set_inhabitant(player);
                display->clear();
                display->touch();
                display->update();
                player->moved();
        } else {
                display->messagec(COLOR_ERROR, "There are no stairs here!");
                display->touch();
        }
}

void Player::set_in_combat()
{
}

void Player::endturn()
{
        //g.clock += ri(5,10);
}



// vim: fdm=syntax guifont=Terminus\ 8
