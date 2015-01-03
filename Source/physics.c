#include "bitmap.h"
#include "kibble.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

unsigned int t; // Time tick to be updated ~25 times/sec
unsigned int camera;
unsigned int previouscamera;
extern unsigned char running;
extern unsigned char menu;

void init_physics(void)
{
    previouscamera = 0;
    camera = 0;
    t = 0;
}

// Note: do this before drawing the lander, but after applying physics
void collisions(void)
{
    unsigned int i;
    unsigned char max;
    unsigned char same;
    unsigned int edge = lander.x+LANDER_WIDTH;
    unsigned char feet = lander.y+LANDER_HEIGHT;

    same = true; // Look for a flat landing surface underfoot
    max = moon[lander.x];
    for (i = lander.x+1; i < edge; i++) {
        if (moon[i] != moon[i-1]) {
            same = false;
        }
        if (moon[i] < max) {
            max = moon[i];
        }
    }

    if (feet >= max) { // Lander touched down on something
        lander.y = max-LANDER_HEIGHT;
        if (lander.speed.y > IMPACT_MAX) {
            lander.bitmap = img_downcrash;
            stop_lander(CRASHED);
            return;
        }
        if (same) { // Found a flat spot! Make sure we didn't land too hard
            if (lander.x >= landingpad && edge <= landingpad+LANDINGPAD_WIDTH) {
                stop_lander(LANDED); // Victory!
            }
            return;
        } else { // Uneven ground, 
            lander.speed.y = -1*(lander.speed.y>>1);
            if (lander.speed.y == 0 && lander.fuel == 0) {
                stop_lander(STRANDED);
            }
        }
    }

    // Check if we are about to run into the side of a hill
    if (lander.speed.x > 0) {
        feet = lander.previous.y+LANDER_HEIGHT;
        edge = lander.previous.x+LANDER_WIDTH;
        for (i = edge; i < edge+lander.speed.x; i++) {
            if (i > MOON_WIDTH-1) {
                break;
            }
            if (moon[i] < feet) {
                if (lander.speed.x > IMPACT_MAX) {
                    lander.bitmap = img_rightcrash;
                    stop_lander(CRASHED);
                    lander.x = i-LANDER_WIDTH;
                    lander.y = lander.previous.y;
                    camera = previouscamera;
                    return;
                } else {
                    lander.x = lander.previous.x;
                    lander.acceleration.x = 0;
                    lander.speed.x = -1*(lander.speed.x>>1);
                    if (lander.speed.x > -1) {
                        lander.speed.x = -1; // Bounce!
                    }
                }
            }
        }
    } else if (lander.speed.x < 0) { // Same but from the right
        feet = lander.previous.y+LANDER_HEIGHT;
        edge = lander.previous.x;
        for (i = edge; i > edge+lander.speed.x; i--) {
            if (i > edge) {
                break;
            }
            if (moon[i] < feet) {
                if (lander.speed.x < -IMPACT_MAX) {
                    lander.bitmap = img_leftcrash;
                    stop_lander(CRASHED);
                    lander.x = i;
                    lander.y = lander.previous.y;
                    camera = previouscamera;
                    return;
                } else {
                    lander.x = edge;
                    lander.acceleration.x = 0;
                    lander.speed.x = -1*(lander.speed.x>>1);
                    if (lander.speed.x < 1) {
                        lander.speed.x = 1; // Bounce!
                    }
                }
            }
        }
    }
}

void apply_input(void)
{
    union keyrow_6 k6;
    union keyrow_0 k0;
    static union keyrow_0 prev_k0 = {.raw = 0};

    if (!lander.freedom.stopped && lander.fuel > 0) {
        scan_row_6(&k6);
    } else {
        k6.raw = 0;
    }
    scan_row_0(&k0);

    if (k6.keys.K_RIGHT) {
        if (lander.acceleration.x < ACCEL_MAX && lander.x < MOON_WIDTH-LANDER_WIDTH-1) {
            lander.acceleration.x++;
        }
        lander.thrust.hp_firing = true;
        if ((t&0x03) == 0) { // Cycle ~6 times/sec
            lander.thrust.hp_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        lander.thrust.hp_firing = false;
        lander.thrust.hp_stage = 0;
    }
    if (k6.keys.K_LEFT) {
        if (lander.acceleration.x > -ACCEL_MAX && lander.x > 0) {
            lander.acceleration.x--;
        }
        lander.thrust.hn_firing = true;
        if ((t&0x03) == 0) {
            lander.thrust.hn_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        lander.thrust.hn_firing = false;
        lander.thrust.hn_stage = 0;
    }

    // If both keys are pressed, cancel each acceleration
    // (without cancelling fuel consumption)
    lander.acceleration.x *= (k6.keys.K_LEFT ^ k6.keys.K_RIGHT);

    if (k6.keys.K_UP) {
        if (lander.acceleration.y > -ACCEL_MAX && lander.y > 0) {
            lander.acceleration.y--;
        }
        lander.thrust.vp_firing = true;
        if ((t&0x03) == 0) {
            lander.thrust.vp_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        if (!lander.freedom.stuck.crashed) {
            lander.acceleration.y = GRAVITY;
        }
        lander.thrust.vp_firing = false;
        lander.thrust.vp_stage = 0;
    }

    if (k0.keys.K_2ND && !prev_k0.keys.K_2ND && lander.food > 0) {
        lander.food--;
        create_kibble(lander.food, lander.x, lander.y, lander.speed.x, lander.speed.y);
    }

    if (k0.keys.K_EXIT) {
        running = false;
        menu = false;
    }
    if (k0.keys.K_F1) {
        running = false;
        menu = true;
    }

    prev_k0.raw = k0.raw;
}
