#include "bitmap.h"
#include "event.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

unsigned int camera;
unsigned int previouscamera;

void init_physics(void)
{
    previouscamera = 0;
    camera = 0;
}

void physics(void)
{
    unsigned int landerlimit;
    unsigned char feet;
    int scratch;

    lander.previous.x = lander.x;
    lander.previous.y = lander.y;
    previouscamera = camera;

    lander.momentum.x += lander.acceleration.x;
    if (lander.x > 0 && lander.x < MOON_WIDTH-LANDER_WIDTH) {
        if (lander.momentum.x > 0) {
            if (lander.momentum.x > 25) {
                lander.momentum.x = 25;
            } else {
                lander.momentum.x--;
            }
        } else if (lander.momentum.x < 0) {
            if (lander.momentum.x < -25) {
                lander.momentum.x = -25;
            } else {
                lander.momentum.x++;
            }
        }
    }

    feet = lander.y+LANDER_HEIGHT;
    landerlimit = MOON_WIDTH-LANDER_WIDTH;
    scratch = (int)lander.x + (int)lander.momentum.x;
    if (scratch >= (int)landerlimit) {
        lander.x = landerlimit;
        lander.acceleration.x = 0;
        lander.momentum.x =0;
    } else if (scratch <= 0) {
        lander.acceleration.x = 0;
        lander.momentum.x = 0;
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


    lander.momentum.y += lander.acceleration.y;
    if (lander.momentum.y > 25) {
        lander.momentum.y = 25;
    } else if (lander.momentum.y < -25) {
        lander.momentum.y = -25;
    }

    if (lander.momentum.y > 0) { //Screen down
        landerlimit = SCREEN_HEIGHT-LANDER_HEIGHT;
        lander.y += lander.momentum.y/4;
        if (lander.y > landerlimit) {
            lander.y = landerlimit;
            lander.momentum.y = 0;
            lander.acceleration.y = 0;
        }
    } else if (lander.momentum.y < 0) { //Screen up
        scratch = lander.y + lander.momentum.y/4;
        if (scratch < 0) {
            lander.y = 0;
            lander.momentum.y = 0;
            if (lander.acceleration.y < -1) {
                lander.acceleration.y = -1;
            }
        } else {
            lander.y = scratch;
        }
    }

    if (lander.momentum.y < 0) {
        lander.perched = false;
    }

}

// Note: do this before drawing the lander, but after applying physics
void collisions(void)
{
    unsigned int i;
    unsigned char max;
    unsigned char same;
    unsigned char feet = lander.y+LANDER_HEIGHT;
    unsigned char edge = lander.x+LANDER_WIDTH;

    if (moon[lander.x] <= feet) { // Screen up is negative
        same = true;
        for (i = edge-1; i > lander.x+1; i--) {
            if (moon[i] != moon[i-1]) {
                same = false;
                break;
            }
        }
        if (same) {
            if (lander.momentum.y > 10) {
                lander.bitmap = img_downcrash;
                lander.crashed = true;
            } else {
                if (lander.x > landingpad && lander.x < landingpad+LANDINGPAD_WIDTH) {
                    lander.landed = true;
                }
            }
            lander.y = moon[lander.x]-LANDER_HEIGHT;
            lander.acceleration.x = 0;
            lander.acceleration.y = 0;
            lander.momentum.x = 0;
            lander.momentum.y = 0;
            return;
        } else {
            if (lander.momentum.y > 10) {
                lander.bitmap = img_downcrash;
                lander.crashed = true;
                lander.y = moon[lander.x]-LANDER_HEIGHT;
                lander.acceleration.x = 0;
                lander.acceleration.y = 0;
                lander.momentum.x = 0;
                lander.momentum.y = 0;
                return;
            } else {
                lander.momentum.y = -1*lander.momentum.y/2;
            }
        }
    }

    if (lander.momentum.x > 0) {
        feet = lander.previous.y+LANDER_HEIGHT;
        edge = lander.previous.x+LANDER_WIDTH;
        for (i = edge; i < edge+lander.momentum.x; i++) {
            if (i > MOON_WIDTH-1) {
                break;
            }
            if (moon[i] < feet) {
                if (lander.momentum.x > 10) {
                    lander.bitmap = img_rightcrash;
                    lander.crashed = true;
                    lander.x = i-LANDER_WIDTH;
                    lander.y = lander.previous.y;
                    lander.acceleration.x = 0;
                    lander.acceleration.y = 0;
                    lander.momentum.x = 0;
                    lander.momentum.y = 0;
                    camera = previouscamera;
                    return;
                } else {
                    lander.momentum.x = -1*lander.momentum.x/2;
                    if (lander.momentum.x > -1) {
                        lander.momentum.x = -1; // Bounce!
                    }
                }
            }
        }
    } else if (lander.momentum.x < 0) {
        feet = lander.previous.y+LANDER_HEIGHT;
        edge = lander.previous.x;
        for (i = edge; i > edge+lander.momentum.x; i--) {
            if (i > edge) {
                break;
            }
            if (moon[i] < feet) {
                if (lander.momentum.x < -10) {
                    lander.bitmap = img_leftcrash;
                    lander.crashed = true;
                    lander.x = i;
                    lander.y = lander.previous.y;
                    lander.acceleration.x = 0;
                    lander.acceleration.y = 0;
                    lander.momentum.x = 0;
                    lander.momentum.y = 0;
                    camera = previouscamera;
                    return;
                } else {
                    lander.momentum.x = -1*lander.momentum.x/2;
                    if (lander.momentum.x < 1) {
                        lander.momentum.x = 1; // Bounce!
                    }
                }
            }
        }
    }

    if (lander.momentum.y > 0) {
        edge = lander.previous.x;
        max = edge;
        for (i = edge; i < edge+LANDER_WIDTH; i++) {
            if (moon[i] < moon[max]) {
                max = i;
            }
        }
        if (moon[max] < lander.previous.y+lander.momentum.y) {
            lander.y = moon[max]-LANDER_HEIGHT;
            lander.perched = true;
            lander.acceleration.y = 0;
            lander.momentum.y = 0;
        }
    }
}

void apply_input(void)
{
    union keyrow_6 k6;
    union keyrow_0 k0;

    if (!lander.crashed && !lander.landed) {
        scan_row_6(&k6);
    } else {
        k6.keys.K_RIGHT = false;
        k6.keys.K_LEFT = false;
        k6.keys.K_UP = false;
    }
    scan_row_0(&k0);

    if (k6.keys.K_RIGHT) {
        if (lander.acceleration.x < 10 && lander.x < MOON_WIDTH-LANDER_WIDTH-1) {
            lander.acceleration.x++;
        }
        lander.thrust.hp_firing = true;
        lander.thrust.hp_stage++;
    } else {
        lander.thrust.hp_firing = false;
        lander.thrust.hp_stage = 0;
    }
    if (k6.keys.K_LEFT) {
        if (lander.acceleration.x > -10 && lander.x > 0) {
            lander.acceleration.x--;
        }
        lander.thrust.hn_firing = true;
        lander.thrust.hn_stage++;
    } else {
        lander.thrust.hn_firing = false;
        lander.thrust.hn_stage = 0;
    }

    if (k6.keys.K_LEFT && k6.keys.K_RIGHT) {
        lander.acceleration.x = 0;
    }
    if (!k6.keys.K_LEFT && !k6.keys.K_RIGHT) {
        lander.acceleration.x = 0;
    }

    if (k6.keys.K_UP) {
        if (lander.acceleration.y > -10 && lander.y > 0) {
            lander.acceleration.y--;
        }
        lander.thrust.vp_firing = true;
        lander.thrust.vp_stage++;
    } else {
        if (!lander.crashed && !lander.perched) {
            lander.acceleration.y = 1;
        }
        lander.thrust.vp_firing = false;
        lander.thrust.vp_stage = 0;
    }
    
    if (k0.keys.K_EXIT) {
        add_event(QUIT);
    }
}
