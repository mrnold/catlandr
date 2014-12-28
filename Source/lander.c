#include "bitmap.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

struct lander_t lander;

void init_lander(void)
{
    lander.y = 0;
    lander.x = SCREEN_WIDTH/2;
    lander.momentum.x = 0;
    lander.momentum.y = 0;
    lander.acceleration.x = 0;
    lander.acceleration.y = 1;
    lander.bitmap = img_lander;
    lander.thrust.hp_stage = 0;
    lander.thrust.hp_firing = 0;
    lander.thrust.hn_stage = 0;
    lander.thrust.hn_firing = 0;
    lander.thrust.vp_stage = 0;
    lander.thrust.vp_firing = 0;
    lander.crashed = false;
    lander.landed = false;
    lander.perched = false;
}

void draw_lander(void)
{
    const unsigned int yoffset = lander.y*16;
    const unsigned char landerx = (unsigned char)(lander.x-camera);

    unsigned char i, y, x, rshift, lshift, imgbyte, *screenbyte;
    unsigned int start;

    rshift = landerx%8;
    lshift = 8-rshift;
    start = landerx/8+yoffset;
    screenbyte = (unsigned char *)screenbuffer+start;
    for (i = 0; i < LANDER_HEIGHT; i++) {
        *screenbyte |= (lander.bitmap[i] >> rshift);
        *(screenbyte+1) |= (lander.bitmap[i] << lshift);
        screenbyte += 16;
    }

    if (lander.thrust.hp_firing && landerx > 0) { // Draw left thruster
        x = landerx-LANDER_WIDTH;
        rshift = x%8;
        lshift = 8-rshift;
        start = x/8+yoffset;
        screenbyte = (unsigned char *)screenbuffer+start;
        if (landerx >= LANDER_WIDTH) {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustleft[i][lander.thrust.hp_stage];
                *screenbyte |= (imgbyte >> rshift);
                *(screenbyte+1) |= (imgbyte << lshift);
                screenbyte += 16;
            }
        } else {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustleft[i][lander.thrust.hp_stage];
                *(screenbyte+1) |= (imgbyte << lshift);
                screenbyte += 16;
            }
        }
    }

    x = landerx+LANDER_WIDTH;
    if (lander.thrust.hn_firing && x < SCREEN_WIDTH) { // Draw right thruster
        rshift = x%8;
        lshift = 8-rshift;
        start = x/8+yoffset;
        screenbyte = (unsigned char *)screenbuffer+start;
        if (x < SCREEN_WIDTH-LANDER_WIDTH) {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustright[i][lander.thrust.hn_stage];
                *screenbyte |= (imgbyte >> rshift);
                *(screenbyte+1) |= (imgbyte << lshift);
                screenbyte += 16;
            }
        } else {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustright[i][lander.thrust.hn_stage];
                *screenbyte |= (imgbyte >> rshift);
                screenbyte += 16;
            }
        }
    }

    if (lander.thrust.vp_firing) {
        y = lander.y+LANDER_HEIGHT;
        rshift = landerx%8;
        start = landerx/8+y*16;
        screenbyte = (unsigned char *)screenbuffer+start;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            imgbyte = img_thrustdown[i][lander.thrust.vp_stage];
            *screenbyte ^= (imgbyte >> rshift);
            *(screenbyte+1) ^= (imgbyte << lshift);
            screenbyte += 16;
        }
    }
}
