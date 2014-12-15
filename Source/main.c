#include "lock.h"
#include "ti86.h"

#define true 1
#define false 0

lock_t lcd_lock;
unsigned int ticks;
unsigned int frames;
unsigned int dropped;
unsigned char running;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define WORLD_WIDTH 257
#define LANDER_WIDTH 20
unsigned int camera;
unsigned int moonwidth;
unsigned char moon[WORLD_WIDTH];

#define EVENT_MAX 16
lock_t evt_lock;
typedef enum {
    NONE,
    QUIT,
    REDRAW
} event_t;
event_t events[EVENT_MAX];
unsigned char current_event;
void add_event(event_t);
event_t get_event(void);

void init(void);
void redraw(void);
void perfcheck(void);
void read_keys(void);
void timer_callback(void);

int main(void)
{
    init();
    while (running) {
        event_t e;

        read_keys();
        do {
            e = get_event();
            if (e == REDRAW) {
                redraw();
                frames++;
            } else if (e == QUIT) {
                running = false;
            }
            perfcheck();
        } while (e != NONE);

        if (running) {
            idle();
            if (ticks % 32 == 0) {
                add_event(REDRAW);
            }
        }
    }

    return 0;
}

void read_keys(void)
{
    struct keyrow_6 k6;
    struct keyrow_0 k0;
    scan_row_6(&k6);
    scan_row_0(&k0);
    if (k6.K_RIGHT) {
        if (camera+SCREEN_WIDTH < WORLD_WIDTH-1) {
            camera++;
        }
    }
    if (k6.K_LEFT) {
        if (camera > 0) {
            camera--;
        }
    }

    if (k0.K_EXIT) {
        running = false;
    }
}

void generate_moon(void)
{
    unsigned int i, j;
    unsigned int stride;
    unsigned int left, right;
    unsigned int leftheight, rightheight;
    unsigned int distortion;
    unsigned char rand;
    unsigned short landingpad;
    unsigned char landingheight;

    for (i = 0; i < WORLD_WIDTH; i++) {
        moon[i] = 0;
    }

    moon[0] = SCREEN_HEIGHT/8;
    moon[WORLD_WIDTH-1] = SCREEN_HEIGHT/8;

    landingpad = random16()%(WORLD_WIDTH-1);
    landingheight = random8()%(SCREEN_HEIGHT/2);
    if (landingpad > WORLD_WIDTH-21) {
        landingpad = WORLD_WIDTH-21;
    }
    if (landingheight < 10) {
        landingheight = 10;
    }
    if (landingpad+LANDER_WIDTH > WORLD_WIDTH) {
        for (i = landingpad; i < WORLD_WIDTH-landingpad; i++) {
            moon[i] = landingheight;
        }
    } else {
        for (i = landingpad; i < landingpad+LANDER_WIDTH; i++) {
            moon[i] = landingheight;
        }
    }

    i = (WORLD_WIDTH-1)/2;
    stride = WORLD_WIDTH-1;
    while (i > 0) {
        for (j = i; j < WORLD_WIDTH-1; j += stride) {
            if (moon[j] != 0) {
                continue;
            }
            left = j-i;
            right = j+i;
            leftheight = moon[left];
            rightheight = moon[right];
            rand = random8() & 0x01;
            if (rand == 1) {
                distortion = (leftheight+rightheight)/2+(i&0x1f);
            } else {
                distortion = (leftheight+rightheight)/2-(i&0x1f);
            }
            if (distortion > SCREEN_HEIGHT-20) {
                distortion = SCREEN_HEIGHT-20;
            }
            if (distortion < 2) {
                distortion = 2;
            }
            moon[j] = (unsigned char)distortion;
        }
        stride = i;
        i /= 2;
    }
}

void add_event(event_t evt)
{
    wait_lock(evt_lock);
    events[current_event] = evt;
    current_event++;
    if (current_event == EVENT_MAX) {
        current_event = 0;
    }
    drop_lock(evt_lock);
}

event_t get_event(void)
{
    event_t returned;
    wait_lock(evt_lock);
    if (current_event == 0) {
        returned = NONE;
    } else {
        current_event--;
        returned = events[current_event];
        events[current_event] = NONE;
    }
    drop_lock(evt_lock);
    return returned;
}

void init(void)
{
    camera = 0;
    frames = 0;
    dropped = 0;
    running = true;
    current_event = 0;
    moonwidth = WORLD_WIDTH;

    generate_moon();
    init_lock(lcd_lock);
    init_lock(evt_lock);
    add_event(REDRAW);
    setup_timer(&timer_callback);
}

void timer_callback(void)
{
    ticks++;
}

void draw_vertical(unsigned short x, unsigned char height)
{
    unsigned char *i;
    unsigned char *screen = (unsigned char *)0xfc00;
    unsigned int start = x/8+height*16;

    for (i = screen+start; i > screen; i += 16) {
        *i |= (0x80 >> (x%8));
    }
}

void draw_moon(void)
{
    unsigned int i;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        draw_vertical(i, SCREEN_HEIGHT-moon[i+camera]);
    }
}

void redraw(void)
{
    wait_lock(lcd_lock);
    clear_screen();
    draw_moon();
    drop_lock(lcd_lock);
}

void perfcheck(void)
{
    if (frames == 100) {
        running = false;
    }
}
