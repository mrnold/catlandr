#include "bitmap.h"
#include "lander.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"
#include "kitty.h"

struct kitty_t kitty;

void init_kitty(void)
{
    kitty.y = 0;
    kitty.x = 256;
    kitty.speed.x = 0;
    kitty.speed.y = 0;
    kitty.stage = 0;
    kitty.state = SITTING;
    kitty.bitmap = &cat_sitting;
}

void draw_kitty(void)
{
    const unsigned int yoffset = kitty.y*16;

    unsigned char i, rshift, lshift, imgbyte, *screenbyte, kittyx;
    unsigned int start;
    int scratch;

    if (kitty.x > camera+SCREEN_WIDTH || kitty.x+KITTY_WIDTH < camera) {
        return;
    }

    scratch = kitty.x-camera;
    kittyx = (unsigned char)scratch;

    rshift = kittyx%8;
    lshift = 8-rshift;
    start = kittyx/8+yoffset;

    if (scratch >= 0 && scratch < SCREEN_WIDTH-KITTY_WIDTH) {
        screenbyte = (unsigned char *)screenbuffer+start;
        for (i = 0; i < KITTY_HEIGHT; i++) {
            imgbyte = (*kitty.bitmap)[i][kitty.stage];
            *screenbyte |= (imgbyte >> rshift);
            *(screenbyte+1) |= (imgbyte << lshift);
            screenbyte += 16;
        }
    } else if (scratch < 0 && scratch > -KITTY_WIDTH) {
        screenbyte = (unsigned char *)screenbuffer+yoffset;
        for (i = 0; i < KITTY_HEIGHT; i++) {
            imgbyte = (*kitty.bitmap)[i][kitty.stage];
            *(screenbyte) |= (imgbyte << lshift);
            screenbyte += 16;
        }
    } else if (scratch >= 0 && scratch < SCREEN_WIDTH) {
        screenbyte = (unsigned char *)screenbuffer+start;
        for (i = 0; i < KITTY_HEIGHT; i++) {
            imgbyte = (*kitty.bitmap)[i][kitty.stage];
            *screenbyte |= (imgbyte >> rshift);
            screenbyte += 16;
        }
    }
}

void move_kitty(void)
{
    int scratch;
    unsigned int i;
    unsigned char max = SCREEN_HEIGHT;

    for (i = kitty.x; i < kitty.x+KITTY_WIDTH; i++) {
        if (moon[i] < max) { // Find highest peak under cat's feet
            max = moon[i];
        }
    }

    scratch = kitty.y + kitty.speed.y;
    if (kitty.speed.y < 0) { // Jumping
        if (scratch < 0) {
            scratch = 0;
        }
        if (max < scratch) { // Landed on a hill
            kitty.y = max-KITTY_HEIGHT;
            kitty.speed.y = 0;
        } else {
            kitty.y = scratch;
        }
    } else if (kitty.speed.y >= 0) {
        if (kitty.y > max-KITTY_HEIGHT) {
            kitty.y = max-KITTY_HEIGHT;
            kitty.speed.y = 0;
        } else {
            kitty.y = scratch;
        }
    }

    if (kitty.y < max-KITTY_HEIGHT) {
        kitty.speed.y++; // Gravity
    }

    if (lander.x > kitty.x) {
        kitty.speed.x = 4;
    } else if (lander.x < kitty.x) {
        kitty.speed.x = -4;
    } else {
        kitty.speed.x = 0;
    }

    scratch = kitty.x + kitty.speed.x;
    if (scratch < 0) {
        kitty.x = 0;
        kitty.speed.x = 0;
    } else if (scratch > MOON_WIDTH-KITTY_WIDTH) {
        kitty.x = MOON_WIDTH-KITTY_WIDTH;
        kitty.speed.x = 0;
    } else {
        kitty.x = scratch;
    }

    if (kitty.speed.x > 0) { // Jump range
        if (scratch > lander.x) {
            kitty.x = lander.x;
            kitty.speed.x = 0;
        } else if (scratch > lander.x-3*KITTY_WIDTH) {
            kitty.speed.y = -3;
        }
    } else if (kitty.speed.x < 0) {
        if (scratch < lander.x+KITTY_WIDTH) {
            kitty.x = lander.x;
            kitty.speed.x = 0;
        } else if (scratch < lander.x+3*KITTY_WIDTH) {
            kitty.speed.y = -3;
        }
    }

    if (kitty.speed.y < 0) {
        kitty.state = JUMPING;
    } else {
        if (kitty.speed.x > 0) {
            kitty.bitmap = &cat_runright;
            kitty.state = RUNNING_RIGHT;
        } else if (kitty.speed.x < 0) {
            kitty.bitmap = &cat_runleft;
            kitty.state = RUNNING_LEFT;
        } else {
            kitty.bitmap = &cat_sitting;
            kitty.state = SITTING;
        }
    }

    if (t%8 == 0) {
        kitty.stage = (kitty.stage+1)%CAT_RUNSTAGES;
    }
}
