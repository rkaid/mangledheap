/*
 * Generic actor class definition
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 */

#include <string>

using namespace std;
#include "common.h"
#include "actor.h"
#include "player.h"
#include "display.h"
#include "game.h"
#include "world.h"

extern World *world;
extern Display *display;
extern Player *player;
extern Game g;

const char *special_name[] = {
    "(none)",
};

string bodypart_string[] = {
    "head",
    "torso",
    "left arm",
    "right arm",
    "left foot",
    "right foot",
    "left hand",
    "right hand",
    "left leg",
    "right leg"
};


BodyPart::BodyPart()
{
    attached = false;
};

BodyPart::BodyPart(bodypart_e t)
{
    type = t;
    attached = false;
};

Stat::Stat()
{
    cur = 0;
    max = 0;
    base = 0;
}

Stat::Stat(int c, int m)
{
    cur = c;
    base = c;
    max = m;
}

Actor::Actor()
{
    role = role_unknown;
    c = ' ';
    co.x = co.y = 0;
    alive = true;
    enemy = NULL;
    moved_ = false;
    str = new Stat(10,10);
    health = new Stat(100,100);
}

bool Actor::is_male()
{ 
    if(male)
        return true;
    else
        return false;
};

void Actor::kill()
{
    //display->message("%s dies at %d,%d!", name, this->co.x, this->co.y);
    if(!this->is_player()) {
        this->alive = false;
    } else {
        player->die();
    }
}

void Actor::setxy(int x, int y)
{
    co.x = x;
    co.y = y;
}

void Actor::setxy(coord_t newco)
{
    co = newco;
}

int Actor::getx()
{
    return co.x;
}

int Actor::gety()
{
    return co.y;
}

void Actor::setprevxy(int x, int y)
{
    prev.x = x;
    prev.y = y;
}

void Actor::setprevxy(coord_t newco)
{
    prev = newco;
}

void Actor::setchar(char newc)
{
    c = newc;
}

void Actor::setname(const char *name)
{
    strcpy(this->name, name);
}

void Actor::setcolors(TCODColor fg, TCODColor bg)
{
    this->fg = fg;
    this->bg = bg;
}

char *Actor::getname()
{
    return this->name;
}

coord_t Actor::getxy()
{
    return co;
}

void Actor::draw()
{
    if(this->area->cell_is_visible(co.x, co.y)) {
        display->putmap(this->co.x, this->co.y, this->c, this->fg, this->bg);
        display->touch();
    }
}

void Actor::draw(TCODColor fg, TCODColor bg)
{
    if(world->a->cell_is_visible(co.x, co.y)) {
        display->putmap(this->co.x, this->co.y, this->c, fg, bg);
        display->touch();
    }
}

void Actor::drawcorpse()
{
    display->putmap(this->co.x, this->co.y, 0xB6);
    display->touch();
}

void Actor::move(int dx, int dy)
{
    if(this->area->cell[this->co.x + dx][this->co.y + dy].inhabitant) {
        if(this == player) {
            display->message("%s is standing in your way!", this->area->cell[this->co.x + dx][this->co.y + dy].inhabitant->getname());
            display->message("Do you want to attack %s [y/n]?", this->area->cell[this->co.x + dx][this->co.y + dy].inhabitant->getname());
            display->touch();
            display->print_messages();
            display->touch();
            if(display->askyn()) {
                player->attack(this->area->cell[this->co.x + dx][this->co.y + dy].inhabitant);
                player->moved(true);
            } else {
                player->moved(false);
            }
        } else {
            if(this->enemy) {
                if(this->enemy->area == this->area) {
                    if(this->enemy->getx() == this->co.x + dx && this->enemy->gety() == this->co.y + dy) {
                        attack(enemy);
                        return;
                    }
                }
            }
        }

        return;
    }

    if(world->is_closed_door(this->area, this->co.x + dx, this->co.y + dy)) {
        world->open_door(this->area, this->co.x + dx, this->co.y + dy);
        display->touch();
        return;
    }

    if(g.wizmode && this == player) {
        prev = co;
        co.x += dx;
        co.y += dy;
        world->clear_inhabitant(this->area, prev);
        world->set_inhabitant(this);
        player->moved(true);
        display->touch();
        return;
    }

    if(this->area->is_walkable(this->co.x + dx, this->co.y + dy)) {
        prev = co;
        co.x += dx;
        co.y += dy;
        world->clear_inhabitant(this->area, prev);
        world->set_inhabitant(this);
        display->touch();
        if(this == player)
            player->moved(true);
    }
}

void Actor::move_left()
{
    this->move(-1, 0);
}

void Actor::move_right()
{
    this->move(1, 0);
}

void Actor::move_down()
{
    this->move(0, 1);
}

void Actor::move_up()
{
    this->move(0, -1);
}

void Actor::move_nw()
{
    this->move(-1, -1);
}

void Actor::move_ne()
{
    this->move(1, -1);
}

void Actor::move_sw()
{
    this->move(-1, 1);
}

void Actor::move_se()
{
    this->move(1, 1);
}

/*
int Actor::getstat(enum_stat which)
{
    return this->stat[(int)which];
}

void Actor::setstat(enum_stat which, int what)
{
    this->stat[which] = what;
}

void Actor::decstat(enum_stat which, int amount)
{
    if(this->stat[which] > 0)
        this->stat[which] -= amount;
}

void Actor::incstat(enum_stat which, int amount)
{
    this->stat[which] += amount;
    *if(which == s) {
        if(this->stat[which] > 20)
            this->stat[which] = 20;
    }
}
*/

void Actor::use_stairs()
{
}

void Actor::set_in_combat()
{
}

bool Actor::pass_roll(Stat *stat)
{
    int x;

    x = dice(1,21,0);
    if(x <= stat->get())
        return true;
    else
        return false;
}

void Actor::attack_physical(Actor *target)
{
    int tohit = target->str->get();
    int d = dice(1, 20, 0);
    if(d >= tohit) {
        int damage = dice(1, this->str->get(), ability_modifier(this->str->get()));
        if(damage <= 0)
            damage = 1;
        if(this != player) {
            if(!this->can_see(target)) {
            //if(!world->a->tcodmap->isInFov(target->getx(), target->gety())) {
            } else {
                if(target != player)
                    display->messagec(COLOR_FEAR, "You see %s attacking %s!", this->getname(), target->getname());
                if(target == player)
                    display->messagec(COLOR_FEAR, "%s %s you in the !", this->getname()/*, fiftyfifty() ? "hits" : "kicks", bodypart_string[ri(0,8)].c_str()*/);
            }
        } else {    // this == player
            display->messagec(COLOR_GOOD, "You attack %s, causing %d amounts of damage!", target->getname(), damage);
        }
        //display->message("%s HIT %s! %d damage.", this->name, target->getname(), damage);
        target->health -= damage;
        if(target->health->get() <= 0) {
            target->kill();
            this->enemy = NULL;

        }
    }
}

void Actor::attack(Actor *target, attack_type type)
{
    target->enemy = this;
    if(this == player) {
        target->set_in_combat();
    }

    switch(type) {
        case body:
            attack_physical(target);
            break;
        default:
            break;
    }
}

bool Actor::can_see(Actor *target)
{
    bool result;

    result = this->area->tcodmap->isInFov(target->getx(), target->gety());
    if(result && this == player)
        this->area->cell[target->getx()][target->gety()].seen();

    return result;
}

bool Actor::can_see(int x, int y)
{
    bool result;

    result = this->area->tcodmap->isInFov(x, y);
    if(result && this == player)
        this->area->cell[x][y].seen();

    return result;
}

bool Actor::is_next_to(Actor *target)
{
    int dx,dy;
    for(dx=-1;dx<=1;++dx) {
        for(dy=-1;dy<=1;++dy) {
            if((this->getx() + dx == target->getx()) && (this->gety() + dy == target->gety()))
                return true;
        }
    }

    return false;
}

int  Actor::add_special_attack(special_type t)
{
    vector<SpecialAttack>::iterator it;
    SpecialAttack *s;

    for(it = this->special.begin(); it != this->special.end(); ++it) {
        if(it->type == t) {
            it->level++;
            return SPECIAL_ADD_INCREASE;
        }
    }

    s = new SpecialAttack(t);
    this->special.push_back(*s);
    delete s;
    return SPECIAL_ADD_SUCCESS;
}

SpecialAttack::SpecialAttack()
{
    type = special_none;
    level = 0;
    name[0] = '\0';
}

SpecialAttack::SpecialAttack(special_type t)
{
    type = t;
    level = 1;
    strcpy(name, special_name[(int) t]);
}
// vim: fdm=syntax guifont=Terminus\ 8
