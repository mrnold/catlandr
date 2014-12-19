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

unsigned int camera;
unsigned int previouscamera;

void init(void);
void perfcheck(void);
void read_keys(void);
void timer_callback(void);

lock_t main_lock;
lock_t lcd;

int main(void)
{
    init();
    while (running) {
        event_t e;

        wait_lock(main_lock);
        do {
            e = get_event();
            if (e == SIGNAL_GO) {
                read_keys();
                physics();
                collisions();
                clear_buffer();
                draw_moon();
                clear_screen();
                screencopy();
                draw_lander();
                frames++;
            } else if (e == IMPULSE_RIGHT) {
                if (lander.acceleration.x < 10 && lander.x < SCREEN_WIDTH-LANDER_WIDTH) {
                    lander.acceleration.x++;
                }
            } else if (e == IMPULSE_LEFT) {
                if (lander.acceleration.x > -10 && lander.x > 0) {
                    lander.acceleration.x--;
                }
            } else if (e == IMPULSE_UP) {
                if (lander.acceleration.y > -10 && lander.y > 0) {
                    lander.acceleration.y--;
                }
            } else if (e == IMPULSE_DOWN) {
                if (lander.acceleration.y < 10 && lander.y < SCREEN_HEIGHT-LANDER_HEIGHT) {
                    lander.acceleration.y++;
                }
            } else if (e == READ_KEYS) {
                read_keys();
            } else if (e == WORLD_REDRAW) {
                world_redraw();
            } else if (e == PHYSICS) {
                physics();
            } else if (e == CLEAR_SCREEN) {
                clear_screen();
            } else if (e == CLEAR_BUFFER) {
                clear_buffer();
            } else if (e == DRAW_MOON) {
                draw_moon();
            } else if (e == SCREEN_COPY) {
                screencopy();
            } else if (e == DRAW_LANDER) {
                draw_lander();
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

void read_keys(void)
{
    struct keyrow_6 k6;
    struct keyrow_0 k0;

    if (!lander.crashed && !lander.landed) {
        scan_row_6(&k6);
    } else {
        k6.K_RIGHT = false;
        k6.K_LEFT = false;
        k6.K_UP = false;
    }
    scan_row_0(&k0);

    if (k6.K_RIGHT) {
        if (lander.acceleration.x < 10 && lander.x < MOON_WIDTH-LANDER_WIDTH-1) {
            lander.acceleration.x++;
        }
        lander.thrust.hp_firing = true;
        lander.thrust.hp_stage++;
    } else {
        lander.thrust.hp_firing = false;
        lander.thrust.hp_stage = 0;
    }
    if (k6.K_LEFT) {
        if (lander.acceleration.x > -10 && lander.x > 0) {
            lander.acceleration.x--;
        }
        lander.thrust.hn_firing = true;
        lander.thrust.hn_stage++;
    } else {
        lander.thrust.hn_firing = false;
        lander.thrust.hn_stage = 0;
    }

    if (k6.K_LEFT && k6.K_RIGHT) {
        lander.acceleration.x = 0;
    }
    if (!k6.K_LEFT && !k6.K_RIGHT) {
        lander.acceleration.x = 0;
    }

    if (k6.K_UP) {
        if (lander.acceleration.y > -10 && lander.y > 0) {
            lander.acceleration.y--;
        }
        lander.thrust.vp_firing = true;
        lander.thrust.vp_stage++;
    } else {
        if (!lander.crashed && !lander.perched) {
            lander.acceleration.y = 1;
        }
        lander.thrust.vp_firing = false;
        lander.thrust.vp_stage = 0;
    }
    
    if (k0.K_EXIT) {
        add_event(QUIT);
    }
}

void init(void)
{
    ticks = 0;
    camera = 0;
    frames = 0;
    dropped = 0;
    running = true;

    init_event();
    init_lander();
    init_moon();
    init_physics();

    init_lock(lcd);
    init_lock(main_lock);
    setup_timer(&timer_callback);
}

void timer_callback(void)
{
    if (is_locked(main_lock)) {
        dropped++;
    } else {
        ticks++;

        if (ticks%16 == 0) {
            add_event(SIGNAL_GO);
        }
    }
}

void perfcheck(void)
{
    if (frames == 1000) {
        running = false;
    }
}
