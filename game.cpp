/*
 * Roguelike Engine in C++
 *
 * The Game (loop etc.)
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

using namespace std;

#include <iostream>
#include <string>

#include "libtcod.hpp"
#include "actor.h"
#include "debug.h"
#include "display.h"
#include "game.h"
#include "player.h"
#include "monster.h"
#include "world.h"
#include "item.h"

extern Display *display;
extern Player *player;
extern Monster *monster;
extern World *world;
extern Game g;

Game::Game()
{
    init_commands(&this->cmd);

    name = "Mangled Heap!";
    version = "0.0.1";
    wizmode = false;
    clock.settime(0, 0, 0);
    monster_counter = 0;

    running = true;
}

Game::~Game()
{
}

bool Game::is_running()
{
    return running;
}

void Game::endthegame()
{
    running = false;
}

void Game::intro()
{
}

void Game::end_turn()
{
    if(player->has_moved()) {
        /*
        for(int i=0;i<12;i++) {
            if(monster[i].is_alive()) {
                //if(player->can_see(monster[i].getx(), monster[i].gety()))
                //        display->message("You can see %s!", monster[i].getname());
                monster[i].ai();
            }
        }
        */

        player->endturn();
        player->look();
    }
}

void Game::loop()
{
    command_type c;
    TCOD_key_t key;

    display->update();
    for (int fade=0; fade <= 255; fade++) {
        TCODConsole::setFade(fade,TCODColor::black);
        TCODConsole::flush();
    }
    while (this->is_running()) {
        world->update_fov();
        display->update();
        c = cmd.get_command();
        player->moved(false);
        switch(c) {
            case cmd_exit:
                endthegame();
                break;
            case cmd_move_left:
                player->move_left();
                end_turn();
                break;
            case cmd_move_right:
                player->move_right();
                end_turn();
                break;
            case cmd_move_up:
                player->move_up();
                end_turn();
                break;
            case cmd_move_down:
                player->move_down();
                end_turn();
                break;
            case cmd_move_nw:
                player->move_nw();
                end_turn();
                break;
            case cmd_move_ne:
                player->move_ne();
                end_turn();
                break;
            case cmd_move_sw:
                player->move_sw();
                end_turn();
                break;
            case cmd_move_se:
                player->move_se();
                end_turn();
                break;
            case cmd_close_door:
                world->close_nearest_door(player);
                break;
            case cmd_stairs:
                player->use_stairs();
                end_turn();
                break;
            case cmd_activate:
                player->area->cell[player->getx()][player->gety()].activate();
                break;
            case cmd_wait:
                player->moved();
                end_turn();
                break;
            case cmd_special_1:
                display->message("SPECIAL ATTACK 1");
                display->touch();
                break;
            case cmd_special_2:
                display->message("SPECIAL ATTACK 2");
                display->touch();
                break;
            case cmd_special_3:
                display->message("SPECIAL ATTACK 3");
                display->touch();
                break;
                // Debug/development commands:
            case cmd_all_visible:
                player->area->set_all_visible();
                world->update_fov();
                g.clock += 3700;
                wizmode = true;
                break;
            default:
                break;
        }
    }

    display->messagec(COLOR_FATAL, "Game over. Press ESC to exit.");
    display->touch();
    display->update();

    key = display->get_key(true);
    while(key.vk != TCODK_ESCAPE)
        key = display->get_key(true);
}

// vim: fdm=syntax guifont=Terminus\ 8
