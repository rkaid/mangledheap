/*
 * Display handling etc.
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <vector>
#include "libtcod.hpp"

#define CHARS_X 128
#define CHARS_Y 75

#define LEFT_X 0
#define LEFT_Y 0
#define LEFT_W (CHARS_X - 90)
#define LEFT_H 60

#define MAP_X (LEFT_W)
#define MAP_Y 0
#define MAP_W 90
#define MAP_H 60

#define BOTTOM_X 0
#define BOTTOM_Y (LEFT_H)
#define BOTTOM_W CHARS_X
#define BOTTOM_H (CHARS_Y - MAP_H)

#define FONT "fonts/terminal8x14_gs_ro.png"
#define MAX_MESSAGE_SIZE 115

struct message_t {
        int num;
        char message[MAX_MESSAGE_SIZE];
        TCODColor color;
};

class Display {
        public:
                Display();
                ~Display();
                void set_resolution(int w, int h);
                void set_title(char *window_title);
                char *get_title();
                void draw_game_screen();
                void draw_left_window();
                void update();
                void put(int x, int y, int c, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);
                void putmap(int x, int y, int c, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT);
                void putmap(int x, int y, int c, TCODColor &fg, TCODColor &bg);
                void message(const char *message, ...);
                void messagec(TCODColor c, const char *message, ...);
                bool askyn();
                void print_messages();
                void touch() { touched = true; };
                void clear();
                TCOD_key_t get_key(bool flush);
                TCOD_key_t wait_for_key();
                TCODColor get_random_color();
                void print_npc_name(int x, int y, char *name);
                void print_npc_name(int x, int y, char *name, TCODColor fg, TCODColor bg);
                void flushem();
                void intro();
        protected:
        private:
                bool touched;
                int width, height;
                int chars_x, chars_y;
                char title[256];
                TCODConsole *console;
                TCODConsole *map;
                TCODConsole *left;
                TCODConsole *bottom;
                vector <struct message_t> message_list;
};

#define COLOR_FEAR      TCODColor::flame
#define COLOR_LESSFEAR  TCODColor::lime
#define COLOR_FATAL     TCODColor::darkFuchsia
#define COLOR_INFO      TCODColor::amber
#define COLOR_HORROR    TCODColor::crimson
#define COLOR_ERROR     TCODColor::red
#define COLOR_BOOK      TCODColor::azure
#define COLOR_GOOD      TCODColor::green

#endif

// vim: fdm=syntax guifont=Terminus\ 8
