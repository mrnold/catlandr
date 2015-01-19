#include "bitmap.h"
#include "lander.h"
#include "moon.h"
#include "ti86.h"

unsigned short landingpad;
__at (MOON_SAFERAM) unsigned char moon[MOON_WIDTH];

void init_moon(void)
{
    generate_moon();
}

void generate_moon(void)
{
    unsigned int i, j;
    unsigned int stride;
    unsigned int left, right;
    unsigned int leftheight, rightheight;
    unsigned int distortion;
    unsigned char rand;
    unsigned char landingheight;

    __asm
        push hl
        push de
        push bc

        ld hl, #_moon
        ld (hl), #0
        ld de, #_moon+1
        ld bc, #1024
        ldir

        pop bc
        pop de
        pop hl
    __endasm;

    moon[0] = SCREEN_HEIGHT-SCREEN_HEIGHT/8;
    moon[MOON_WIDTH-1] = SCREEN_HEIGHT-SCREEN_HEIGHT/8;

    landingpad = random16()%(MOON_WIDTH-1);
    landingheight = SCREEN_HEIGHT-random8()%(SCREEN_HEIGHT/2);
    if (landingpad > MOON_WIDTH-21) {
        landingpad = MOON_WIDTH-21;
    }
    if (landingheight > SCREEN_HEIGHT-10) {
        landingheight = SCREEN_HEIGHT-10;
    }
    if (landingpad+LANDINGPAD_WIDTH > MOON_WIDTH) {
        for (i = landingpad; i < MOON_WIDTH-landingpad; i++) {
            moon[i] = landingheight;
        }
    } else {
        for (i = landingpad; i < landingpad+LANDINGPAD_WIDTH; i++) {
            moon[i] = landingheight;
        }
    }

    i = (MOON_WIDTH-1)/2;
    stride = MOON_WIDTH-1;
    while (i > 0) {
        for (j = i; j < MOON_WIDTH-1; j += stride) {
            if (moon[j] != 0) {
                continue;
            }
            left = j-i;
            right = j+i;
            leftheight = moon[left];
            rightheight = moon[right];
            rand = random8() & 0x01;
            if (rand == 1) {
                distortion = (leftheight+rightheight)/2+(i&0x1f);
            } else {
                distortion = (leftheight+rightheight)/2-(i&0x1f);
            }
            if (distortion < 20) {
                distortion = 20;
            }
            if (distortion > SCREEN_HEIGHT-LANDER_HEIGHT) {
                distortion = SCREEN_HEIGHT-LANDER_HEIGHT;
            }
            moon[j] = (unsigned char)distortion;
        }
        stride = i;
        i /= 2;
    }

    prerender();
}
