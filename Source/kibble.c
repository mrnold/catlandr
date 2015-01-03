#include "bitmap.h"
#include "kibble.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

struct kibble_t kibbles[KIBBLE_MAX];

void init_kibbles(void)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        kibbles[i].y = SCREEN_HEIGHT-8;
        kibbles[i].x = i*KIBBLE_WIDTH+1;
        kibbles[i].bitmap = &kibble;
        kibbles[i].stage = 0;
        kibbles[i].active = 0;
        kibbles[i].landed = false;
        kibbles[i].speed.x = 0;
        kibbles[i].speed.y = 1;
    }
}

void create_kibble(unsigned char index, unsigned short x, unsigned char y, char speedx, char speedy)
{
    kibbles[index].active = true;
    kibbles[index].y = y;
    kibbles[index].x = x;
    kibbles[index].speed.x = speedx;
    kibbles[index].speed.y = speedy;
}

void draw_kibbles(void)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        draw_live_sprite(*(kibbles[i].bitmap), kibbles[i].stage, kibbles[i].x, kibbles[i].y, 0, XOR);
    }
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
        if (!kibbles[i].active) {
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

        if (kibbles[i].speed.y < 1) {
            kibbles[i].speed.y++;
        }

        if (t%8 == 0) {
            kibbles[i].stage = (kibbles[i].stage+1)%KIBBLE_FALLSTAGES;
        }
    }
}
