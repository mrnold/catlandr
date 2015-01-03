#include "bitmap.h"
#include "lander.h"
#include "lock.h"
#include "kibble.h"
#include "kitty.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

unsigned int ticks;
unsigned int frames;
unsigned int dropped;
unsigned char running;
unsigned char menu;

void init(void);
void reset(void);
void showmenu(void);
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
    "Herding cats..."
};

void init(void)
{
    menu = true;
    running = false;
    printxy(0, 0, loading[random8()%4]);
    reset();
}

int main(void)
{
    clear_screen();
    init();
    while (menu || running) {
        menusequence();
        showmenu();
        while (menu) {
            wait_lock(idle_lock);
            menu_input();
            while (is_locked(idle_lock)) {
                idle();
            }
        }

        while (running) {
            wait_lock(idle_lock);
            wait_lock(frame_lock);
            gamesequence();
            frames++;
            t++; // This should be done after all physics calls are finished
            drop_lock(frame_lock);

            if (running) {
                while (is_locked(idle_lock)) {
                    idle();
                }
            } else {
                if (lander.freedom.stopped) {
                    menu = true;
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
    move_kibbles();
    move_kitty();
    move_lander();
    collisions();
    draw_moon();
    draw_lander();
    draw_kitty();
    draw_kibbles();
    draw_status();
    screencopy();
    force_call();
}

void menusequence(void)
{
    draw_moon();
    draw_lander();
    draw_kitty();
    draw_kibbles();
    screencopy();
}

void reset(void)
{
    ticks = 0;
    frames = 0;
    dropped = 0;

    init_kibbles();
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
        if (!menu && running && is_locked(frame_lock)) {
            dropped++;
        }
        drop_lock(idle_lock);
    }
    ticks++;
}

void menu_input(void)
{
    union keyrow_0 k0;
    union keyrow_6 k6;
    static union keyrow_6 prev_k6 = {.raw = 0};

    scan_row_0(&k0);
    scan_row_6(&k6);

    if (k0.keys.K_EXIT) {
        running = false;
        menu = false;
    }

    if (k6.raw && !prev_k6.raw && !lander.freedom.stopped) {
        running = true;
        menu = false;
    }

    if (k0.keys.K_F2) {
        init();
        menusequence();
        showmenu();
    }

    prev_k6.raw = k6.raw;
}

void showmenu(void)
{
    printxy(0, 0,  "EXIT to quit");
    printxy(0, 6,  "F1 for this menu");
    printxy(0, 12, "F2 for a new moon");
    printxy(0, 18, "2ND to drop a kibble");
    printxy(0, 24, "Arrow keys for thrusters");

    if (lander.freedom.stuck.crashed) {
        printxy(87, 6, "Lander");
        printxy(91, 12, "destroyed!");
    } else if (lander.freedom.stuck.stranded) {
        printxy(80, 6, "Out of fuel!");
    } else if (lander.freedom.stuck.landed) {
        printxy(80, 6, "Safe landing!");
    } else {
        printxy(89, 0,  "Land safely");
        printxy(85, 6,  "to feed Luna!");
        printxy(88, 12, "Thrust up to");
        printxy(88, 18, "get started!");
    }
}
