/*
 * Everything related to the world itself - maps, levels, generators, whatnot.
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _WORLD_H
#define _WORLD_H

#include "libtcod.hpp"
#include "actor.h"
#include "common.h"
#include "item.h"

#define AREA_MAX_X 90
#define AREA_MAX_Y 60

#define MAX_AREAS 12

enum cell_type {
        nothing = 0,
        wall,
        floor,
        door_open,
        door_closed,
        stairs_up,
        stairs_down,
};

class Cell {
        private:
                cell_type    type;
                int          flags;
                bool         visible;
                bool         activated;
                bool         cell_seen;
        protected:
        public:
                Cell();
                ~Cell();
                void set_wall();
                void set_floor();
                void set_door_closed();
                void set_door_open();

                void set_color(TCODColor fg, TCODColor bg);
                void set_visibility(bool b);
                void set_stairs_up();
                void set_stairs_down();
                void setfg(TCODColor color) { fg = color; };
                void setbg(TCODColor color) { bg = color; };
                void seen() { cell_seen = true; };
                bool is_seen() { return cell_seen; };
                TCODColor getfg() { return fg; };

                void draw(int x, int y);
                void draw(int x, int y, TCODColor fg, TCODColor bg);
                bool is_walkable();
                bool is_visible();
                bool is_transparent();
                void set_type(cell_type t);
                cell_type get_type();

                void activate();

                Actor       *inhabitant;
                char         c;
                TCODColor    fg, bg;
                Item        *item;
};

class Area {
        private:
                TCODBsp *bsp;
                //area_id_type id;
        protected:
        public:
                Area();
                ~Area();
                void generate();
                void place_furniture();
                direction generate_starting_room();
                void horizontal_line(int y);
                void horizontal_line(int x, int y, int x2);
                void horizontal_line(int x, int y, int x2, cell_type t);
                void vertical_line(int x);
                void vertical_line(int x, int y, int y2);
                void vertical_line(int x, int y, int y2, cell_type t);
                void frame();
                void build_tcodmap();
                bool cell_is_visible(int x, int y);
                bool is_walkable(int x, int y);
                void make_room(int x1, int y1, int x2, int y2);
                void make_door(int x, int y, bool open);
                void make_stairs_up();
                void make_stairs_down();
                void set_all_visible();
                void set_all_invisible();
                void update_visibility();
                /*void set_id(area_id_type identifier) { id = identifier; };
                area_id_type get_id() { return id; };*/
                const char *get_area_name();
                coord_t get_random_floor_cell();
                coord_t get_random_empty_floor_cell();
                void spawn_items(int num);

                Cell    **cell;
                TCODMap *tcodmap;
                bool lights_on;
                coord_t stairs_up;
                coord_t stairs_down;
                vector<Item> items;

                friend class Cell;
};

class World {
        private:
        protected:
        public:
                World();
                ~World();
                const char *get_cell_type(Area *where, int x, int y);
                cell_type get_cell_type(Area *where, coord_t co);
                bool is_walkable(Area *where, int x, int y);
                bool is_closed_door(Area *where, int x, int y);
                bool is_open_door(Area *where, int x, int y);
                void open_door(Area *where, int x, int y);
                void close_door(Area *where, int x, int y);
                bool close_nearest_door(Actor *actor);

                void draw_map();
                void draw_cell(int x, int y);
                void draw_cell(coord_t co);
                void draw_cell(int x, int y, TCODColor fg, TCODColor bg);
                void update_fov();
                coord_t get_random_walkable_cell(int id);
                coord_t get_random_floor_cell(int id);
                void set_inhabitant(Actor *actor);
                void clear_inhabitant(Area *where, coord_t co);
                Area *get_current_area() { return &area[current_area]; };
                void generate_stairs();

                int current_area;
                Area *area;       // becomes array of areas. the actual areas in the game.
                Area *a;          // should always point to area[current_area]

                vector<Item> items;

                friend class Cell;
};

#endif
// vim: fdm=syntax guifont=Terminus\ 8
