#include "bitmap.h"
#include "game.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "lock.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

extern unsigned int frames;
extern unsigned int dropped;
extern unsigned char gamestate;

extern lock_t frame_lock;
extern lock_t idle_lock;

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
