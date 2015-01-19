#include "camera.h"
#include "moon.h"
#include "physics.h"
#include "lander.h"
#include "ti86.h"

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

    scratch = lander.phys.position.x-camera;
    if (scratch < LANDER_WIDTH) {
        scratch = lander.phys.position.x-LANDER_WIDTH;
        if (scratch < 0) {
            camera = 0;
        } else {
            camera = scratch;
        }
    }

    scratch = camera+SCREEN_WIDTH-lander.phys.position.x;
    if (scratch < 2*LANDER_WIDTH) {
        scratch = lander.phys.position.x+2*LANDER_WIDTH-SCREEN_WIDTH;
        if (scratch+SCREEN_WIDTH > MOON_WIDTH) {
            camera = MOON_WIDTH-SCREEN_WIDTH;
        } else {
            camera = scratch;
        }
    }
}
