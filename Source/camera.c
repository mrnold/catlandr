#include "bitmap.h"
#include "camera.h"
#include "kibble.h"
#include "lander.h"
#include "moon.h"
#include "ti86display.h"

unsigned int camera;
unsigned int previouscamera;

void init_camera(void)
{
    previouscamera = 0;
    camera = 0;
}

void move_camera(void)
{
    int scratch;
    previouscamera = camera;

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
}
