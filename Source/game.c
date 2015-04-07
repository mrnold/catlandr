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
#include "calc/display.h"
#include "calc/input.h"
#include "calc/ram.h"
#include "calc/timer.h"

unsigned int crashes = 0;
unsigned int landings = 0;
__at (GAMESTATE_ADDRESS) unsigned char gamestate;

extern __at (FRAMELOCK_ADDRESS) lock_t frame_lock;
extern __at (IDLELOCK_ADDRESS) lock_t idle_lock;

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
    updatescreen();
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
    switch (gamestate) {
    case DONE_STRANDED:
    case DONE_CRASHED:
        if (crashes != 0xFFFF) { crashes++; } break;
    case DONE_LANDED:
        if (landings != 0xFFFF) { landings++; } break;
    default:
    }
}

void apply_input(void)
{
    union keyrow_6 k6;
    union keyrow_0 k0;
    static __at (PREVKEY0_ADDRESS) union keyrow_0 prev_k0 = {.raw = 0};

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
        lander.hp_firing = true;
        if ((t&0x03) == 0) { // Cycle ~6 times/sec
            lander.hp_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        lander.hp_firing = false;
        lander.hp_stage = 0;
    }
    if (k6.keys.K_LEFT) {
        if (lander.acceleration.x > -ACCEL_MAX && lander.x > 0) {
            lander.acceleration.x--;
        }
        lander.hn_firing = true;
        if ((t&0x03) == 0) {
            lander.hn_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        lander.hn_firing = false;
        lander.hn_stage = 0;
    }

    // If both keys are pressed, cancel each acceleration
    // (without cancelling fuel consumption)
    lander.acceleration.x *= (k6.keys.K_LEFT ^ k6.keys.K_RIGHT);

    if (k6.keys.K_UP || k6.keys.K_DOWN) {
        if (lander.acceleration.y > -ACCEL_MAX && lander.y > 0) {
            lander.acceleration.y--;
        }
        lander.vp_firing = true;
        if ((t&0x03) == 0) {
            lander.vp_stage++;
            if (lander.fuel != 0) {
                lander.fuel--;
            }
        }
    } else {
        if (!lander.freedom.stuck.crashed) {
            lander.acceleration.y = GRAVITY;
        }
        lander.vp_firing = false;
        lander.vp_stage = 0;
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
