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
    kitty.x = 0;
    kitty.stage = 0;
    kitty.state = SITTING;
    kitty.bitmap = &cat_sitting;
}

void draw_kitty(void)
{
    const unsigned int yoffset = kitty.y*16;
    const unsigned char kittyx = (unsigned char)(kitty.x-camera);

    unsigned char i, rshift, lshift, imgbyte, *screenbyte;
    unsigned int start;

    //if (kittyx > SCREEN_WIDTH || kittyx+KITTY_WIDTH < camera) {
        //return;
    //}

    rshift = kittyx%8;
    lshift = 8-rshift;
    start = kittyx/8+yoffset;
    screenbyte = (unsigned char *)screenbuffer+start;

    for (i = 0; i < KITTY_HEIGHT; i++) {
        imgbyte = (*kitty.bitmap)[i][kitty.stage];
        *screenbyte |= (imgbyte >> rshift);
        *(screenbyte+1) |= (imgbyte << lshift);
        screenbyte += 16;
    }
}

void move_kitty(void)
{
    unsigned int i;
    unsigned char max = SCREEN_HEIGHT;

    for (i = kitty.x; i < kitty.x+KITTY_WIDTH; i++) {
        if (moon[i] < max) {
            max = moon[i];
        }
    }
    kitty.y = max-KITTY_HEIGHT;
    kitty.x = lander.x;

    if (lander.speed.x > 0) {
        kitty.bitmap = &cat_runright;
        kitty.state = RUNNING_RIGHT;
    } else if (lander.speed.x < 0) {
        kitty.bitmap = &cat_runleft;
        kitty.state = RUNNING_LEFT;
    } else {
        kitty.bitmap = &cat_sitting;
        kitty.state = SITTING;
    }

    if (t%8 == 0) {
        kitty.stage = (kitty.stage+1)%CAT_RUNSTAGES;
    }
}
