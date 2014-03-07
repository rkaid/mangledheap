/*
 * The Items
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */


#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

#include "common.h"

#define IF_WIELDABLE  0x00000001
#define IF_POCKETABLE 0x00000002
#define IF_WEARABLE   0x00000004

enum item_type {
    it_nothing = 0,
    it_weapon,
    it_clothing,
    it_tool,
    it_key,
};

/*
 * struct for how we define items 
 */
struct item_definition {
    string name;
    char c;
    item_type type;
    long flags;
    int value;
    int chance;
};

class Item {
    private:
        string name;
        item_type type;
        long flags;
        int value;       // max damage for weapons, protection for armor, something else for other
    public:
        Item(struct item_definition def);
        Item(Item *item);
        Item();
        ~Item();
        //string get_name() { return name; };
        char *get_name();

        coord_t co;
        int chance;      // spawn chance
        char c;
        TCODColor fg, bg;
};

#endif



// vim: fdm=syntax guifont=Terminus\ 8
