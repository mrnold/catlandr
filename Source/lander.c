#include "bitmap.h"
#include "lander.h"
#include "misc.h"
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
    unsigned char i, y;
    unsigned char x = lander.x-camera;
    unsigned char *screen = (unsigned char *)0xfc00;
    unsigned int start = x/8+lander.y*16;
    unsigned char *screenbyte = screen+start;
    unsigned char shift = x%8;
    unsigned char imgbyte;

    for (i = 0; i < LANDER_HEIGHT; i++) {
        *screenbyte |= (lander.bitmap[i] >> shift);
        *(screenbyte+1) |= (lander.bitmap[i] << (8-shift));
        screenbyte += 16;
    }

    if (lander.thrust.hp_firing) { // Draw left thruster
        x = lander.x-camera-LANDER_WIDTH;
        start = x/8+lander.y*16;
        screenbyte = screen+start;
        shift = x%8;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            imgbyte = img_thrustleft[i][lander.thrust.hp_stage];
            if (lander.x-camera >= LANDER_WIDTH) {
                *screenbyte |= (imgbyte >> shift);
            }
            if (lander.x-camera > 0) {
                *(screenbyte+1) |= (imgbyte << (8-shift));
            }
            screenbyte += 16;
        }
    }

    if (lander.thrust.hn_firing) { // Draw right thruster
        x = lander.x-camera+LANDER_WIDTH;
        start = x/8+lander.y*16;
        screenbyte = screen+start;
        shift = x%8;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            imgbyte = img_thrustleft[i][lander.thrust.hn_stage];
            imgbyte = reverse(imgbyte);
            if (x < SCREEN_WIDTH) {
                *screenbyte |= (imgbyte >> shift);
            }
            if (x < SCREEN_WIDTH-LANDER_WIDTH) {
                *(screenbyte+1) |= (imgbyte << (8-shift));
            }
            screenbyte += 16;
        }
    }

    if (lander.thrust.vp_firing) {
        x = lander.x-camera;
        y = lander.y+LANDER_HEIGHT;
        start = x/8+y*16;
        screenbyte = screen+start;
        shift = x%8;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            if (i+y > SCREEN_HEIGHT) {
                break;
            }
            imgbyte = img_thrustdown[i][lander.thrust.vp_stage];
            *screenbyte ^= (imgbyte >> shift);
            *(screenbyte+1) ^= (imgbyte << (8-shift));
            screenbyte += 16;
        }
    }
}
