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
        ld hl, #ASAPVAR
        rst #0x20
        rst #0x10
        xor a
        ld hl,#_crashes-0xd744
        add hl, de
        adc a, b
        call #SET_ABS_DEST_ADDR
        xor a
        ld hl, #_crashes
        call #SET_ABS_SRC_ADDR
        ld hl, #4
        call MM_LDIR_SET_SIZE
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
        di
        ld bc, #3
        ld hl, #jump_timer
        ld de, #0xadad
        ldir

        ld hl, #0xae00
        ld de, #0xae01
        ld (hl), #0xad
        ld bc, #256
        ldir
        ld a, #0xae
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
__sfr __at 0x00 lcdptr; // Port that tells the LCD where to look
unsigned char *screenbuffer = SCREENBUF1_ADDRESS; // Off-screen buffer

void clear_screen(void) __naked
{
    __asm
        ld hl, #SCREENBUF0_ADDRESS
        xor a
        ld (hl), a
        ld bc, #0x03ff
        ld de, #SCREENBUF0_ADDRESS+1
        ldir
        ret
    __endasm;
}

// World x, not screen x.
void draw_live_sprite(const unsigned char animation[8][4],
        unsigned char frame, unsigned short x, unsigned char y,
        char offset, char mode)
{
    unsigned int yoffset = y<<4;

    unsigned char i, rshift, lshift, imgbyte, *screenbyte, screenx;
    unsigned int start;
    int scratch;

    if (x > camera+SCREEN_WIDTH || x+8 < camera) {
        return;
    }

    scratch = x-camera+offset;
    screenx = (unsigned char)scratch;

    rshift = screenx&0x07;
    lshift = 8-rshift;
    start = (screenx>>3)+yoffset;

    screenbyte = screenbuffer+start;
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
            screenbyte += 16;
        }
    } else if (scratch < 0 && scratch > -8) {
        screenbyte = screenbuffer+yoffset;
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            if (mode == OR) {
                *(screenbyte) |= (imgbyte << lshift);
            } else {
                *(screenbyte) ^= (imgbyte << lshift);
            }
            screenbyte += 16;
        }
    } else if (scratch < SCREEN_WIDTH && scratch+8 > SCREEN_WIDTH) {
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            if (mode == OR) {
                *screenbyte |= (imgbyte >> rshift);
            } else {
                *screenbyte ^= (imgbyte >> rshift);
            }
            screenbyte += 16;
        }
    }
}

void draw_moon(void) __naked
{
    __asm
        push ix ;// shift amount
        ;//push bc ;// scr
        ;//push de ;// pre
        ;//push hl ;// temps
        ;//push af ;// math

        ;// Fast clear upper portions of screen
        ;// that are always zero: top 20 rows (0-19)
        ld hl, (#_screenbuffer)
        ld (hl), #0x00
        ld de, (#_screenbuffer)
        inc de
        ld bc, #319 ;// 20 rows*16 bytes/row, minus the one byte already cleared
        ldir

        ;// Fast fill lower portions of screen
        ;// that are always 0xff: bottom 8 rows (56-63)
        ld hl, (#_screenbuffer)
        ld de, #896 ;// 56th row
        add hl, de
        ld d, h
        ld e, l
        inc de
        ld (hl), #0xff
        ld bc, #127
        ldir


        ld hl, #_camera

        ld a, #0x07
        and a, (hl)
        jp z, fdm_setup_loop_shift0
        dec a
        jp z, fdm_setup_loop_shift1
        dec a
        jp z, fdm_setup_loop_shift2
        dec a
        jp z, fdm_setup_loop_shift3
        dec a
        jp z, fdm_setup_loop_shift4
        dec a
        jp z, fdm_setup_loop_shift5
        dec a
        jp z, fdm_setup_loop_shift6
        dec a
        jp z, fdm_setup_loop_shift7
        jp fdm_setup_loop_done
    fdm_setup_loop_shift7:
        ld ix, #fdm_loop_shift7
        jp fdm_setup_loop_done
    fdm_setup_loop_shift6:
        ld ix, #fdm_loop_shift6
        jp fdm_setup_loop_done
    fdm_setup_loop_shift5:
        ld ix, #fdm_loop_shift5
        jp fdm_setup_loop_done
    fdm_setup_loop_shift4:
        ld ix, #fdm_loop_shift4
        jp fdm_setup_loop_done
    fdm_setup_loop_shift3:
        ld ix, #fdm_loop_shift3
        jp fdm_setup_loop_done
    fdm_setup_loop_shift2:
        ld ix, #fdm_loop_shift2
        jp fdm_setup_loop_done
    fdm_setup_loop_shift1:
        ld ix, #fdm_loop_shift1
        jp fdm_setup_loop_done
    fdm_setup_loop_shift0:
        ld ix, #fdm_loop_shift0
        ;jp fdm_setup_loop_done
    fdm_setup_loop_done:

        ld hl, #_camera
        ld e, (hl)
        inc hl
        ld d, (hl)
        srl d
        rr e
        srl d
        rr e
        srl d
        rr e ;// camera/8 in de

        ld hl, #_prerendered
        ld bc, #2560 ;// 20 rows * 128 bytes/row from prerendered
        add hl, bc
        add hl, de
        ex de, hl ;// de = prerendered+prex

        ld bc, #320
        ld hl, (#_screenbuffer)
        add hl, bc
        ld b, h
        ld c, l ;// bc = screen offset that actually needs to get drawn

    fdm_loop: ;// Want (bc) = (de)

        ld a, (de) ;// Put (de) into hl so we can shift with add hl, hl
        ld h, a ;// No little-endian business! The bytes are already in order!
        inc de
        ld a, (de)
        ld l, a
        jp (ix)

    fdm_loop_shift7: ;// 22 beats 77
        ;// ld a, l
        srl h
        rra
        jp fdm_loop_shiftdone
    fdm_loop_shift6: ;// 32 beats 66
        ;// ld a, l
        srl h
        rra
        srl h
        rra
        jp fdm_loop_shiftdone
    fdm_loop_shift5: ;// 46 beats 55+4
        ;// ld a, l
        srl h
        rra
        srl h
        rra
        srl h
        rra
        jp fdm_loop_shiftdone
    fdm_loop_shift4: ;// 44+4
        add hl, hl
    fdm_loop_shift3: ;// 33+4
        add hl, hl
    fdm_loop_shift2: ;// 22+4
        add hl, hl
    fdm_loop_shift1: ;// 11+4
        add hl, hl
    fdm_loop_shift0:
        ld a, h
    fdm_loop_shiftdone:
        ld (bc), a

        ;// inc de Aldready taken care of
        inc bc

        ld a, #0x80
        cp c
        jr nz, fdm_loop_cont
        ld a, #0xff
        cp b
        jp z, fdm_loop_done
        ld a, #0xcd
        cp b
        jp z, fdm_loop_done

    fdm_loop_cont:
        ld a, #0x0f
        and c ;// multiples of 16: de (pre) needs to make a big jump to next row. use _at!
        jp nz, fdm_loop

        ld a, e
        add a, #0x70 ;// 128 to next row, (later) minus 16 for the loop incs
        ld e, a
        ld a, #0x00
        adc a, d
        ld d, a
        jp fdm_loop
    fdm_loop_done:

        ;//pop af
        ;//pop hl
        ;//pop de
        ;//pop bc
        pop ix

        ret
    __endasm;
}

void draw_status(void)
{
    unsigned char i;
    unsigned char x = lander.fuel>>4;
    unsigned char shift = (lander.fuel>>1)&0x07;

    if (lander.fuel == 0) {
        return;
    }
    for (i = 0; i < x; i++) {
        *(screenbuffer+i) = 0xff;
    }
    *(screenbuffer+i) |= ((char)0x80 >> shift);
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
    if (screenbuffer == (unsigned char *)SCREENBUF0_ADDRESS) {
        refresh_sequence();
    }
    __asm
        ld l, 4(ix)
        ld h, 5(ix)
        call #VPUTS
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
    if (screenbuffer == (unsigned char *)SCREENBUF0_ADDRESS) {
        screenbuffer = (unsigned char *)SCREENBUF1_ADDRESS;
        lcdptr = 0x3c;
    } else {
        screenbuffer = (unsigned char *)SCREENBUF0_ADDRESS;
        lcdptr = 0x0a;
    }
}

// Internal display interface
void prerender(void)
{
    unsigned int j;
    unsigned char i;
    unsigned char height;

    __asm
        ld hl, #_prerendered
        ld (hl), #0
        ld de, #_prerendered+1
        ld bc, #8191
        ldir
    __endasm;

    for (j = 0; j < (MOON_WIDTH-1); j++) {
        height = moon[j];
        for (i = height; i < SCREEN_HEIGHT; i++) {
            prerendered[i][j>>3] |= (0x80 >> (j&0x07));
        }
    }
}

void save_graphbuffer(void) __naked
{
    __asm
        ld bc, #0x0400
        ld de, #BACKUPGRAPH_ADDRESS
        ld hl, #SCREENBUF1_ADDRESS
        ldir
        ret
    __endasm;
}

void restore_graphbuffer(void) __naked
{
    lcdptr = 0x3c;
    __asm
        ld bc, #0x0400
        ld de, #SCREENBUF1_ADDRESS
        ld hl, #BACKUPGRAPH_ADDRESS
        ldir
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
