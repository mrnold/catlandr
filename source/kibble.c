#include "bitmap.h"
#include "camera.h"
#include "kibble.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "calc/display.h"
#include "calc/ram.h"
#include "calc/timer.h"

void create_kibble(unsigned char index, unsigned short x, unsigned char y, char speedx, char speedy)
{
    kibbles[index].y = y;
    kibbles[index].x = x;
    kibbles[index].ready = false;
    kibbles[index].speed.y = speedy;
    kibbles[index].speed.x = speedx;
}

void draw_kibbles(void)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        if (kibbles[i].eaten) {
            continue;
        }
        draw_live_sprite(*(kibbles[i].bitmap), kibbles[i].stage, kibbles[i].x, kibbles[i].y, 0, kibbles[i].ready?XOR:OR);
    }
}

struct kibble_t *find_kibbles(unsigned short x)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        if (!kibbles[i].landed || kibbles[i].eaten) {
            continue;
        }
        if (x > kibbles[i].x && x < kibbles[i].x+KIBBLE_WIDTH) {
            return &kibbles[i];
        }
    }
    return (struct kibble_t *)NULL;
}

void move_kibbles(void)
{
    unsigned char i;
    unsigned short j;
    unsigned char max;
    int scratch;

    for (i = 0; i < KIBBLE_MAX; i++) {
        if (kibbles[i].landed) {
            continue;
        }
        if (kibbles[i].ready) {
            kibbles[i].x = camera+i*KIBBLE_WIDTH;
            continue;
        }

        max = SCREEN_HEIGHT;
        for (j = kibbles[i].x; j < kibbles[i].x+8; j++) {
            if (moon[j] < max) {
                max = moon[j];
            }
        }

        scratch = kibbles[i].y+kibbles[i].speed.y;
        if (scratch <= 0) {
            kibbles[i].y = 0;
            kibbles[i].speed.y = 0;
        } else if (scratch > max-8) {
            kibbles[i].y = max-8;
            kibbles[i].landed = true;
        } else {
            kibbles[i].y = scratch;
        }

        if (kibbles[i].speed.y < 1) { // Gravity
            kibbles[i].speed.y++;
        }

        scratch = kibbles[i].x+kibbles[i].speed.x;
        if (scratch <= 0) {
            kibbles[i].x = 0;
            kibbles[i].speed.x = 0;
        } else if (scratch > MOON_WIDTH-KIBBLE_WIDTH) {
            kibbles[i].x = MOON_WIDTH-KIBBLE_WIDTH;
            kibbles[i].speed.x = 0;
        } else {
            kibbles[i].x = scratch;
        }

        // "Air" friction
        if (kibbles[i].speed.x > 0) {
            kibbles[i].speed.x--;
        } else if (kibbles[i].speed.x < 0) {
            kibbles[i].speed.x++;
        }

        if ((t&0x07) == 0) {
            kibbles[i].stage++;
        }
    }
}
