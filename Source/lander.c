#include "bitmap.h"
#include "physics.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "ti86.h"

extern unsigned char gamestate;

struct lander_t lander;

void init_lander(void)
{
    init_object(&lander.phys);
    lander.phys.position.x = SCREEN_WIDTH/2;
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
    lander.phys.acceleration.x = 0;
    lander.phys.acceleration.y = 0;
    lander.phys.velocity.x = 0;
    lander.phys.velocity.y = 0;
}

void move_lander(void)
{
    char rand;
    unsigned int i;
    unsigned char max;
    unsigned char same;
    unsigned char event;

    event = move_object(&lander.phys);

    same = true; // Look for a flat landing surface underfoot
    max = moon[lander.phys.position.x+1];
    for (i = lander.phys.position.x+1; i < lander.phys.position.x+8; i++) {
        if (moon[i] != moon[i-1]) {
            same = false;
        }
        if (moon[i] < max) {
            max = moon[i];
        }
    }

    if (lander.freedom.stuck.crashed) {
        kitty.bitmap = &cat_sittingleft;
        kitty.batting = false;
        return;
    }

    if (kitty.x+KITTY_WIDTH >= lander.phys.position.x && kitty.x <= lander.phys.position.x+LANDER_WIDTH) {
        if (kitty.y >= lander.phys.position.y && kitty.y <= lander.phys.position.y+LANDER_HEIGHT) { // Hit!
            if (kitty.speed.x == 0 && kitty.speed.y == 0) {
                lander.phys.velocity.y = -SPEED_MAX; // Bat lander upwards
                rand = (char)((random8()<<1)&0x82);
                if (rand > 0) {
                    lander.phys.velocity.x = SPEED_MAX;
                } else if (rand < 0) {
                    lander.phys.velocity.x = -SPEED_MAX;
                }
                kitty.batting = true;
            } else {
                lander.phys.velocity.x += kitty.speed.x;
                lander.phys.velocity.y += kitty.speed.y;
            }
        }
    } else {
        kitty.batting = false;
    }

    switch (event) {
    case OBJ_LOW_IMPACT_DOWN:
        if (same) { // Victory!
            stop_lander(LANDED);
        } else { // Uneven ground
            if (lander.phys.velocity.y == 0 && lander.fuel == 0) {
                stop_lander(STRANDED);
            }
        }
        break;
    case OBJ_HIGH_IMPACT_DOWN:
        lander.bitmap = img_downcrash;
        stop_lander(CRASHED);
        break;
    case OBJ_LOW_IMPACT_RIGHT: // Bounce left
        lander.phys.acceleration.x = 0;
        lander.phys.velocity.x = -1*(lander.phys.velocity.x>>1);
        if (lander.phys.velocity.x == 0) {
            lander.phys.velocity.x = -1;
        }
        break;
    case OBJ_HIGH_IMPACT_RIGHT:
        lander.bitmap = img_rightcrash;
        stop_lander(CRASHED);
        break;
    case OBJ_LOW_IMPACT_LEFT: // Bounce right
        lander.phys.acceleration.x = 0;
        lander.phys.velocity.x = -1*(lander.phys.velocity.x>>1);
        if (lander.phys.velocity.x == 0) {
            lander.phys.velocity.x = 0;
        }
        break;
    case OBJ_HIGH_IMPACT_LEFT:
        lander.bitmap = img_leftcrash;
        stop_lander(CRASHED);
        break;
    }
}

void draw_lander(void)
{
    draw_static_sprite_noclip(lander.bitmap, lander.phys.position.x, lander.phys.position.y);

    if (lander.thrust.hp_firing) { // Draw left thruster
        draw_live_sprite(img_thrustleft, lander.thrust.hp_stage, lander.phys.position.x, lander.phys.position.y, -8, OR);
    }

    if (lander.thrust.hn_firing) { // Draw right thruster
        draw_live_sprite(img_thrustright, lander.thrust.hn_stage, lander.phys.position.x, lander.phys.position.y, 8, OR);
    }

    if (lander.thrust.vp_firing) {
        draw_live_sprite(img_thrustdown, lander.thrust.vp_stage, lander.phys.position.x, lander.phys.position.y+8, 0, OR);
    }
}
