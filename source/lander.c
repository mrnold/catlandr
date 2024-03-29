#include "bitmap.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "calc/display.h"
#include "calc/ram.h"
#include "calc/random.h"

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
    lander.hp_firing = false;
    lander.hn_firing = false;
    lander.vp_firing = false;
    lander.acceleration.x = 0;
    lander.acceleration.y = 0;
    lander.speed.x = 0;
    lander.speed.y = 0;
}

void move_lander(void)
{
    int scratch;
    signed char rand;

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
                rand = (signed char)((random8()<<1)&0x82);
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
    draw_live_sprite(*(lander.bitmap), lander.stage, lander.x, lander.y, 0, OR);

    if (lander.hp_firing) { // Draw left thruster
        draw_live_sprite(img_thrustleft, lander.hp_stage, lander.x, lander.y, -8, OR);
    }

    if (lander.hn_firing) { // Draw right thruster
        draw_live_sprite(img_thrustright, lander.hn_stage, lander.x, lander.y, 8, OR);
    }

    if (lander.vp_firing) {
        draw_live_sprite(img_thrustdown, lander.vp_stage, lander.x, lander.y+8, 0, OR);
    }
}
