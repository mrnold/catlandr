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
unsigned char gamestate;

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
    gamestate = START_MENU;
    printxy(0, 0, loading[random8()%4]);
    reset();
}

void loop_menu(void)
{
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

int main(void)
{
    save_graphbuffer();
    clear_screen();
    init();
    while (true) {
        switch (gamestate) {
            case DONE_STRANDED:
            case DONE_CRASHED:
            case DONE_LANDED:
            case GAME_RESET:
            case START_MENU:
                loop_menu(); break;
            case GAME_RUNNING:
                loop_game(); break;
            case EXIT:
                goto quit_program;
        }
    }

quit_program:
    clear_screen();
    restore_graphbuffer();
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
    flipscreen();
    force_call();
}

void menusequence(void)
{
    draw_moon();
    draw_lander();
    draw_kitty();
    draw_kibbles();
    flipscreen();
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
        if (gamestate == GAME_RUNNING && is_locked(frame_lock)) {
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
    if (screenbuffer == (unsigned char *)0xfc00) {
        menusequence();
    }
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
