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
    this->fg = fiftyfifty() ? TCODColor::darkGrey : TCODColor::grey;
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
            if(inhabitant->area == player->area) {
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
            if(inhabitant->area == player->area) {
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
    this->generate();
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

bool Area::only_walls(int x1, int y1, int x2, int y2)
{
    int i, j;

    for(i = x1; i <= x2; ++i) {
        for(j = y1; j <= y2; ++j) {
            if(this->cell[i][j].get_type() != wall)
                return false;
        }
    }

    return true;
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

void Area::generate_drunken_walk()
{
    int i,j,x,y,q,r,num;

    q = ri(70, AREA_MAX_X);
    r = ri(50, AREA_MAX_Y);
    q = AREA_MAX_X*2;
    r = AREA_MAX_Y*2;

    for(i = 2; i < q; ++i) {
        x = AREA_MAX_X / 2;
        y = AREA_MAX_Y / 2;
        for(j = 2; j < r; ++j) {
            num = ri(1,4);
            switch(num) {
                case 1: x++; break;
                case 2: x--; break;
                case 3: y++; break;
                case 4: y--; break;
            }

            if(x < AREA_MAX_X && y < AREA_MAX_Y)
                this->cell[x][y].set_floor();
        }
    }
}

room_t Area::generate_room(int maxx, int maxy)
{
    room_t room;
    int min = 4;
    bool finished = false;
    int tries;

    tries = 0;
    room.x1 = room.x2 = room.y1 = room.y2 = 0;
    while(!finished) {
        int x1, y1, x2, y2;

        x1 = ri(min, AREA_MAX_X-maxx);
        x2 = x1 + ri(min, maxx);
        y1 = ri(min, AREA_MAX_Y-maxy);
        y2 = y1 + ri(min, maxy);

        // check that there is nothing else in the chosen room
        if(this->only_walls(x1, y1, x2, y2)) {
            this->make_room(x1, y1, x2, y2);
            room.x1 = x1;
            room.x2 = x2;
            room.y1 = y1;
            room.y2 = y2;
            finished = true;
        }

        ++tries;
        if(tries > 10)
            return room;
    }

    return room;
}

room_t Area::generate_room(int startx, int starty, int maxx, int maxy)
{
    room_t room;
    bool finished = false;
    int min = 4;
    int tries = 0;

    room.x1 = room.x2 = room.y1 = room.y2 = 0;
    while(!finished) {
        int x1, y1, x2, y2;
        x1 = startx;
        y1 = starty;
        x2 = x1 + ri(min, maxx);
        y2 = y1 + ri(min, maxy);

        if(x1 >= AREA_MAX_X)
            return room;
        if(y1 >= AREA_MAX_Y)
            return room;
        if(x2 >= AREA_MAX_X)
            x2 = AREA_MAX_X-1;
        if(y2 >= AREA_MAX_Y)
            y2 = AREA_MAX_Y-1;

        // check that there is nothing else in the chosen room
        //if(this->only_walls(x1, y1, x2, y2)) {
            this->make_room(x1, y1, x2, y2);
            room.x1 = x1;
            room.x2 = x2;
            room.y1 = y1;
            room.y2 = y2;
            finished = true;
        //}

        ++tries;
        if(tries > 10)
            return room;
    }

    return room;
}

void Area::generate_type_1()
{
    vector<room_t> roomlist;

    // Fill the area with walls
    this->fill();

    // Generate a drunken-walk-cavern
    this->generate_drunken_walk();

    for(int a=0;a<10;++a) {
        room_t room;

        // Generate a room
        room = this->generate_room(15,15);

        // Add room to list of rooms
        roomlist.push_back(room);
    }

    this->build_tcodmap();

    vector<room_t>::iterator it;
    for(it = roomlist.begin(); it != roomlist.end(); ++it) {

    }

    lights_on = false;
}

void Area::generate_type_2()
{
    vector<room_t> roomlist;
    room_t r;
    this->fill();
    int dx;

    dx = ri(2,4);

    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 3; ++j) {
            int sx, sy;
tryagain:
            sx = dx + ri(1,5) + (i*20);
            sy = ri(1,5) + (j*20);
            r = this->generate_room(sx, sy, 20, 20);
            DBG("i,j = %d,%d     sx,sy=%d,%d", i, j, sx, sy);
            if(r.x1 == 0)
                goto tryagain;
            roomlist.push_back(r);
        }
    }

   /* vector<room_t>::iterator it;

    for(it = roomlist.begin(); it != roomlist.end(); ++it) {
        int sx, sy, ex, ey;
        vector<room_t>::iterator dest;
        dest = it;
        dest++;
        if(dest == roomlist.end())
            goto finish1;

        sx = it->x1 + ((it->x2 - it->x1)/2);
        sy = it->y1 + ((it->y2 - it->y1)/2);

        ex = dest->x1 + ((dest->x2 - dest->x1)/2);
        ey = dest->y1 + ((dest->y2 - dest->y1)/2);

        this->line(sx, sy, ex, ey);
    }

finish1:*/

    /*
     * Horrible and quite static hallway generation code follows!
     */
    int srcroom[]  = { 0, 3, 6, 1, 4,  7, 2, 5,  8 };
    int destroom[] = { 3, 6, 9, 4, 7, 10, 5, 8, 11 };
    room_t src, dest;

    for(int q=0;q<9;q++) {
        int sx, sy, ex, ey;
        
        src = roomlist.at(srcroom[q]);
        dest = roomlist.at(destroom[q]);

        sx = src.x1 + ((src.x2 - src.x1)/2);
        sy = src.y1 + ((src.y2 - src.y1)/2);

        ex = dest.x1 + ((dest.x2 - dest.x1)/2);
        ey = dest.y1 + ((dest.y2 - dest.y1)/2);

        this->line(sx, sy, ex, ey);
    }

    int srcroom2[]  = { 0, 3, 6,  9, 1, 4, 7, 10 };
    int destroom2[] = { 1, 4, 7, 10, 2, 5, 8, 11 };

    for(int q=0;q<9;q++) {
        int sx, sy, ex, ey;
        
        src = roomlist.at(srcroom2[q]);
        dest = roomlist.at(destroom2[q]);

        sx = src.x1 + ((src.x2 - src.x1)/2);
        sy = src.y1 + ((src.y2 - src.y1)/2);

        ex = dest.x1 + ((dest.x2 - dest.x1)/2);
        ey = dest.y1 + ((dest.y2 - dest.y1)/2);

        this->line(sx, sy, ex, ey);
    }

    this->build_tcodmap();
    lights_on = false;
}

void Area::generate()
{
    int type;

    type = ri(1,3);

    switch(type) {
        case 1:
            this->generate_type_2();
            break;
        case 2:
            this->fill();
            this->generate_drunken_walk();
            break;
        case 3:
            this->generate_type_2();
            this->generate_drunken_walk();
    };

    this->frame();
    this->build_tcodmap();
    lights_on = false;
}

void Area::place_furniture()
{
}

void Area::frame()
{
    horizontal_line(1);
    horizontal_line(AREA_MAX_Y - 2);
    vertical_line(1);
    vertical_line(AREA_MAX_X - 2);
}

void Area::fill()
{
    for(int x=0;x<AREA_MAX_X;++x)
        for(int y=0;y<AREA_MAX_Y;++y)
            this->cell[x][y].set_wall();
}

void Area::make_room(int x1, int y1, int x2, int y2)
{
    for(int i=x1; i<=x2; ++i)
        for(int j=y1; j<=y2; ++j)
            this->cell[i][j].set_floor();

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

/*
 * Adapted from the article on Bresenham's line algorithm on en.wikipedia.org
 */
void Area::line(int x1, int y1, int x2, int y2)
{
    bool steep = (abs(y2 - y1) > abs(x2 - x1));
    if(steep) {
        swap(x1, y1);
        swap(x2, y2);
    }

    if(x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }

    int dx = x2 - x1;
    int dy = abs(y2 - y1);

    float error = dx / 2;
    int ystep;
    int y = y1;

    if(y1 < y2)
        ystep = 1;
    else
        ystep = -1;

    for(int x = x1; x <= x2; ++x) {
        if(steep)
            this->cell[y][x].set_floor();
        else
            this->cell[x][y].set_floor();

        error -= dy;
        if(error < 0) {
            y += ystep;
            error += dx;
        }
    }
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
            display->putmap(x, y, player->area->cell[x][y].c, player->area->cell[x][y].fg, player->area->cell[x][y].bg);
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

coord_t World::get_random_walkable_cell(int id)
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

coord_t World::get_random_floor_cell(int id)
{
    coord_t co;

again:
    co.x = ri(1, AREA_MAX_X-2);
    co.y = ri(1, AREA_MAX_Y-2);
    while(area[id].cell[co.x][co.y].get_type() != floor) {
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
    //actor->area_id = actor->area->get_id();
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
    
    for(i=1; i<=40; i++) {
        world->area[i].make_stairs_down();
        world->area[i].make_stairs_up();
    }

    world->area[0].make_stairs_down();
    world->area[41].make_stairs_up();
}




// vim: fdm=syntax guifont=Terminus\ 8
