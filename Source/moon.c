#include "lander.h"
#include "ti86.h"
#include "moon.h"

unsigned short landingpad;
unsigned char moon[MOON_WIDTH];

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

    for (i = 0; i < MOON_WIDTH; i++) {
        moon[i] = 0;
    }

    moon[0] = SCREEN_HEIGHT/8;
    moon[MOON_WIDTH-1] = SCREEN_HEIGHT/8;

    landingpad = random16()%(MOON_WIDTH-1);
    landingheight = random8()%(SCREEN_HEIGHT/2);
    if (landingpad > MOON_WIDTH-21) {
        landingpad = MOON_WIDTH-21;
    }
    if (landingheight < 10) {
        landingheight = 10;
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
            if (distortion > SCREEN_HEIGHT-20) {
                distortion = SCREEN_HEIGHT-20;
            }
            if (distortion < 2) {
                distortion = 2;
            }
            moon[j] = (unsigned char)distortion;
        }
        stride = i;
        i /= 2;
    }

    // Flip the world around so positive y-axis is screen down
    for (i = 0; i < MOON_WIDTH; i++) {
        moon[i] = SCREEN_HEIGHT-moon[i];
    }
}

void draw_moon(void)
{
    unsigned int i;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        draw_vertical(i, moon[i+camera]);
    }
}
