#include "bitmap.h"
#include "lander.h"
#include "misc.h"
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
    unsigned char * const screen = (unsigned char *)0xfc00;

    unsigned char i, y, x, shift, imgbyte, *screenbyte;
    unsigned int start;

    shift = landerx%8;
    start = landerx/8+yoffset;
    screenbyte = screen+start;
    for (i = 0; i < LANDER_HEIGHT; i++) {
        *screenbyte |= (lander.bitmap[i] >> shift);
        *(screenbyte+1) |= (lander.bitmap[i] << (8-shift));
        screenbyte += 16;
    }

    if (lander.thrust.hp_firing && landerx > 0) { // Draw left thruster
        x = landerx-LANDER_WIDTH;
        shift = x%8;
        start = x/8+yoffset;
        screenbyte = screen+start;
        if (landerx >= LANDER_WIDTH) {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustleft[i][lander.thrust.hp_stage];
                *screenbyte |= (imgbyte >> shift);
                *(screenbyte+1) |= (imgbyte << (8-shift));
                screenbyte += 16;
            }
        } else {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustleft[i][lander.thrust.hp_stage];
                *(screenbyte+1) |= (imgbyte << (8-shift));
                screenbyte += 16;
            }
        }
    }

    x = landerx+LANDER_WIDTH;
    if (lander.thrust.hn_firing && x < SCREEN_WIDTH) { // Draw right thruster
        shift = x%8;
        start = x/8+yoffset;
        screenbyte = screen+start;
        if (x < SCREEN_WIDTH-LANDER_WIDTH) {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustright[i][lander.thrust.hn_stage];
                *screenbyte |= (imgbyte >> shift);
                *(screenbyte+1) |= (imgbyte << (8-shift));
                screenbyte += 16;
            }
        } else {
            for (i = 0; i < LANDER_HEIGHT; i++) {
                imgbyte = img_thrustright[i][lander.thrust.hn_stage];
                *screenbyte |= (imgbyte >> shift);
                screenbyte += 16;
            }
        }
    }

    if (lander.thrust.vp_firing) {
        y = lander.y+LANDER_HEIGHT;
        shift = landerx%8;
        start = landerx/8+y*16;
        screenbyte = screen+start;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            imgbyte = img_thrustdown[i][lander.thrust.vp_stage];
            *screenbyte ^= (imgbyte >> shift);
            *(screenbyte+1) ^= (imgbyte << (8-shift));
            screenbyte += 16;
        }
    }
}
