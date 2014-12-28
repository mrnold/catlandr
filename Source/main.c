#include "bitmap.h"
#include "lander.h"
#include "lock.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

unsigned int ticks;
unsigned int frames;
unsigned int dropped;
unsigned char running;

void init(void);
void perfcheck(void);
void gamesequence(void);
void timer_callback(void);

lock_t frame_lock;
lock_t idle_lock;

int main(void)
{
    init();
    while (running) {
        wait_lock(idle_lock);
        wait_lock(frame_lock);
        gamesequence();
        frames++;
        drop_lock(frame_lock);

        if (running) {
            perfcheck();
            while (is_locked(idle_lock)) {
                idle();
            }
        }
    }

    return 0;
}

void force_call(void)
{
    moon;
}

void gamesequence(void)
{
    apply_input();
    physics();
    collisions();
    draw_moon();
    screencopy();
    draw_lander();
    force_call();
}

void init(void)
{
    ticks = 0;
    frames = 0;
    dropped = 0;
    running = true;

    init_lander();
    init_moon();
    init_physics();

    init_lock(idle_lock);
    init_lock(frame_lock);
    setup_timer(&timer_callback);
}

void timer_callback(void)
{
    if (ticks%8 == 0) {
        if (is_locked(frame_lock)) {
            dropped++;
        }
        drop_lock(idle_lock);
    }
    ticks++;
}

void perfcheck(void)
{
    if (frames == 1000) {
        running = false;
    }
}
