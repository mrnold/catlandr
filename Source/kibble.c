#include "bitmap.h"
#include "camera.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "kibble.h"
#include "ti86.h"

__at (0xa600) struct kibble_t kibbles[KIBBLE_MAX];

void init_kibbles(void)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        init_object(&kibbles[i].phys);
        kibbles[i].phys.velocity.y = 1;
        kibbles[i].phys.position.y = SCREEN_HEIGHT-8;
        kibbles[i].phys.position.x = i*KIBBLE_WIDTH+1;
        kibbles[i].bitmap = &kibble;
        kibbles[i].stage = 0;
        kibbles[i].ready = true;
        kibbles[i].eaten = false;
        kibbles[i].landed = false;
    }
}

void create_kibble(unsigned char index, unsigned short x, unsigned char y, char speedx, char speedy)
{
    kibbles[index].ready = false;
    kibbles[index].phys.position.y = y;
    kibbles[index].phys.position.x = x;
    kibbles[index].phys.velocity.y = speedy;
    kibbles[index].phys.velocity.x = speedx;
}

void draw_kibbles(void)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        if (kibbles[i].eaten) {
            continue;
        }
        draw_live_sprite(*(kibbles[i].bitmap), kibbles[i].stage, kibbles[i].phys.position.x, kibbles[i].phys.position.y, 0, kibbles[i].ready?XOR:OR);
    }
}

struct kibble_t *find_kibbles(unsigned short x)
{
    unsigned char i;
    for (i = 0; i < KIBBLE_MAX; i++) {
        if (!kibbles[i].landed || kibbles[i].eaten) {
            continue;
        }
        if (x > kibbles[i].phys.position.x && x < kibbles[i].phys.position.x+KIBBLE_WIDTH) {
            return &kibbles[i];
        }
    }
    return (struct kibble_t *)NULL;
}

void move_kibbles(void)
{
    unsigned char i;

    for (i = 0; i < KIBBLE_MAX; i++) {
        if (kibbles[i].landed) {
            continue;
        }
        if (kibbles[i].ready) {
            kibbles[i].phys.position.x = camera+i*KIBBLE_WIDTH;
            continue;
        }

        switch (move_object(&kibbles[i].phys)) {
            case OBJ_LOW_IMPACT_DOWN:
                kibbles[i].landed = true; break;
            case OBJ_HIGH_IMPACT_DOWN:
                kibbles[i].phys.velocity.y = -1*(kibbles[i].phys.velocity.y>>1);
        }

        if ((t&0x07) == 0) {
            kibbles[i].stage = (kibbles[i].stage+1)&(KIBBLE_FALLSTAGES-1);
        }

        /*
        max = SCREEN_HEIGHT;
        for (j = kibbles[i].phys.position.x; j < kibbles[i].phys.position.x+8; j++) {
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

        scratch = kibbles[i].phys.position.x+kibbles[i].phys.velocity.x;
        if (scratch <= 0) {
            kibbles[i].phys.position.x = 0;
            kibbles[i].phys.velocity.x = 0;
        } else if (scratch > MOON_WIDTH-KIBBLE_WIDTH) {
            kibbles[i].phys.position.x = MOON_WIDTH-KIBBLE_WIDTH;
            kibbles[i].phys.velocity.x = 0;
        } else {
            kibbles[i].phys.position.x = scratch;
        }

        // "Air" friction
        if (kibbles[i].speed.x > 0) {
            kibbles[i].speed.x--;
        } else if (kibbles[i].speed.x < 0) {
            kibbles[i].speed.x++;
        }*/

    }
}
