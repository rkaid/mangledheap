/*
 * Roguelike Engine in C++
 *
 * Everything related to the world itself - maps, levels, generators, whatnot.
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

using namespace std;

#include <iostream>

#include "actor.h"
#include "world.h"
#include "common.h"
#include "player.h"
#include "game.h"

extern Player *player;
extern World *world;
extern Game g;

const char *area_name[] = {
};

class MyCallback : public ITCODBspCallback
{
    public :
        bool visitNode(TCODBsp *node, void *userData)
        {
            //dbg("making room, x=%d y=%d w=%d h=%d", node->x, node->y, node->w, node->h);
            world->a->make_room(node->x, node->y, node->x + node->w-2, node->y + node->h-2);

            if(node->horizontal) {
                int x1 = node->x - 2;
                int y1 = node->y - 2;
                int x2 = node->x + node->w - 2;
                int y2 = node->y + node->h - 2;
                world->a->make_door(x1, ri(y1, y2), false);
                world->a->make_door(x2, ri(y1, y2), false);
            } else {
                int x1 = node->x;
                int y1 = node->y;
                int x2 = node->x + node->w - 2;
                int y2 = node->y + node->h - 2;
                world->a->make_door(ri(x1, x2), y1, false);
                world->a->make_door(ri(x1, x2), y2, false);
            }

            return true;
        }
};

/*
 * class: Cell
 */
Cell::Cell()
{
    flags = 0;
    visible = false;
    activated = false;
    cell_seen = false;
    inhabitant = NULL;
    c = 0;
    item = 0;

    this->set_floor();
}

Cell::~Cell()
{
}

bool Cell::is_walkable()
{
    if(this->inhabitant) {
        return false;
    }

    switch(this->type) {
        case floor:
        case door_open:
        case door_closed:
        case stairs_up:
        case stairs_down:
            return true;
        case wall:
        case nothing:
            return false;
        default:
            return false;
    }

    return false;
}

bool Cell::is_transparent()
{
    switch(this->type) {
        case floor:
        case door_open:
        case stairs_up:
        case stairs_down:
            return true;
        case wall:
        case door_closed:
        case nothing:
            return false;
        default:
            return true;
    }
}

bool Cell::is_visible()
{
    return this->visible; 
}

void Cell::set_wall()
{
    this->type = wall;
    this->fg = TCODColor::darkerFlame;
    this->bg = TCODColor::black;
    c = '#';
}

void Cell::set_floor()
{
    this->type = floor;
    this->fg = TCODColor::darkerGrey;
    this->bg = TCODColor::black;
    c = '.';
}

void Cell::set_stairs_up()
{
    this->type = stairs_up;
    this->fg = TCODColor::yellow;
    this->bg = TCODColor::black;
    c = '<';
}

void Cell::set_stairs_down()
{
    this->type = stairs_down;
    this->fg = TCODColor::yellow;
    this->bg = TCODColor::black;
    c = '>';
}

void Cell::set_door_closed()
{
    this->type = door_closed;
    this->fg = TCODColor::grey;
    this->bg = TCODColor::black;
    c = '+';
}

void Cell::set_door_open()
{
    this->type = door_open;
    this->fg = TCODColor::grey;
    this->bg = TCODColor::black;
    c = '\\';
}

void Cell::set_visibility(bool b)
{
    this->visible = b;
}

void Cell::draw(int x, int y)
{
    if(inhabitant) {
        if(inhabitant->alive) {
            if(inhabitant->area == world->a) {
                inhabitant->draw();
            }
        }
    } else {
        display->putmap(x, y, this->c, this->fg, this->bg);
    }
    display->touch();
}

void Cell::draw(int x, int y, TCODColor fore, TCODColor back)
{
    if(inhabitant) {
        if(inhabitant->alive) {
            if(inhabitant->area == world->a) {
                inhabitant->draw(fore, back);
            }
        }
    } else {
        display->putmap(x, y, this->c, fore, back);
    }
    display->touch();
}

void Cell::set_color(TCODColor fg, TCODColor bg)
{
    this->fg = fg;
    this->bg = bg;
}

cell_type Cell::get_type()
{
    return this->type;
}

void Cell::activate()
{
    switch(this->type) {
        default:
            break;
    }
    display->touch();
}

void Cell::set_type(cell_type t)
{
    switch(t) {
        case wall:
            this->set_wall();
            break;
        case floor:
            this->set_floor();
            break;
        case stairs_up:
            this->set_stairs_up();
            break;
        case stairs_down:
            this->set_stairs_down();
            break;
        case door_closed:
            this->set_door_closed();
            break;
        case door_open:
            this->set_door_open();
            break;
        default:
            dbg("unknown type!");
            break;
    }
}


/*
 * class: Area
 */
Area::Area()
{
    cell = new Cell*[AREA_MAX_X];

    for(int i = 0; i < AREA_MAX_X; ++i)
        cell[i] = new Cell[AREA_MAX_Y];

    tcodmap = new TCODMap(AREA_MAX_X, AREA_MAX_Y);
    bsp = new TCODBsp(1, 1, AREA_MAX_X, AREA_MAX_Y);
    lights_on = true;
}

Area::~Area()
{
    for(int i = 0; i < AREA_MAX_X; ++i)
        delete [] cell[i];

    delete [] cell;
    delete tcodmap;
    delete bsp;
}

void Area::build_tcodmap()
{
    int x, y;

    for(x = 0; x < AREA_MAX_X; ++x) {
        for(y = 0; y < AREA_MAX_Y; ++y) {
            tcodmap->setProperties(x, y, cell[x][y].is_transparent(), cell[x][y].is_walkable());
        }
    }
}

void Area::set_all_invisible()
{
    int x, y;

    for(x = 0; x < AREA_MAX_X; ++x) {
        for(y = 0; y < AREA_MAX_Y; ++y) {
            cell[x][y].set_visibility(false);
        }
    }
}

void Area::set_all_visible()
{
    int x, y;

    for(x = 0; x < AREA_MAX_X; ++x) {
        for(y = 0; y < AREA_MAX_Y; ++y) {
            this->tcodmap->setProperties(x, y, true, cell[x][y].is_walkable());
            this->cell[x][y].seen();
        }
    }
    player->setfovradius(0);
}

void Area::update_visibility()
{
    int x, y;

    for(x = 0; x < AREA_MAX_X; ++x) {
        for(y = 0; y < AREA_MAX_Y; ++y) {
            if(this->tcodmap->isInFov(x, y))
                cell[x][y].set_visibility(true);
            else
                cell[x][y].set_visibility(false);
        }
    }
}

direction Area::generate_starting_room()
{
    int x1, x2, y1, y2;
    direction d;

    x1 = PLAYER_STARTX - 3;
    x2 = PLAYER_STARTX + 3;
    y1 = PLAYER_STARTY + 1;
    y2 = PLAYER_STARTY - 6;
    make_room(x1, y1, x2, y2);

    d = (direction) d(1, 4);

    if(d == north || d == south) {
        int dx, dy;
        dx = ri(x1+1, x2-1);
        dy = y2;
        make_door(dx, dy, false);
    }
    /*if(d == south) {
      int dx, dy;
      dx = ri(x1, x2);
      dy = y1;
      make_door(dx, dy, false);
      }*/
    if(d == west) {
        int dx, dy;
        dx = x1;
        dy = ri(y1-2, y2+1);
        make_door(dx, dy, false);
    }
    if(d == east) {
        int dx, dy;
        dx = x2;
        dy = ri(y1-2, y2+1);
        make_door(dx, dy, false);
    }

    return d;
}

void Area::generate(area_id_type identifier)
{
}

void Area::place_furniture(area_id_type identifier)
{
}

void Area::frame()
{
    horizontal_line(1);
    horizontal_line(AREA_MAX_Y - 2);
    vertical_line(1);
    vertical_line(AREA_MAX_X - 2);
}

void Area::make_room(int x1, int y1, int x2, int y2)
{
    horizontal_line(x1, y1, x2);
    horizontal_line(x1, y2, x2);
    vertical_line(x1, y1, y2);
    vertical_line(x2, y1, y2);
}

void Area::make_door(int x, int y, bool open)
{
    //dbg("Making door at %d,%d (%s)", x, y, open ? "open" : "closed");

    if(x <= 1)
        return;
    if(y <= 1)
        return;
    if(cell[x][y].get_type() != wall)
        return;

    if(open)
        cell[x][y].set_door_open();
    else
        cell[x][y].set_door_closed();
}

void Area::make_stairs_up()
{
    coord_t co;

    co = this->get_random_floor_cell();
    cell[co.x][co.y].set_stairs_up();
    stairs_up = co;
}

void Area::make_stairs_down()
{
    coord_t co;

    co = this->get_random_floor_cell();
    cell[co.x][co.y].set_stairs_down();
    stairs_down = co;
}

coord_t Area::get_random_floor_cell()
{
    coord_t co;

again:
    co.x = ri(1, AREA_MAX_X-2);
    co.y = ri(1, AREA_MAX_Y-2);
    while(this->cell[co.x][co.y].get_type() != floor) {
        co.x = ri(1, AREA_MAX_X-2);
        co.y = ri(1, AREA_MAX_Y-2);
    }

    if(this->cell[co.x][co.y].get_type() == floor)
        return co;
    else
        goto again;
}

coord_t Area::get_random_empty_floor_cell()
{
    coord_t co;

again:
    co.x = ri(1, AREA_MAX_X-2);
    co.y = ri(1, AREA_MAX_Y-2);
    while(this->cell[co.x][co.y].get_type() != floor ) {
        co.x = ri(1, AREA_MAX_X-2);
        co.y = ri(1, AREA_MAX_Y-2);
    }

    if(this->cell[co.x][co.y].get_type() == floor && this->cell[co.x][co.y].item == 0)
        return co;
    else
        goto again;
}

void Area::horizontal_line(int y)
{
    for(int i = 0; i < AREA_MAX_X; ++i)
        cell[i][y].set_wall();
}

void Area::horizontal_line(int x, int y, int x2)
{
    if(x2 > x) {
        for(int j = x; j <= x2; ++j)
            cell[j][y].set_wall();
    } else {
        for(int j = x; j >= x2; --j)
            cell[j][y].set_wall();
    }
}

void Area::horizontal_line(int x, int y, int x2, cell_type t)
{
    if(x2 > x) {
        for(int j = x; j <= x2; ++j)
            cell[j][y].set_type(t);
    } else {
        for(int j = x; j >= x2; --j)
            cell[j][y].set_type(t);
    }
}

void Area::vertical_line(int x, int y, int y2)
{
    if(y2 > y) {
        for(int j = y; j <= y2; ++j)
            cell[x][j].set_wall();
    } else if(y2 < y) {
        for(int j = y; j > y2; --j)
            cell[x][j].set_wall();
    }
}

void Area::vertical_line(int x)
{
    for(int j = 0; j < AREA_MAX_Y; ++j)
        cell[x][j].set_wall();
}

void Area::vertical_line(int x, int y, int y2, cell_type t)
{
//    dbg("vertical line x=%d y=%d y2=%d t=%d", x, y, y2, (int) t);
    if(y2 > y) {
        for(int j = y; j <= y2; ++j)
            cell[x][j].set_type(t);
    } else if(y2 < y) {
        for(int j = y; j > y2; --j)
            cell[x][j].set_type(t);
    }
}

bool Area::cell_is_visible(int x, int y)
{
    return tcodmap->isInFov(x, y);
    //return cell[x][y].is_visible();
}

const char *Area::get_area_name()
{
        return NULL;
}

bool Area::is_walkable(int x, int y)
{
    return this->cell[x][y].is_walkable();
}

void Area::spawn_items(int num)
{
    int i = 0;
    vector<Item>::iterator it; 
    it = g.itemdef.begin();

    while(i != num) {
       int c = ri(1, 100);
       //cout << "c = " << c << " chance = " << it->chance << endl;
       if(c <= it->chance) {
           //cout << i << " - spawning a " << it->get_name() << endl;

           coord_t co;
           Item newitem;
           
           co = this->get_random_empty_floor_cell();
           newitem = *it;
           newitem.co = co;

           world->items.push_back(newitem); // master list of items -- TODO: needed???
           //this->items.push_back(*it);  // master list of items in one area - TODO: needed?

           this->cell[co.x][co.y].item = new Item;
           *this->cell[co.x][co.y].item = newitem;
           this->cell[co.x][co.y].item->fg = TCODColor::darkGreen;
           i++;
       } else {
           if(it == g.itemdef.end())
               it = g.itemdef.begin();
           else
               it++;
       }
    }
}


/*
 * class: World
 */
World::World()
{
    area = new Area[MAX_AREAS];
    current_area = 0;
    //a = &area[current_area];
}

World::~World()
{
    delete [] area;
}

bool World::is_walkable(Area *where, int x, int y)
{
    return where->cell[x][y].is_walkable();
}

bool World::is_closed_door(Area *where, int x, int y)
{
    if(where->cell[x][y].get_type() == door_closed)
        return true;
    else
        return false;
}

bool World::is_open_door(Area *where, int x, int y)
{
    if(where->cell[x][y].get_type() == door_open)
        return true;
    else
        return false;
}

void World::open_door(Area *where, int x, int y)
{
    where->cell[x][y].set_door_open();
    where->build_tcodmap();
}

void World::close_door(Area *where, int x, int y)
{
    where->cell[x][y].set_door_closed();
    where->build_tcodmap();
}

bool World::close_nearest_door(Actor *actor)
{
    // Close the door nearest to actor, start at northwest.
    // Returns true if successfully closed a door.

    int x, y, dx, dy;

    x = actor->getx();
    y = actor->gety();
    for(dx=-1; dx <= 1; dx++) {
        for(dy=-1; dy <= 1; dy++) {
            if(is_open_door(actor->area, x+dx, y+dy)) {
                close_door(actor->area, x+dx, y+dy);
                if(actor == player)
                    player->moved();
                return true;
            }
        }
    }

    return false;
}

void World::draw_map()
{
    int i, j;

    player->area->tcodmap->computeFov(player->getx(), player->gety(), player->getfovradius(), true, FOV_BASIC);
    for(i = 1; i < AREA_MAX_X-1; ++i) {
        for(j = 1; j < AREA_MAX_Y-1; ++j) {
            if(player->can_see(i, j) || player->area->cell[i][j].is_seen()) {
                draw_cell(i, j);
            } else {
                if(!player->area->lights_on) {
                    draw_cell(i, j, TCODColor::black, TCODColor::black); 
                }
            }

        }
    }
    display->touch();
}

void World::draw_cell(int x, int y)
{
    if(player->area->cell[x][y].inhabitant) {
        if(player->area->cell[x][y].inhabitant->alive) {
            player->area->cell[x][y].inhabitant->draw();
        }
    } else {
        if(player->area->cell[x][y].item) {
            display->putmap(x, y, player->area->cell[x][y].item->c, player->area->cell[x][y].item->fg, player->area->cell[x][y].item->bg);
        } else {
            display->putmap(x, y, a->cell[x][y].c, a->cell[x][y].fg, a->cell[x][y].bg);
        }
    }
}

void World::draw_cell(coord_t co)
{
    display->putmap(co.x, co.y, a->cell[co.x][co.y].c, a->cell[co.x][co.y].fg, a->cell[co.x][co.y].bg);
}

void World::draw_cell(int x, int y, TCODColor fg, TCODColor bg)
{
    display->putmap(x, y, player->area->cell[x][y].c, fg, bg);
}

void World::update_fov()
{
    coord_t co;

    co = player->getxy();
    player->area->tcodmap->computeFov(co.x, co.y, player->getfovradius(), true, FOV_BASIC);
}

coord_t World::get_random_walkable_cell(area_id_type id)
{
    coord_t co;

again:
    co.x = ri(1, AREA_MAX_X-2);
    co.y = ri(1, AREA_MAX_Y-2);
    while(!area[id].cell[co.x][co.y].is_walkable()) {
        co.x = ri(1, AREA_MAX_X-2);
        co.y = ri(1, AREA_MAX_Y-2);
    }

    if(area[id].cell[co.x][co.y].is_walkable())
        return co;
    else
        goto again;
}

coord_t World::get_random_floor_cell(area_id_type id)
{
    coord_t co;

again:
    co.x = ri(1, AREA_MAX_X-2);
    co.y = ri(1, AREA_MAX_Y-2);
    while(area[(int)id].cell[co.x][co.y].get_type() != floor) {
        co.x = ri(1, AREA_MAX_X-2);
        co.y = ri(1, AREA_MAX_Y-2);
    }

    if(area[(int)id].cell[co.x][co.y].get_type() == floor)
        return co;
    else
        goto again;
}

void World::set_inhabitant(Actor *actor)
{
    actor->area->cell[actor->getx()][actor->gety()].inhabitant = actor;
    actor->area_id = actor->area->get_id();
}

void World::clear_inhabitant(Area *where, coord_t co)
{
    where->cell[co.x][co.y].inhabitant = NULL;
    /*if(where->cell_is_visible(co.x, co.y))
        where->cell[co.x][co.y].draw(co.x, co.y);
    else
        where->cell[co.x][co.y].draw(co.x, co.y, TCODColor::black, TCODColor::black);*/
}

const char *World::get_cell_type(Area *where, int x, int y)
{
    switch(where->cell[x][y].get_type()) {
        case floor: return "floor"; break;
        case wall: return "wall"; break;
        case nothing: return "nothing"; break;
        case door_open: return "open door"; break;
        case door_closed: return "closed door"; break;
        default: return "unknown"; break;
    }
}

cell_type World::get_cell_type(Area *where, coord_t co)
{
    return where->cell[co.x][co.y].get_type();
}

void World::generate_stairs()
{
    int i;

    for(i=1; i<=10; i++) {
        world->area[i].make_stairs_down();
        world->area[i].make_stairs_up();
    }

    world->area[0].make_stairs_up();
    world->area[11].make_stairs_down();
}




// vim: fdm=syntax guifont=Terminus\ 8
