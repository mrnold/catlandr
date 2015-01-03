#include "bitmap.h"
#include "kitty.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

extern unsigned char running;
extern unsigned char menu;

struct lander_t lander;

void init_lander(void)
{
    lander.y = 0;
    lander.x = SCREEN_WIDTH/2;
    lander.speed.x = 0;
    lander.speed.y = 0;
    lander.acceleration.x = 0;
    lander.acceleration.y = GRAVITY;
    lander.bitmap = img_lander;
    lander.thrust.hp_stage = 0;
    lander.thrust.hp_firing = 0;
    lander.thrust.hn_stage = 0;
    lander.thrust.hn_firing = 0;
    lander.thrust.vp_stage = 0;
    lander.thrust.vp_firing = 0;
    lander.stranded = false;
    lander.crashed = false;
    lander.landed = false;
    lander.fuel = 255;
    lander.food = 31;
}

void stop_lander(unsigned char state)
{
    switch (state) {
        case STRANDED:
            lander.stranded = true;
            lander.crashed = false;
            lander.landed = false;
            break;
        case CRASHED:
            lander.stranded = false;
            lander.crashed = true;
            lander.landed = false;
            break;
        case LANDED:
            lander.stranded = false;
            lander.crashed = false;
            lander.landed = true;
            break;
    }
    lander.thrust.hp_firing = false;
    lander.thrust.hn_firing = false;
    lander.thrust.vp_firing = false;
    running = false;
    menu = true;
}

void move_lander(void)
{
    unsigned int landerlimit;
    unsigned char feet;
    int scratch;
    char rand;

    lander.previous.x = lander.x;
    lander.previous.y = lander.y;
    previouscamera = camera;

    if (lander.crashed) {
        kitty.bitmap = &cat_sittingleft;
        kitty.batting = false;
        return;
    }

    if (kitty.x+KITTY_WIDTH >= lander.x && kitty.x <= lander.x+LANDER_WIDTH) {
        if (kitty.y >= lander.y && kitty.y <= lander.y+LANDER_HEIGHT) { // Hit!
            if (kitty.speed.x == 0 && kitty.speed.y == 0) {
                lander.speed.y = -SPEED_MAX; // Bat lander upwards
                rand = (char)((random8()<<1)&0x82);
                if (rand > 0) {
                    lander.speed.x = SPEED_MAX;
                } else if (rand < 0) {
                    lander.speed.x = -SPEED_MAX;
                }
                kitty.batting = true;
            } else {
                lander.speed.x += kitty.speed.x;
                lander.speed.y += kitty.speed.y;
            }
        }
    }

    lander.speed.x += lander.acceleration.x/2;
    if (lander.speed.x > 0) {
        if (lander.speed.x > SPEED_MAX) {
            lander.speed.x = SPEED_MAX;
        } else {
            lander.speed.x--;
        }
    } else if (lander.speed.x < 0) {
        if (lander.speed.x < -SPEED_MAX) {
            lander.speed.x = -SPEED_MAX;
        } else {
            lander.speed.x++;
        }
    }

    feet = lander.y+LANDER_HEIGHT;
    landerlimit = MOON_WIDTH-LANDER_WIDTH;
    scratch = (int)lander.x + (int)lander.speed.x;
    if (scratch >= (int)landerlimit) {
        lander.x = landerlimit;
        lander.speed.x = 0;
    } else if (scratch <= 0) {
        lander.speed.x = 0;
        lander.x = 0;
    } else {
        lander.x = (unsigned int)scratch;
    }

    scratch = lander.x-camera;
    if (scratch < LANDER_WIDTH) {
        scratch = lander.x-LANDER_WIDTH;
        if (scratch < 0) {
            camera = 0;
        } else {
            camera = scratch;
        }
    }

    scratch = camera+SCREEN_WIDTH-lander.x;
    if (scratch < 2*LANDER_WIDTH) {
        scratch = lander.x+2*LANDER_WIDTH-SCREEN_WIDTH;
        if (scratch+SCREEN_WIDTH > MOON_WIDTH) {
            camera = MOON_WIDTH-SCREEN_WIDTH;
        } else {
            camera = scratch;
        }
    }


    lander.speed.y += lander.acceleration.y/2;
    if (lander.speed.y > SPEED_MAX) {
        lander.speed.y = SPEED_MAX;
    } else if (lander.speed.y < -SPEED_MAX) {
        lander.speed.y = -SPEED_MAX;
    }

    if (lander.speed.y > 0) { //Screen down
        landerlimit = SCREEN_HEIGHT-LANDER_HEIGHT;
        lander.y += lander.speed.y/4;
        if (lander.y > landerlimit) {
            lander.y = landerlimit;
            lander.speed.y = 0;
            lander.acceleration.y = 0;
        }
    } else if (lander.speed.y < 0) { //Screen up
        scratch = lander.y + lander.speed.y/4;
        if (scratch < 0) {
            lander.y = 0;
            lander.speed.y = 0;
            if (lander.acceleration.y < -GRAVITY) { // "Hang time"
                lander.acceleration.y = -GRAVITY;
            }
        } else {
            lander.y = scratch;
        }
    }
}

void draw_lander(void)
{
    draw_static_sprite_noclip(lander.bitmap, lander.x, lander.y);

    if (lander.thrust.hp_firing) { // Draw left thruster
        draw_live_sprite(img_thrustleft, lander.thrust.hp_stage, lander.x-8, lander.y);
    }

    if (lander.thrust.hn_firing) { // Draw right thruster
        draw_live_sprite(img_thrustright, lander.thrust.hn_stage, lander.x+8, lander.y);
    }

    if (lander.thrust.vp_firing) {
        draw_live_sprite(img_thrustdown, lander.thrust.vp_stage, lander.x, lander.y+8);
    }
}
