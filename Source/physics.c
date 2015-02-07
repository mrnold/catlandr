#include "bitmap.h"
#include "game.h"
#include "kibble.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86display.h"
#include "ti86.h"

unsigned int t; // Time tick to be updated ~25 times/sec

void init_physics(void)
{
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
            lander.stage = LANDER_DOWNCRASH;
            stop_lander(CRASHED);
            return;
        }
        if (same) { // Found a flat spot! Make sure we didn't land too hard
            if (lander.x >= landingpad && edge <= landingpad+LANDINGPAD_WIDTH) {
                stop_lander(LANDED); // Victory!
            }
            return;
        } else { // Uneven ground, 
            lander.speed.y = 0;
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
                    lander.stage = LANDER_RIGHTCRASH;
                    stop_lander(CRASHED);
                    lander.x = i-LANDER_WIDTH;
                    lander.y = lander.previous.y;
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
                    lander.stage = LANDER_LEFTCRASH;
                    stop_lander(CRASHED);
                    lander.x = i;
                    lander.y = lander.previous.y;
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
