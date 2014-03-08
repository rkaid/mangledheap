/*
 * Roguelike Engine in C++
 *
 * In this particular instance, used to make the 7DRL "Mangled Heap!"
 * 
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

using namespace std;

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include "libtcod.hpp"

#include "actor.h"
#include "command.h"
#include "debug.h"
#include "display.h"
#include "game.h"
#include "player.h"
#include "npc.h"
#include "sound.h"
#include "world.h"
#include "item.h"

Game g;
Display *display;
Player *player;
NPC *npc;
World *world;
SoundEngine *audio;

/* 
 * The actual item definitions.
 * Reading them from a text file would be nicer, but would also require more work.
 */
struct item_definition item_definitions[] = {
    // name                 char  type         flags       value chance 
    // Weapons
    { "knife",               ')', it_weapon,   IF_WIELDABLE, 10, 30 },
    // Clothing
    // Tools and other things
};

signed int ability_modifier(int ab)
{
    return ((ab / 2) - 5);
}

bool fiftyfifty()
{
    int i;

    i = 1 + (rand() % 100);

    if(i <= 50)
        return true;
    else
        return false;
}

bool one_in(int chance)
{
    int i;

    i = ri(1,chance);
    if(i == chance)
        return true;
    else
        return false;
}

int dice(int num, int sides, signed int modifier)
{
    int i, result;

    result = modifier;
    for(i=0;i<num;i++) {
        result += 1 + (rand() % sides);
    }

    return result;
}

int ri(int a, int b) 
{
    int result;
    result = (a + (rand() % (b-a+1)));

    return result;
}

void clean_up()
{
    delete player;
    delete world;
    //delete audio;
    //delete [] npc;
    //delete game;
    delete display;
}

void init_npcs()
{
}

void init_areas()
{
    world->generate_stairs();

    //world->current_area = (int)floor_1;
    //world->a = &world->area[(int)floor_1];
}

void init_player()
{
    for(int i=0;i<80;++i)
        cout << " " << endl;

    player->create();

    player->setxy(world->get_random_walkable_cell(0));
    player->setprevxy(player->getxy());
    player->area = world->get_current_area();
    world->set_inhabitant(player);
}

void init_item_definitions()
{
    Item *i;

    for(size_t x = 0; x < (sizeof(item_definitions) / sizeof(struct item_definition)); x++) {
        i = new Item(item_definitions[x]);
        g.itemdef.push_back(*i);
        delete i;
    }
}

int main(int argc, char **argv)
{
    //unsigned int seed = time(0);
    //srand(seed);

    //game = new Game;
    //audio = new SoundEngine;
    world = new World;
    player = new Player;

    init_item_definitions();
    init_areas();
    init_player();
    //init_npcs();

    display = new Display;

    //audio->initialize();
    //audio->load_all_files();

    //audio->play_music(SOUND_MUSIC_HOUSE_OF_LEAVES);
    //audio->play_sound(SOUND_EFFECT_STORM01, 0);

    display->message("");     // "kickstart" the messaging system!
    g.loop();

    clean_up();
    return 0;
}

// vim: fdm=syntax guifont=Terminus\ 8
