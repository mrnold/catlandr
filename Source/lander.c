#include "bitmap.h"
#include "kitty.h"
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
    lander.speed.x = 0;
    lander.speed.y = 0;
    lander.acceleration.x = 0;
    lander.acceleration.y = GRAVITY;
    lander.bitmap = img_lander;
    lander.thrust.hp_stage = 0;
    lander.thrust.hp_firing = 0;
    lander.thrust.hn_stage = 0;
    lander.thrust.hn_firing = 0;
    lander.thrust.vp_stage = 0;
    lander.thrust.vp_firing = 0;
    lander.crashed = false;
    lander.landed = false;
    lander.fuel = 127;
    lander.food = 31;
    lander.air = 255;
}

void move_lander(void)
{
    unsigned int landerlimit;
    unsigned char feet;
    int scratch;
    char rand;

    lander.previous.x = lander.x;
    lander.previous.y = lander.y;
    previouscamera = camera;

    if (lander.crashed) {
        kitty.batting = false;
        return;
    }

    if (kitty.x+KITTY_WIDTH >= lander.x && kitty.x <= lander.x+LANDER_WIDTH) {
        if (kitty.y >= lander.y && kitty.y <= lander.y+LANDER_HEIGHT) { // Hit!
            if (kitty.speed.x == 0 && kitty.speed.y == 0) {
                lander.speed.y = -SPEED_MAX; // Bat lander upwards
                rand = (char)((random8()<<1)&0x82);
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
    }

    lander.speed.x += lander.acceleration.x/2;
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

    feet = lander.y+LANDER_HEIGHT;
    landerlimit = MOON_WIDTH-LANDER_WIDTH;
    scratch = (int)lander.x + (int)lander.speed.x;
    if (scratch >= (int)landerlimit) {
        lander.x = landerlimit;
        lander.speed.x = 0;
    } else if (scratch <= 0) {
        lander.speed.x = 0;
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


    lander.speed.y += lander.acceleration.y/2;
    if (lander.speed.y > SPEED_MAX) {
        lander.speed.y = SPEED_MAX;
    } else if (lander.speed.y < -SPEED_MAX) {
        lander.speed.y = -SPEED_MAX;
    }

    if (lander.speed.y > 0) { //Screen down
        landerlimit = SCREEN_HEIGHT-LANDER_HEIGHT;
        lander.y += lander.speed.y/4;
        if (lander.y > landerlimit) {
            lander.y = landerlimit;
            lander.speed.y = 0;
            lander.acceleration.y = 0;
        }
    } else if (lander.speed.y < 0) { //Screen up
        scratch = lander.y + lander.speed.y/4;
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
