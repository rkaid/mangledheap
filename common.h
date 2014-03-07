/*
 * Common stuff
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

#ifndef _COMMON_H
#define _COMMON_H

#include "display.h"
#include "debug.h"

#define d(x,y) dice(x,y,0)
#define CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))

#ifdef DEBUG_ON
#define DBG(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(M, ...)
#endif

extern Display *display;

typedef struct {
        int x;
        int y;
} coord_t;

enum direction {
        none = 0,
        north,
        south,
        west,
        east,
        ne,
        nw,
        se,
        sw
};

class Clock {
    private:
        int second, minute, hour;
    public:
        void settime(int h, int m, int s) { second=s; minute=m; hour=h; };
        void inc_hour(int i) {
            hour += i;
            if(hour >= 24) {
                hour -= 24;
            }
        };
        void inc_minute(int i) {
            minute += i;
            while(minute >= 60) {
                inc_hour(1);
                minute -= 60;
            }
        };
        void inc_second(int i) {
            second += i;
            while(second >= 60) {
                inc_minute(1);
                second -= 60;
            }
        };
        Clock& operator+=(const int x) {
            (*this).inc_second(x);
            return *this;
        };
        void inc(int i) {
            inc_second(i);
        }
        int get_hour() { return hour; };
        int get_minute() { return minute; };
};

// Function prototypes
bool fiftyfifty();
int dice(int num, int sides, signed int modifier);
int ri(int a, int b);
signed int ability_modifier(int ab);
bool one_in(int chance);

#endif

// vim: fdm=syntax guifont=Terminus\ 8
