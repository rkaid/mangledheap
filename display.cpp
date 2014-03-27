/*
 * Roguelike Engine in C++
 *
 * Display handling with libtcod
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

using namespace std;

#include <iostream>
#include <vector>
#include <cstdarg>
#include <cstdio>

#include "libtcod.hpp"
#include "debug.h"
#include "display.h"
#include "common.h"
#include "actor.h"
#include "player.h"
#include "monster.h"
#include "world.h"
#include "game.h"

extern World *world;
extern Player *player;
extern Monster *monster;
extern Game g;

TCODColor colorlist[] = {
    TCODColor::white,
    TCODColor::red,
    TCODColor::flame,
    TCODColor::orange,
    TCODColor::yellow,
    TCODColor::lime,
    TCODColor::chartreuse,
    TCODColor::green,
    TCODColor::sea,
    TCODColor::turquoise,
    TCODColor::cyan,
    TCODColor::sky,
    TCODColor::azure,
    TCODColor::blue,
    TCODColor::han,
    TCODColor::violet,
    TCODColor::purple,
    TCODColor::fuchsia,
    TCODColor::magenta,
    TCODColor::pink,
    TCODColor::crimson,
    TCODColor::peach,
    TCODColor::celadon,
    TCODColor::grey,
};

Display::Display()
{
    char t[] = "Mangled Heap!";

    set_resolution(1024, 600);
    set_title(t);

    TCODConsole::initRoot(chars_x, chars_y, title, false, TCOD_RENDERER_SDL);
    console = new TCODConsole(chars_x, chars_y);
    console->setCustomFont(FONT, TCOD_FONT_TYPE_GRAYSCALE | TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
    console->root->setCustomFont(FONT, TCOD_FONT_TYPE_GRAYSCALE | TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
    console->root->setKeyboardRepeat(200, 50);

    for(int i = 0; i<13; ++i) {
        message_t item;
        item.num = 0;
        strcpy(item.message, "");
        message_list.push_back(item);
    }
    touched = true;
}

Display::~Display()
{
    delete console;
}

void Display::set_resolution(int w, int h)
{
    width = w;
    height = h;
    chars_x = CHARS_X;
    chars_y = CHARS_Y;
    dbg("Set resolution to %d,%d - chars x,y = %d,%d", w, h, chars_x, chars_y);
}

void Display::set_title(char *window_title)
{
    strcpy(title, window_title);
}

TCOD_key_t Display::get_key(bool flush)
{
    if(flush)
        console->root->flush();
    return console->root->checkForKeypress(TCOD_KEY_PRESSED);
}

/*TCOD_key_t Display::wait_for_key()
{
    TCOD_key_t k;
    console->flush();

    k = console->waitForKeypress(true);

    return k;
}*/

TCOD_key_t Display::wait_for_key()
{
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    TCOD_event_t ev;
    
    ev = TCODSystem::waitForEvent(TCOD_EVENT_ANY,&key,&mouse,true);
    if (ev == TCOD_EVENT_KEY_PRESS)
        return key;

    return key;
}

TCODColor Display::get_random_color()
{
    int i = ri(0, (sizeof(colorlist)/sizeof(TCODColor)));

    return colorlist[i];
}

char *Display::get_title()
{
    return title;
}

void Display::draw_left_window()
{
    int x, y;

    x = LEFT_X+1;
    y = LEFT_Y+2;

    console->printEx(x+17, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Name: %s", player->getname()); y++; y++;
    console->printEx(x+17, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Dungeon level: %d", world->current_area+1); y++; y++;
    console->print(x, y, ""); y++;
    y++;
    console->print(x, y, ""); y++;
    console->print(x, y, ""); y++;
    console->print(x, y, ""); y++;
    y++;
    x+=17;
    console->setDefaultForeground(TCODColor::azure);
    console->printEx(x-1, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Strength:    %d", player->str->get()); y++;
    console->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Health:      %d", player->health->get()); y++;
    console->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Mangledness: %d", player->mangled->get()); y++;
    console->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Heapiness:   %d", player->heapiness->get()); y++;
    console->setDefaultForeground(TCODColor::white);

    y++;
    console->setDefaultForeground(TCODColor::green);
    console->print(x, y, ""); y++;
    console->setDefaultForeground(TCODColor::white);

    vector<SpecialAttack>::iterator it;
    int i;
    for(it = player->special.begin(), i = 1; it != player->special.end(); ++it, ++i) {
        console->setDefaultForeground(TCODColor::azure);
        console->print(x, y, "%d. %s %d", i, it->name, it->level); y++;
    }


    console->setDefaultForeground(TCODColor::white);
}

void Display::draw_game_screen()
{
    console->printFrame(LEFT_X, LEFT_Y, LEFT_W, LEFT_H, true);
    console->printFrame(MAP_X, MAP_Y, MAP_W, MAP_H, false);
    console->printFrame(BOTTOM_X, BOTTOM_Y, BOTTOM_W, BOTTOM_H, true);
    draw_left_window();
}

void Display::update()
{
    if(!touched)
        return;

    this->draw_game_screen();

    world->draw_map();

    display->print_messages();

    TCODConsole::blit(console, 0, 0, chars_x, chars_y, TCODConsole::root, 0.1, 0.1);

    TCODConsole::flush();

    touched = false;

    //TCODConsole::blit(left, 0, 0, LEFT_W, LEFT_H, console, LEFT_X, LEFT_Y, 0.2, 0.2);
    //TCODConsole::blit(map, 0, 0, MAP_W, MAP_H, console, MAP_X, MAP_Y, 0.2, 0.2);
    //TCODConsole::blit(bottom, 0, 0, BOTTOM_W, BOTTOM_H, console, BOTTOM_X, BOTTOM_Y, 0.2, 0.2);
}

void Display::put(int x, int y, int c, TCOD_bkgnd_flag_t flag)
{
    console->putChar(x, y, c, flag);
}

void Display::putmap(int x, int y, int c, TCOD_bkgnd_flag_t flag)
{
    console->putChar(MAP_X + x, MAP_Y + y, c, flag);
}

void Display::putmap(int x, int y, int c, TCODColor &fg, TCODColor &bg)
{
    console->putCharEx(MAP_X + x, MAP_Y + y, c, fg, bg);
}

//void Display::print_monster_name(int x, int y, char *name)
//{
//    //console->printEx(LEFT_X + 1, LEFT_Y + 20, TCOD_BKGND_DEFAULT, TCOD_LEFT, name);
//}
//
//void Display::print_monster_name(int x, int y, char *name, TCODColor fg, TCODColor bg)
//{
//    //console->printEx(LEFT_X + 1, LEFT_Y + 20, TCOD_BKGND_DEFAULT, TCOD_LEFT, name);
//}

void Display::print_messages()
{
    int y;
    vector <struct message_t>::iterator i; 

    y = 13;
    for(i = message_list.end(); i != message_list.begin(); --i) {
        if(y < 1)
            break;
        if(strcmp(i->message, "")) {
            console->setDefaultForeground(i->color);
            console->print(BOTTOM_X + 1, BOTTOM_Y + y, i->message);
            console->setDefaultForeground(TCODColor::white);
            --y;
        }
    }
}

void Display::message(const char *message, ...)
{
    va_list argp;
    char s[1000];
    message_t item;

    va_start(argp, message);
    vsprintf(s, message, argp);
    va_end(argp);

    strcpy(item.message, s);
    item.color = TCODColor::white;

    message_list.push_back(item);

    //console->print(BOTTOM_X + 1, BOTTOM_Y + 13, message);
}

bool Display::askyn()
{
    TCOD_key_t key;
again: 
    console->flush();
    key = display->wait_for_key();
    
    if(key.c == 'y' || key.c == 'Y') {
        return true;
    }
    
    if(key.c == 'n' || key.c == 'N') {
        return false;
    } 

    //display->message("Please choose [y]es or [n]o.");
    display->touch();
    display->update();
    goto again;
}

void Display::messagec(TCODColor c, const char *message, ...)
{
    va_list argp;
    char s[1000];
    message_t item;

    va_start(argp, message);
    vsprintf(s, message, argp);
    va_end(argp);

    strcpy(item.message, s);
    item.color = c;

    message_list.push_back(item);

    //console->print(BOTTOM_X + 1, BOTTOM_Y + 13, message);
}

void Display::clear()
{
    console->clear();
}

void Display::flushem()
{
    console->flush();
}

void Display::intro()
{
#ifndef DEVELOPMENT_MODE
    TCODConsole *c;

    c = new TCODConsole(chars_x, chars_y);

    int x = chars_x/2;
    int y = 3;

    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "M A N G L E D   H E A P !"); y++;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=3;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "It was really a beautiful day. On your way to the %s of Happiness, you heard birds chirping merrily, you saw deer", fiftyfifty() ? "Caverns" : "Dungeons"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "frolicking on the meadow and rabbits humping each other amongst the trees like there was no tomorrow."); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Ah! To finally be an adventurer! A Hero! The subject of countless, soon-to-be-written epic songs!"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Songs which for centuries will regale and inspire peasants and kings alike!"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "So, with great anticipation and zero hesitation, torch in one hand and sword in the other,"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "you entered the darkness."); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Two heartbeats later, a huge stone door slammed shut behind you. But you were fearless, and ventured on."); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "And before you knew it you were assaulted by a gang of horrible monsters!"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "You tried to fight, but - well - let's just say it would've been easier to be the headmaster of"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "the Aviation Academy for Pigs."); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "It seems the rabbits were right after all."); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Mere heartbeats later - if you still had heartbeats to count - a voice booms all around you:"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "YOU HAVE BEEN TRANSFORMED INTO A MANGLED HEAP!"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, ""); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "But as you're standing on the Threshold of Death's Door, a mysterious force pulls you back to Life!"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Unfortunately, you are still a mangled heap."); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "On the positive side, you sense that you now have the power to attach body parts to the mangled heap"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "that once was your body. With a sudden feeling of great hope and optimism, you embark on a new quest -"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "to build a new body (as strange as that may seem)!"); y+=2;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "It's not every day you get a second chance at life, right?"); y+=2;

    y += 6;
    c->printEx(x, y, TCOD_BKGND_DEFAULT, TCOD_CENTER, "- PRESS ANY KEY TO CONTINUE -"); y+=2;



    TCODConsole::blit(c, 0, 0, chars_x, chars_y, TCODConsole::root, 0.1, 0.1);
    TCODConsole::flush();
    display->wait_for_key();

    for (int fade=255; fade >= 0; --fade) {
        TCODConsole::setFade(fade,TCODColor::black);
        TCODConsole::flush();
    }

    delete c;
#endif
}
// vim: fdm=syntax guifont=Terminus\ 8
