#include "bitmap.h"
#include "game.h"
#include "lander.h"
#include "lock.h"
#include "kibble.h"
#include "kitty.h"
#include "menu.h"
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
