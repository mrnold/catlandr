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
__at (PRERENDER_ADDRESS) unsigned char prerendered[SCREEN_HEIGHT][(MOON_WIDTH-1)/8];
__at (MOON_ADDRESS) unsigned char moon[MOON_WIDTH];
__at (KIBBLES_ADDRESS) struct kibble_t kibbles[KIBBLE_MAX];
__at (LANDER_ADDRESS) struct lander_t lander;
__at (KITTY_ADDRESS) struct kitty_t kitty;
__at (FRAMES_ADDRESS) unsigned int frames;
__at (DROPPED_ADDRESS) unsigned int dropped;
__at (BACKUPGRAPH_ADDRESS) unsigned char *backupgraph;
__at (TEXTCOL_ADDRESS) unsigned char textcol;
__at (TEXTROW_ADDRESS) unsigned char textrow;

void memset(void *ptr, unsigned char value, unsigned short count)
{
    ptr, value, count;
    __asm
        ld l, 4(ix)
        ld h, 5(ix)
        ld c, 7(ix)
        ld b, 8(ix)
        ld a, 6(ix)
        ld (hl), a
        ld e, l
        ld d, h
        inc de
        ldir
    __endasm;
}

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
        di
        ex af, af'
        exx
        ld hl, #timer_isr_exit
        push hl
        ld hl, (#_callback_function)
        jp (hl)
    timer_isr_exit:
        in a, (3) ;// TI-86 boilerplate
        and #0x01 ;// Clear ON key status
        add a, #0x09
        out (3), a
        ld a, #0x0b
        out (3), a
        ex af, af'
        exx
        ei
        reti
    __endasm;
}
#pragma preproc_asm +

// Internal timer interface
void set_timer(void) __naked
{
    __asm
        ret
    __endasm;
}
// End of timer interface -------------------------------------------

// Display interface ------------------------------------------------
void (*refresh)(void);
__sfr __at 0x00 lcdptr; // Port that tells the LCD where to look
unsigned char *screenbuffer = SCREENBUF1_ADDRESS; // Off-screen buffer

void clear_screen(void) __naked
{
    __asm
        ret
    __endasm;
}

// World x, not screen x.
void draw_live_sprite(const unsigned char animation[8][4],
        unsigned char frame, unsigned short x, unsigned char y,
        char offset, char mode)
{
    return;
}

void draw_moon(void) __naked
{
    __asm
        ret
    __endasm;
}

void draw_status(void) __naked
{
    __asm__("ret");
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
        if (screenbuffer == (unsigned char *)SCREENBUF0_ADDRESS) {
            refresh_sequence();
        }
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
    return;
}

// Internal display interface
void prerender(void) __naked
{
    __asm__("ret");
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
