#include "bitmap.h"
#include "camera.h"
#include "game.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "lock.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

unsigned int frames;
unsigned int dropped;
unsigned char gamestate;

extern lock_t frame_lock;
extern lock_t idle_lock;

void init_game(void)
{
    frames = 0;
    dropped = 0;
}

void force_call(void)
{
    moon;
}

void gamesequence(void)
{
    apply_input();
    move_kibbles();
    move_kitty();
    move_lander();
    collisions();
    move_camera();
    draw_moon();
    draw_lander();
    draw_kitty();
    draw_kibbles();
    draw_status();
    flipscreen();
    force_call();
}

void loop_game(void)
{
    setup_refresh(gamesequence);
    while (gamestate == GAME_RUNNING) {
        wait_lock(idle_lock);
        wait_lock(frame_lock);
        gamesequence();
        frames++;
        t++; // This should be done after all physics calls are finished
        drop_lock(frame_lock);

        while (is_locked(idle_lock)) {
            idle();
        }
    }
}

void apply_input(void)
{
    union keyrow_6 k6;
    union keyrow_0 k0;
    static union keyrow_0 prev_k0 = {.raw = 0};

    if (!lander.freedom.stopped && lander.fuel > 0) {
        scan_row_6(&k6);
    } else {
        k6.raw = 0;
    }
    scan_row_0(&k0);

    if (k6.keys.K_RIGHT) {
        if (lander.acceleration.x < ACCEL_MAX && lander.x < MOON_WIDTH-LANDER_WIDTH-1) {
            lander.acceleration.x++;
        }
        lander.thrust.hp_firing = true;
        if ((t&0x03) == 0) { // Cycle ~6 times/sec
            lander.thrust.hp_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        lander.thrust.hp_firing = false;
        lander.thrust.hp_stage = 0;
    }
    if (k6.keys.K_LEFT) {
        if (lander.acceleration.x > -ACCEL_MAX && lander.x > 0) {
            lander.acceleration.x--;
        }
        lander.thrust.hn_firing = true;
        if ((t&0x03) == 0) {
            lander.thrust.hn_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        lander.thrust.hn_firing = false;
        lander.thrust.hn_stage = 0;
    }

    // If both keys are pressed, cancel each acceleration
    // (without cancelling fuel consumption)
    lander.acceleration.x *= (k6.keys.K_LEFT ^ k6.keys.K_RIGHT);

    if (k6.keys.K_UP) {
        if (lander.acceleration.y > -ACCEL_MAX && lander.y > 0) {
            lander.acceleration.y--;
        }
        lander.thrust.vp_firing = true;
        if ((t&0x03) == 0) {
            lander.thrust.vp_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        if (!lander.freedom.stuck.crashed) {
            lander.acceleration.y = GRAVITY;
        }
        lander.thrust.vp_firing = false;
        lander.thrust.vp_stage = 0;
    }

    if (k0.keys.K_2ND && !prev_k0.keys.K_2ND && lander.food > 0) {
        lander.food--;
        create_kibble(lander.food, lander.x, lander.y, lander.speed.x, lander.speed.y);
    }

    if (k0.keys.K_EXIT) {
        gamestate = EXIT;
    }
    if (k0.keys.K_F1) {
        gamestate = START_MENU;
    }

    prev_k0.raw = k0.raw;
}
