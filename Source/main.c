#include "bitmap.h"
#include "event.h"
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

lock_t main_lock;

int main(void)
{
    init();
    while (running) {
        event_t e;

        wait_lock(main_lock);
        do {
            e = get_event();
            if (e == SIGNAL_GO) {
                gamesequence();
                frames++;
            } else if (e == QUIT) {
                running = false;
            }
        } while (e != NONE);
        drop_lock(main_lock);

        if (running) {
            perfcheck();
            idle();
        }
    }

    return 0;
}

void gamesequence(void)
{
    apply_input();
    physics();
    collisions();
    draw_moon();
    screencopy();
    draw_lander();
}

void init(void)
{
    ticks = 0;
    frames = 0;
    dropped = 0;
    running = true;

    init_event();
    init_lander();
    init_moon();
    init_physics();

    init_lock(main_lock);
    setup_timer(&timer_callback);
}

void timer_callback(void)
{
    if (ticks%8 == 0) {
        if (is_locked(main_lock)) {
            dropped++;
        } else {
            add_event(SIGNAL_GO);
        }
    }
    ticks++;
}

void perfcheck(void)
{
    if (frames == 1000) {
        running = false;
    }
}
