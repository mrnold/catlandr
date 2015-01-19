#include "physics.h"
#include "camera.h"
#include "lander.h"
#include "moon.h"
#include "ti86.h"

static unsigned char r8;
static unsigned short r16;
static void (*callback_function)(void);

__at (0x8100) unsigned char prerendered[SCREEN_HEIGHT][(MOON_WIDTH-1)/8];
__at (0xca00) unsigned char screenbuffer1[SCREEN_HEIGHT][SCREEN_WIDTH/8];
__at (0xfc00) unsigned char screenbuffer2[SCREEN_HEIGHT][SCREEN_WIDTH/8];
__at (0xb000) unsigned char *backupgraph;

__at (0xc37c) unsigned char textcol;
__at (0xc37d) unsigned char textrow;

unsigned char *screenbuffer = 0xca00;

__sfr __at 0x00 lcdptr;

void printxy(unsigned char col, unsigned char row, const char * const string)
{
    string;
    textcol = col;
    textrow = row;
    __asm
        push hl
        ld l, 6(ix)
        ld h, 7(ix)
        call #0x4aa5
        pop hl
    __endasm;
}

void idle(void) __naked
{
    __asm
        ei
        halt
        ret
    __endasm;
}

void flipscreen(void)
{
    if (screenbuffer == (unsigned char *)0xfc00) {
        screenbuffer = (unsigned char *)0xca00;
        lcdptr = 0x3c;
    } else {
        screenbuffer = (unsigned char *)0xfc00;
        lcdptr = 0x0a;
    }
}

void save_graphbuffer(void) __naked
{
    __asm
        push af
        push bc
        push de
        push hl
        ld bc, #0x0400
        ld de, #0xb000
        ld hl, #0xca00
        ldir
        pop hl
        pop de
        pop bc
        pop af
        ret
    __endasm;
}

void restore_graphbuffer(void) __naked
{
    lcdptr = 0x3c;
    __asm
        push af
        push bc
        push de
        push hl
        ld bc, #0x0400
        ld de, #0xca00
        ld hl, #0xb000
        ldir
        pop hl
        pop de
        pop bc
        pop af
        ret
    __endasm;
}

void clear_screen(void) __naked
{
    __asm
        push af
        push bc
        push de
        push hl
        ld hl, #0xfc00
        xor a
        ld (hl), a
        ld bc, #0x03ff
        ld de, #0xfc01
        ldir
        pop hl
        pop de
        pop bc
        pop af
        ret
    __endasm;
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

void setup_timer(void (*callback)(void))
{
    callback_function = callback;
    set_timer();
}

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

unsigned char random8(void)
{
    __asm
        push af
        ld a, r
        ld (#_r8), a
        pop af
    __endasm;

    return r8;
}

unsigned short random16(void)
{
    __asm
        push af
        ld a, r
        ld (#_r16), a
        ld a, r
        ld (#_r16+1), a
        pop af
    __endasm;

    return r16;
}

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

void screencopy(void) __naked
{
    __asm
        push bc
        push de
        push hl
        ld hl, (#_screenbuffer)
        ld de, #0xfc00
        ld bc, #1024
        ldir
        pop hl
        pop de
        pop bc
        ret
    __endasm;
}

void prerender(void)
{
    unsigned int j;
    unsigned char i;
    unsigned char height;

    __asm
        push hl
        push de
        push bc

        ld hl, #_prerendered
        ld (hl), #0
        ld de, #_prerendered+1
        ld bc, #8191
        ldir

        pop bc
        pop de
        pop hl
    __endasm;

    for (j = 0; j < (MOON_WIDTH-1); j++) {
        height = moon[j];
        for (i = height; i < SCREEN_HEIGHT; i++) {
            prerendered[i][j>>3] |= (0x80 >> (j&0x07));
        }
    }
}


void draw_moon(void) __naked
{
    __asm
        push ix ;// shift amount
        push bc ;// scr
        push de ;// pre
        push hl ;// temps
        push af ;// math

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

        pop af
        pop hl
        pop de
        pop bc
        pop ix

        ret
    __endasm;
}

/*
 * The draw_moon graveyard. Things I tried that were not fast enough.

void draw_vertical(unsigned short x, unsigned char height)
{
    unsigned char *i;
    unsigned char *screen = (unsigned char *)screenbuffer;
    unsigned int start = x/8+height*16;

    for (i = screen+start; i < screen+1024; i += 16) {
        *i |= (0x80 >> (x%8));
    }
}

void draw_moon(void)
{
    unsigned int i;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        draw_vertical(i, moon[i+camera]);
    }
}

void draw_moon(void)
{
    for (x = 0; x < SCREEN_WIDTH/8; x++) {
        for (y = 0; y < SCREEN_HEIGHT; y++) {
            screenbuffer[y][x]  = prerendered[y][prex] << shift1;
            screenbuffer[y][x] |= prerendered[y][prex+1] >> shift2;
        }
        prex++;
    }
}

void draw_moon(void)
{
    unsigned int prex = camera/8;
    unsigned int shift1 = camera%8;
    unsigned int shift2 = 8-shift1;

    unsigned char *scr = (unsigned char *)screenbuffer;
    unsigned char *pre = (unsigned char *)prerendered + prex;
    while (scr < ((unsigned char *)screenbuffer)+1024) {
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        *(scr)  = *(pre) << shift1; pre++;
        *(scr) |= *(pre) >> shift2; scr++;
        pre += (MOON_WIDTH-1)/8-16;
    }
}

*/

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

// Only for sprites guaranteed to be fully onscreen
void draw_static_sprite_noclip(const unsigned char image[8], unsigned short x, unsigned char y)
{
    unsigned int yoffset = y<<4;
    unsigned char i, rshift, lshift, screenx;
    unsigned char *screenbyte;
    unsigned int start;

    screenx = x-camera;
    rshift = screenx&0x07;
    lshift = 8-rshift;
    start = (screenx>>3)+yoffset;
    screenbyte = screenbuffer+start;
    for (i = 0; i < 8; i++) {
        *screenbyte |= (image[i] >> rshift);
        *(screenbyte+1) |= (image[i] << lshift);
        screenbyte += 16;
    }
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

