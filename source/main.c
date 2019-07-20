#include "bitmap.h"
#include "camera.h"
#include "game.h"
#include "lander.h"
#include "lock.h"
#include "kibble.h"
#include "kitty.h"
#include "menu.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "calc/display.h"
#include "calc/init.h"
#include "calc/ram.h"
#include "calc/random.h"
#include "calc/timer.h"

__at (TICKS_ADDRESS) unsigned int ticks;

void init(void);
void reset(void);
void timer_callback(void);

__at (FRAMELOCK_ADDRESS) lock_t frame_lock;
__at (IDLELOCK_ADDRESS) lock_t idle_lock;

const signed char * const loading[] = {
    "Creating lunar surface",
    "Waxing the moon",
    "Tranquilizing seas",
    "Herding cats"
};

void init(void)
{
    gamestate = START_MENU;
    printxy(0, 0, loading[random8()%4]);
    print("...");
    reset();
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
    restore_timer();
    savescores();
    return 0;
}

void reset(void)
{
    unsigned char i;
    ticks = 0;

    init_game();
    init_camera();
    init_kibbles();
    init_kitty();
    init_lander();
    init_moon();
    init_physics();
    init_calculator();

    init_lock(idle_lock);
    init_lock(frame_lock);
    setup_timer(timer_callback);
    setup_refresh(menusequence);
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
