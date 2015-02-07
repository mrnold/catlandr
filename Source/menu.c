#include "bitmap.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "lock.h"
#include "menu.h"
#include "misc.h"
#include "moon.h"
#include "display.h"
#include "ti86.h"

extern unsigned char gamestate;
extern lock_t idle_lock;
extern void init(void);

void loop_menu(void)
{
    setup_refresh(menusequence);
    menusequence();
    showmenu();

    while (true) {
        switch (gamestate) {
        case GAME_RESET:
            init(); // Fall through to quit
        case GAME_RUNNING:
        case EXIT:
            goto quit_menu;
        default:
            wait_lock(idle_lock);
            menu_input();
            while (is_locked(idle_lock)) {
                idle();
            }
        }
    }

quit_menu:
    return;
}

void menusequence(void)
{
    draw_moon();
    draw_lander();
    draw_kitty();
    draw_kibbles();
    flipscreen();
}

void menu_input(void)
{
    union keyrow_0 k0;
    union keyrow_6 k6;
    static union keyrow_6 prev_k6 = {.raw = 0};

    scan_row_0(&k0);
    scan_row_6(&k6);

    if (k0.keys.K_EXIT) {
        gamestate = EXIT;
    }

    if (k6.raw && !prev_k6.raw && !lander.freedom.stopped) {
        gamestate = GAME_RUNNING;
    }

    if (k0.keys.K_F2) {
        gamestate = GAME_RESET;
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
