#include "bitmap.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

extern unsigned char gamestate;

__at (0xa700) struct lander_t lander;

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
    lander.freedom.stopped = false;
    lander.fuel = 255;
    lander.food = KIBBLE_MAX;
}

void stop_lander(unsigned char state)
{
    lander.freedom.stopped = false;
    switch (state) {
        case STRANDED:
            lander.freedom.stuck.stranded = true;
            gamestate = DONE_STRANDED;
            break;
        case CRASHED:
            lander.freedom.stuck.crashed = true;
            gamestate = DONE_CRASHED;
            break;
        case LANDED:
            lander.freedom.stuck.landed = true;
            gamestate = DONE_LANDED;
            break;
    }
    lander.thrust.hp_firing = false;
    lander.thrust.hn_firing = false;
    lander.thrust.vp_firing = false;
    lander.acceleration.x = 0;
    lander.acceleration.y = 0;
    lander.speed.x = 0;
    lander.speed.y = 0;
}

void move_lander(void)
{
    int scratch;
    char rand;

    lander.previous.x = lander.x;
    lander.previous.y = lander.y;

    if (lander.freedom.stuck.crashed) {
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
    } else {
        kitty.batting = false;
    }

    lander.speed.x += (lander.acceleration.x>>1);
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

    scratch = (int)lander.x + (int)lander.speed.x;
    if (scratch >= (MOON_WIDTH-LANDER_WIDTH)) {
        lander.x = (MOON_WIDTH-LANDER_WIDTH);
        lander.speed.x = 0;
    } else if (scratch <= 0) {
        lander.speed.x = 0;
        lander.x = 0;
    } else {
        lander.x = (unsigned int)scratch;
    }

    lander.speed.y += (lander.acceleration.y>>1);
    if (lander.speed.y > SPEED_MAX) {
        lander.speed.y = SPEED_MAX;
    } else if (lander.speed.y < -SPEED_MAX) {
        lander.speed.y = -SPEED_MAX;
    }

    if (lander.speed.y > 0) { //Screen down
        lander.y += (lander.speed.y>>2);
        if (lander.y > (SCREEN_HEIGHT-LANDER_HEIGHT)) {
            lander.y = (SCREEN_HEIGHT-LANDER_HEIGHT);
            lander.speed.y = 0;
            lander.acceleration.y = 0;
        }
    } else if (lander.speed.y < 0) { //Screen up
        scratch = lander.y + (lander.speed.y>>2);
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
        draw_live_sprite(img_thrustleft, lander.thrust.hp_stage, lander.x, lander.y, -8, OR);
    }

    if (lander.thrust.hn_firing) { // Draw right thruster
        draw_live_sprite(img_thrustright, lander.thrust.hn_stage, lander.x, lander.y, 8, OR);
    }

    if (lander.thrust.vp_firing) {
        draw_live_sprite(img_thrustdown, lander.thrust.vp_stage, lander.x, lander.y+8, 0, OR);
    }
}
