#include "bitmap.h"
#include "game.h"
#include "kibble.h"
#include "kitty.h"
#include "lander.h"
#include "lock.h"
#include "menu.h"
#include "misc.h"
#include "moon.h"
#include "calc/display.h"
#include "calc/input.h"
#include "calc/ram.h"
#include "calc/random.h"
#include "calc/timer.h"

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
    updatescreen();
}

void menu_input(void)
{
    union keyrow_0 k0;
    union keyrow_6 k6;
    static __at (PREVKEY6_ADDRESS) union keyrow_6 prev_k6 = {.raw = 0};

    scan_row_0(&k0);
    scan_row_6(&k6);

    if (k0.K_QUIT) {
        gamestate = EXIT;
    }

    if (k6.raw && !prev_k6.raw && !lander.freedom.stopped) {
        gamestate = GAME_RUNNING;
    }

    if (k0.K_MOON) {
        gamestate = GAME_RESET;
    }

    prev_k6.raw = k6.raw;
}

void showmenu(void)
{
    printhelp();

    if (lander.freedom.stopped) {
        printscore();
    }

    if (lander.freedom.stuck.crashed) {
        printdestroyed();
    } else if (lander.freedom.stuck.stranded) {
        printnogas();
    } else if (lander.freedom.stuck.landed) {
        printvictory();
    } else {
        printgamestart();
    }
}
