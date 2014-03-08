/*
 * Generic actor class definition
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _ACTOR_H
#define _ACTOR_H

#include "common.h"

enum e_role {
    role_unknown = 0,
    role_player,
    role_npc,
    role_enemy,
};

enum attack_type {
    body = 0,
    mind,
    soul,
    other
};

enum enum_stat {
    sHealth = 0,
    sMangle,
    sHeap,
    sStr
};

enum special_type {
    special_none = 0,
    special_1,
    special_2,
    special_3,
    special_4,
    special_5,
    special_6,
    special_7,
    special_8,
    special_9,
};

extern const char *special_name[];

#define AI_RANDOM 1
#define AI_PATH 2

class Actor;

typedef void (Actor::*aifn)();

#include "world.h"

#define SPECIAL_ADD_FAIL     0
#define SPECIAL_ADD_SUCCESS  1
#define SPECIAL_ADD_INCREASE 2

class SpecialAttack {
    public:
        SpecialAttack();
        SpecialAttack(special_type t);
        ~SpecialAttack() {
        };

        special_type type;
        char name[20];
        int level;
};

class Stat {
    private:
        int cur;
        int max;
        int base;
    public:
        Stat();
        Stat(int c, int m);
        ~Stat() { };

        void set(int x) { cur = x; };
        int  get() { return cur; };
        void setmax(int m) { max = m; };
        
        Stat& operator+=(const int x) {
            if((cur+x) <= max)
                cur += x;
            
            return *this;
        };
        
        Stat& operator++(const int x) {
            (*this)+=1;
            return *this;
        };

        Stat& operator-=(const int x) {
            if((cur-x) > 0)
                cur -= x;
            
            return *this;
        };
        
        Stat& operator--(const int x) {
            (*this)-=1;
            return *this;
        };
};

enum bodypart_e {
    head = 0,
    torso,
    left_arm,
    right_arm,
    left_foot,
    right_foot,
    left_hand,
    right_hand,
    left_leg,
    right_leg
};

class BodyPart {
    public:
        BodyPart();
        BodyPart(bodypart_e t);
        ~BodyPart() { };
        string name;
        bodypart_e type;
        bool attached;
};

class Actor {
    public:
        Actor();
        virtual ~Actor() {  };
        virtual bool is_player() = 0;
        virtual void use_stairs();
        virtual void set_in_combat();

        bool is_alive() { return alive; };
        bool is_male();
        void kill();
        void draw();
        void draw(TCODColor fg, TCODColor bg);
        void drawcorpse();
        void setxy(int x, int y);
        void setxy(coord_t newco);
        int  getx();
        int  gety();
        coord_t getxy();
        void setprevxy(int x, int y);
        void setprevxy(coord_t newco);
        void setchar(char newc);
        void setname(const char *name);
        void setcolors(TCODColor fg, TCODColor bg);
        void setgender(int i) { male = i; };
        char *getname();

        /*void decstat(enum_stat which, int amount = 1);
        void incstat(enum_stat which, int amount = 1);
        void setstat(enum_stat which, int what);
        int  getstat(enum_stat which);*/

        void setfovradius(int amount) { fovradius = amount; };
        int  getfovradius() { return fovradius; };

        bool can_see(Actor *target);
        bool can_see(int x, int y);
        bool is_next_to(Actor *target);

        void attack(Actor *target, attack_type type = body);
        void attack_physical(Actor *target);

        bool pass_roll(Stat *stat);
        int  add_special_attack(special_type t); 

        // Movement
        void move(int dx, int dy);
        void move_left();
        void move_right();
        void move_up();
        void move_down();
        void move_nw();
        void move_ne();
        void move_sw();
        void move_se();
        void moved(bool b = true) { moved_ = b; };
        bool has_moved() { return moved_; };

        int area_id;
        Actor *enemy;              // public enemy haha!
        Area *area;                // in which area is this actor?
        bool alive;
        vector <SpecialAttack> special;
        vector <BodyPart> bparts;

        Stat *str, *health;
    protected:
    private:
        bool male;
        bool moved_;
        char c;
        char name[64];
        coord_t co;
        coord_t prev;
        e_role role;
        TCODColor fg, bg;
        int fovradius;

        // Stats
        int stat[6];
};


#endif
// vim: fdm=syntax guifont=Terminus\ 8
