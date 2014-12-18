#include "lock.h"
#include "ti86.h"

#define true 1
#define false 0

lock_t lcd;
lock_t main_lock;
unsigned int ticks;
unsigned int frames;
unsigned int dropped;
unsigned char running;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define WORLD_WIDTH 1025
#define LANDINGPAD_WIDTH 20
#define LANDER_WIDTH 8
#define LANDER_HEIGHT 8
unsigned int camera;
unsigned int moonwidth;
unsigned char moon[WORLD_WIDTH];
struct {
    unsigned short x;
    unsigned char y;
    struct {
        char x, y;
    } momentum;
    struct {
        char x, y;
    } acceleration;
    struct {
        unsigned char hp_stage   : 2; // Horizontal positive
        unsigned char hp_firing  : 1;
        unsigned char hn_stage   : 2; // Horizontal negative
        unsigned char hn_firing  : 1;
        unsigned char vp_stage   : 2; // Vertical positive (screen down)
        unsigned char vp_firing  : 1;
    } thrust;
    unsigned char *bitmap;
} lander;

unsigned char img_lander[LANDER_HEIGHT] = {
    0b00011000,
    0b01111110,
    0b11100111,
    0b01111110,
    0b00011000,
    0b00100100,
    0b01111110,
    0b11011011
};

#define THRUST_STAGES 4
unsigned char img_thrustleft[LANDER_HEIGHT][THRUST_STAGES] = {
    { 0b00000000, 0b00000000, 0b00000100, 0b00001110 },
    { 0b00000000, 0b00000110, 0b00011110, 0b00111011 },
    { 0b01010111, 0b01011101, 0b01111001, 0b11110001 },
    { 0b00000000, 0b00000110, 0b00011110, 0b00111011 },
    { 0b00000000, 0b00000000, 0b00000100, 0b00001110 },
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 }
};
unsigned char img_thrustdown[LANDER_HEIGHT][THRUST_STAGES] = {
    { 0b00011000, 0b00011000, 0b00011000, 0b00111100 },
    { 0b00111100, 0b00111100, 0b01100110, 0b01100110 },
    { 0b00011000, 0b00111100, 0b00100100, 0b11000011 },
    { 0b00000000, 0b00011000, 0b00011000, 0b11000011 },
    { 0b00010000, 0b00010000, 0b00001000, 0b01000010 },
    { 0b00001000, 0b00001000, 0b00010000, 0b00100100 },
    { 0b00000000, 0b00010000, 0b00001000, 0b00011000 },
    { 0b00010000, 0b00001000, 0b00010000, 0b00011000 }
};

unsigned char screenbuffer[1024];

#define EVENT_MAX 16
lock_t evt_lock;
typedef enum {
    NONE,
    QUIT,
    WORLD_REDRAW,
    IMPULSE_LEFT,
    IMPULSE_RIGHT,
    IMPULSE_DOWN,
    IMPULSE_UP,
    PHYSICS,
    READ_KEYS,
    CLEAR_SCREEN,
    CLEAR_BUFFER,
    SCREEN_COPY,
    DRAW_MOON,
    DRAW_LANDER,
    SIGNAL_GO
} event_t;
event_t events[EVENT_MAX];
unsigned char current_event;
void add_event(event_t);
event_t get_event(void);

void init(void);
void physics(void);
void perfcheck(void);
void read_keys(void);
void draw_moon(void);
void draw_lander(void);
void world_redraw(void);
void timer_callback(void);

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

void physics(void)
{
    unsigned int landerlimit;
    int scratch;

    lander.momentum.x += lander.acceleration.x;
    if (lander.momentum.x > 1) {
        if (lander.momentum.x > 25) {
            lander.momentum.x = 25;
        } else {
            lander.momentum.x--;
        }
    } else if (lander.momentum.x < -1) {
        if (lander.momentum.x < -25) {
            lander.momentum.x = -25;
        } else {
            lander.momentum.x++;
        }
    }

    landerlimit = WORLD_WIDTH-LANDER_WIDTH;
    scratch = (int)lander.x + (int)lander.momentum.x;
    if (scratch >= (int)landerlimit) {
        lander.x = landerlimit;
        lander.acceleration.x = 0;
        lander.momentum.x =0;
    } else if (scratch <= 0) {
        lander.acceleration.x = 0;
        lander.momentum.x = 0;
        lander.x = 0;
    } else {
        lander.x = (unsigned int)scratch;
    }

    scratch = lander.x-camera;
    if (scratch < LANDER_WIDTH) {
        scratch = lander.x-LANDER_WIDTH;
        if (scratch < 0) {
            camera = 0;
        } else {
            camera = scratch;
        }
    }

    scratch = camera+SCREEN_WIDTH-lander.x;
    if (scratch < 2*LANDER_WIDTH) {
        scratch = lander.x+2*LANDER_WIDTH-SCREEN_WIDTH;
        if (scratch+SCREEN_WIDTH > WORLD_WIDTH) {
            camera = WORLD_WIDTH-SCREEN_WIDTH;
        } else {
            camera = scratch;
        }
    }


    lander.momentum.y += lander.acceleration.y;
    if (lander.momentum.y > 25) {
        lander.momentum.y = 25;
    } else if (lander.momentum.y < -25) {
        lander.momentum.y = -25;
    }

    if (lander.momentum.y > 0) { //Screen down
        landerlimit = SCREEN_HEIGHT-LANDER_HEIGHT;
        lander.y += lander.momentum.y/4;
        if (lander.y > landerlimit) {
            lander.y = landerlimit;
            lander.momentum.y = 0;
            lander.acceleration.y = 0;
        }
    } else if (lander.momentum.y < 0) { //Screen up
        scratch = lander.y + lander.momentum.y/4;
        if (scratch < 0) {
            lander.y = 0;
            lander.momentum.y = 0;
            if (lander.acceleration.y < -1) {
                lander.acceleration.y = -1;
            }
        } else {
            lander.y = scratch;
        }
    }

    if (lander.momentum.x == 1 || lander.momentum.x == -1) {
        lander.momentum.x = 0;
    }
}

void read_keys(void)
{
    struct keyrow_6 k6;
    struct keyrow_0 k0;

    scan_row_6(&k6);
    scan_row_0(&k0);

    if (k6.K_RIGHT) {
        if (lander.acceleration.x < 10 && lander.x < WORLD_WIDTH-LANDER_WIDTH-1) {
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
        lander.acceleration.y = 1;
        lander.thrust.vp_firing = false;
        lander.thrust.vp_stage = 0;
    }
    
    if (k0.K_EXIT) {
        add_event(QUIT);
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
    if (landingpad+LANDINGPAD_WIDTH > WORLD_WIDTH) {
        for (i = landingpad; i < WORLD_WIDTH-landingpad; i++) {
            moon[i] = landingheight;
        }
    } else {
        for (i = landingpad; i < landingpad+LANDINGPAD_WIDTH; i++) {
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
    ticks = 0;
    camera = 0;
    frames = 0;
    dropped = 0;
    lander.y = 0;
    running = true;
    current_event = 0;
    moonwidth = WORLD_WIDTH;
    lander.x = SCREEN_WIDTH/2;
    lander.momentum.x = 0;
    lander.momentum.y = 0;
    lander.acceleration.x = 0;
    lander.acceleration.y = 1;
    lander.bitmap = img_lander;
    lander.thrust.hp_stage = 0;
    lander.thrust.hp_firing = 0;
    lander.thrust.hn_stage = 0;
    lander.thrust.hn_firing = 0;
    lander.thrust.vp_stage = 0;
    lander.thrust.vp_firing = 0;

    generate_moon();
    init_lock(evt_lock);
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

        if (ticks%8 == 0) {
            add_event(SIGNAL_GO);
        }
    }
}

void draw_vertical(unsigned short x, unsigned char height)
{
    unsigned char *i;
    unsigned char *screen = (unsigned char *)screenbuffer;
    unsigned int start = x/8+height*16;

    for (i = screen+start; i < screen+1024; i += 16) {
        *i |= (0x80 >> (x%8));
    }
}

/* Fast byte reversal from:
 http://www.retroprogramming.com/2014/01/fast-z80-bit-reversal.html
 First parameter in a, return value in l
*/
unsigned char reverse(unsigned char byte) __naked {
    byte;
    __asm
        ld l, a
        rlca
        rlca
        xor l
        and #0xaa
        xor l
        ld l, a
        rlca
        rlca
        rlca
        rrc l
        xor l
        and #0x66
        xor l
        ld l, a
        ret
    __endasm;
}

void draw_lander(void)
{
    unsigned char i, y;
    unsigned char x = lander.x-camera;
    unsigned char *screen = (unsigned char *)0xfc00;
    unsigned int start = x/8+lander.y*16;
    unsigned char *screenbyte = screen+start;
    unsigned char shift = x%8;
    unsigned char imgbyte;

    for (i = 0; i < LANDER_HEIGHT; i++) {
        *screenbyte |= (lander.bitmap[i] >> shift);
        *(screenbyte+1) |= (lander.bitmap[i] << (8-shift));
        screenbyte += 16;
    }

    if (lander.thrust.hp_firing) { // Draw left thruster
        x = lander.x-camera-LANDER_WIDTH;
        start = x/8+lander.y*16;
        screenbyte = screen+start;
        shift = x%8;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            imgbyte = img_thrustleft[i][lander.thrust.hp_stage];
            if (lander.x-camera >= LANDER_WIDTH) {
                *screenbyte |= (imgbyte >> shift);
            }
            if (lander.x-camera > 0) {
                *(screenbyte+1) |= (imgbyte << (8-shift));
            }
            screenbyte += 16;
        }
    }

    if (lander.thrust.hn_firing) { // Draw right thruster
        x = lander.x-camera+LANDER_WIDTH;
        start = x/8+lander.y*16;
        screenbyte = screen+start;
        shift = x%8;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            imgbyte = img_thrustleft[i][lander.thrust.hn_stage];
            imgbyte = reverse(imgbyte);
            if (x < SCREEN_WIDTH) {
                *screenbyte |= (imgbyte >> shift);
            }
            if (x < SCREEN_WIDTH-LANDER_WIDTH) {
                *(screenbyte+1) |= (imgbyte << (8-shift));
            }
            screenbyte += 16;
        }
    }

    if (lander.thrust.vp_firing) {
        x = lander.x-camera;
        y = lander.y+LANDER_HEIGHT;
        start = x/8+y*16;
        screenbyte = screen+start;
        shift = x%8;
        for (i = 0; i < LANDER_HEIGHT; i++) {
            if (i+y > SCREEN_HEIGHT) {
                break;
            }
            imgbyte = img_thrustdown[i][lander.thrust.vp_stage];
            *screenbyte ^= (imgbyte >> shift);
            *(screenbyte+1) ^= (imgbyte << (8-shift));
            screenbyte += 16;
        }
    }
}

void draw_moon(void)
{
    unsigned int i;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        draw_vertical(i, SCREEN_HEIGHT-moon[i+camera]);
    }
}

void world_redraw(void)
{
    wait_lock(lcd);
    clear_buffer();
    draw_moon();
    clear_screen();
    screencopy();
    draw_lander();
    frames++;
    drop_lock(lcd);
}

void perfcheck(void)
{
    if (frames == 1000) {
        running = false;
    }
}
