#include "bitmap.h"
#include "camera.h"
#include "kibble.h"
#include "input.h"
#include "lander.h"
#include "moon.h"
#include "physics.h"

#include "calc/display.h"
#include "calc/init.h"
#include "calc/ram.h"
#include "calc/random.h"
#include "calc/timer.h"
#include "display.h"
#include "init.h"
#include "ram.h"
#include "random.h"
#include "timer.h"

// RAM interface - specifies where all the big stuff lives ----------
__at (MOON_ADDRESS) unsigned char moon[MOON_WIDTH];
__at (KIBBLES_ADDRESS) struct kibble_t kibbles[KIBBLE_MAX];
__at (LANDER_ADDRESS) struct lander_t lander;
__at (KITTY_ADDRESS) struct kitty_t kitty;
__at (FRAMES_ADDRESS) unsigned int frames;
__at (DROPPED_ADDRESS) unsigned int dropped;
__at (TEXTCOL_ADDRESS) unsigned char textcol;
__at (TEXTROW_ADDRESS) unsigned char textrow;
__at (SCREENBUF_ADDRESS) unsigned char screenbuffer;

void savescores(void) __naked
{
    __asm
        ret
    __endasm;
}
// End of RAM interface ----------------------------------------------


// Timer interface --------------------------------------------------
static void (*callback_function)(void);
void setup_timer(void (*callback)(void))
{
    callback_function = callback;
    set_timer();
}

// Silence compiler warning on "af'"
#pragma preproc_asm -
void timer_isr(void) __naked
{
    __asm
        ex af, af'
        exx
        ld hl, #timer_isr_exit
        push hl
        ld hl, (#_callback_function)
        jp (hl)
    timer_isr_exit:
        ex af, af'
        exx
        reti
    __endasm;
}
#pragma preproc_asm +

// Internal timer interface
void set_timer(void) __naked
{
    __asm
        di
        ld bc, #3
        ld hl, #jump_timer
        ld de, #0x9a9a
        ldir

        ld hl, #0x9900
        ld de, #0x9901
        ld (hl), #0x9a
        ld bc, #256
        ldir
        ld a, #0x99
        ld i, a
        im 2
        ei
        ret
    jump_timer::
        jp _timer_isr
    __endasm;
}
// End of timer interface -------------------------------------------

// Display interface ------------------------------------------------
void (*refresh)(void);

void clear_screen(void) __naked
{
    __asm
        ld hl, #SCREENBUF_ADDRESS
        xor a
        ld (hl), a
        ld bc, #767
        ld de, #SCREENBUF_ADDRESS+1
        ldir
        ret
    __endasm;
}

// World x, not screen x.
void draw_live_sprite(const unsigned char animation[8][4],
        unsigned char frame, unsigned short x, unsigned char y,
        char offset, char mode)
{
    unsigned int yoffset = y*12;

    unsigned char i, rshift, lshift, imgbyte, *screenbyte, screenx;
    unsigned int start;
    int scratch;

    if (x > camera+SCREEN_WIDTH || x+8 < camera) {
        return;
    }

    frame &= (ANIMATION_STAGES-1);

    scratch = x-camera+offset;
    screenx = (unsigned char)scratch;

    rshift = screenx&0x07;
    lshift = 8-rshift;
    start = (screenx>>3)+yoffset;

    screenbyte = &screenbuffer+start;
    if (scratch >= 0 && scratch <= SCREEN_WIDTH-8) {
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            if (mode == OR) {
                *screenbyte |= (imgbyte >> rshift);
                *(screenbyte+1) |= (imgbyte << lshift);
            } else {
                *screenbyte ^= (imgbyte >> rshift);
                *(screenbyte+1) ^= (imgbyte << lshift);
            }
            screenbyte += 12;
        }
    } else if (scratch < 0 && scratch > -8) {
        screenbyte = &screenbuffer+yoffset;
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            if (mode == OR) {
                *(screenbyte) |= (imgbyte << lshift);
            } else {
                *(screenbyte) ^= (imgbyte << lshift);
            }
            screenbyte += 12;
        }
    } else if (scratch < SCREEN_WIDTH && scratch+8 > SCREEN_WIDTH) {
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            if (mode == OR) {
                *screenbyte |= (imgbyte >> rshift);
            } else {
                *screenbyte ^= (imgbyte >> rshift);
            }
            screenbyte += 12;
        }
    }
}

void draw_vertical(unsigned short x, unsigned char height)
{
    unsigned char *i;
    unsigned char *screen = (unsigned char *)0x9340;
    unsigned int start = x/8+height*12;

    for (i = screen+start; i < (unsigned char *)0x9640; i += 12) {
        *i |= (0x80 >> (x%8));
    }
}

void draw_moon(void)
{
    unsigned int i;
    clear_screen();
    for (i = 0; i < SCREEN_WIDTH; i++) {
        draw_vertical(i, moon[i+camera]);
    }
}

void draw_status(void)
{
    unsigned char i, x;
    unsigned char shift = lander.fuel&0x07;

    if (lander.fuel == 0) {
        return;
    }
    for (i = 0, x = 0; x < (lander.fuel&0xf8); i++, x += 8) {
        *(&screenbuffer+i) = 0xff;
    }
    *(&screenbuffer+i) |= ((char)0x80 >> shift);
}

static void refresh_sequence(void) __naked;
void printxy(unsigned char col, unsigned char row, const char * const string)
{
    string;
    textcol = col;
    textrow = row;
    print(string);
}

void print(const char * const string)
{
    string;
    __asm
        ld l, 4(ix)
        ld h, 5(ix)
        rst #0x28
        .dw #VPUTS
    __endasm;
}

void printnumxy(unsigned char col, unsigned char row, unsigned int number)
{
    number;
    textcol = col;
    textrow = row;
    printnum(number);
}

static void printdigits(unsigned int number);
void printnum(unsigned int number)
{
    if (number == 0) {
        print("0");
    } else {
        printdigits(number);
    }
}

static void printdigits(unsigned int number)
{
    number;
    __asm
        ld l, 4(ix)
        ld h, 5(ix)
        ld b, #5
    printnumxygetdigits:
        call #DIVHLBY10 ;// _divHLby10, remainder in A
        push af
        djnz printnumxygetdigits
        ld b, #5
    printnumxystripzero:
        pop af
        cp #0
        jp nz, printnumxyprintdigitsskip
        djnz printnumxystripzero
    printnumxyprintdigits:
        pop af
    printnumxyprintdigitsskip:
        add #0x30    ;// Add ASCII numbers offset
        call #VPUTMAP
        djnz printnumxyprintdigits
    __endasm;
}

void updatescreen(void)
{
    __asm
        rst #0x28
        .dw #GRBUFCPY
    __endasm;
}

void save_graphbuffer(void) __naked
{
    __asm
        ret
    __endasm;
}

void restore_graphbuffer(void) __naked
{
    __asm
        ret
    __endasm;
}

static void refresh_sequence(void) __naked
{
    __asm
        ld hl, #refresh_sequence_done
        push hl
        ld hl, (#_refresh)
        jp (hl)
    refresh_sequence_done:
        ret
    __endasm;
}
// End of display interface -----------------------------------------

// Random number interface -------------------------------------------
unsigned char random8(void) __naked
{
    __asm
        ld a, r
        ld l, a
        ret
    __endasm;
}

unsigned short random16(void) __naked
{
    __asm
        ld a, r
        ld l, a
        ld a, r
        ld h, a
        ret
    __endasm;
}
// End of random number interface -----------------------------------

// Keyboard input interface
__sfr __at 0x01 keyport;
void scan_row_6(union keyrow_6 *k6)
{
    keyport = 0xfe;
    k6->raw = ~keyport;
}
void scan_row_0(union keyrow_0 *k0)
{
    keyport = 0xbf;
    k0->raw = ~keyport;
}
// End of keyboard input interface ----------------------------------
