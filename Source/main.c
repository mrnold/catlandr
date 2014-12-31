#include "bitmap.h"
#include "lander.h"
#include "lock.h"
#include "kitty.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

unsigned int ticks;
unsigned int frames;
unsigned int dropped;
unsigned char running;
unsigned char reset;
unsigned char menu;

void init(void);
void perfcheck(void);
void gamesequence(void);
void menusequence(void);
void timer_callback(void);
void menu_input(void);

lock_t frame_lock;
lock_t idle_lock;

char *loading[] = {
    "Creating lunar surface...",
    "Waxing the moon...",
    "Tranquilizing seas...",
    "Planting flags..."
};

int main(void)
{
    menu = true;
    running = false;
    clear_screen();
    printxy(0, 0, loading[random8()%4]);
    init();

    while (menu || running) {
        menusequence();
        printxy(0, 0,  "EXIT to quit");
        printxy(0, 6,  "F1 for this menu");
        printxy(0, 12, "F2 for a new moon");
        printxy(0, 18, "2ND to drop a kibble");
        printxy(0, 24, "Arrow keys for thrusters");

        printxy(89, 0,  "Land safely");
        printxy(85, 6,  "to feed Luna!");
        printxy(88, 12, "Thrust up to");
        printxy(88, 18, "get started!");
        while (menu) {
            menu_input();
        }

        while (running) {
            wait_lock(idle_lock);
            wait_lock(frame_lock);
            gamesequence();
            frames++;
            t++; // This should be done after all physics calls are finished
            drop_lock(frame_lock);

            if (reset) {
                init();
            }
            if (running) {
                perfcheck();
                while (is_locked(idle_lock)) {
                    idle();
                }
            }
        }
    }

    clear_screen();
    return 0;
}

void force_call(void)
{
    moon;
}

void gamesequence(void)
{
    apply_input();
    move_kitty();
    move_lander();
    collisions();
    draw_moon();
    draw_lander();
    draw_kitty();
    screencopy();
    force_call();
}

void menusequence(void)
{
    draw_moon();
    draw_lander();
    draw_kitty();
    screencopy();
    menu_input();
}

void init(void)
{
    ticks = 0;
    frames = 0;
    dropped = 0;
    reset = false;

    init_kitty();
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

void menu_input(void)
{
    union keyrow_0 k0;
    union keyrow_6 k6;

    scan_row_0(&k0);
    scan_row_6(&k6);

    if (k0.keys.K_EXIT) {
        running = false;
        menu = false;
    }

    if (k6.raw) {
        running = true;
        menu = false;
    }
}
