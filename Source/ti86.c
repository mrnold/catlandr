#include "lander.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"

static unsigned char r8;
static unsigned short r16;
static void (*callback_function)(void);

__at (0x8100) unsigned char prerendered[SCREEN_HEIGHT][(MOON_WIDTH-1)/8];
__at (0xb000) unsigned char screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH/8];

__at (0xc37c) unsigned char textcol;
__at (0xc37d) unsigned char textrow;

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
        ex af, af'
        exx
        ei
        ret
    __endasm;
}
#pragma preproc_asm +

void setup_timer(void (*callback)(void))
{
    callback_function = callback;
    set_timer();
}

void set_timer(void)
{
    __asm
        di
        ld bc, #3
        ld hl, #jump_timer
        ld de, #0xf8f8
        ldir

        ld hl, #0xf900
        ld de, #0xf901
        ld (hl), #0xf8
        ld bc, #256
        ldir
        ld a, #0xf9
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
        ld hl, #_screenbuffer
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
            prerendered[i][j/8] |= (0x80 >> (j%8));
        }
    }
}


void draw_moon(void) __naked
{
    __asm
        push iy ;// shift amount
        push bc ;// scr
        push de ;// pre
        push hl ;// temps
        push af ;// math

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
        ld iy, #fdm_loop_shift7
        jp fdm_setup_loop_done
    fdm_setup_loop_shift6:
        ld iy, #fdm_loop_shift6
        jp fdm_setup_loop_done
    fdm_setup_loop_shift5:
        ld iy, #fdm_loop_shift5
        jp fdm_setup_loop_done
    fdm_setup_loop_shift4:
        ld iy, #fdm_loop_shift4
        jp fdm_setup_loop_done
    fdm_setup_loop_shift3:
        ld iy, #fdm_loop_shift3
        jp fdm_setup_loop_done
    fdm_setup_loop_shift2:
        ld iy, #fdm_loop_shift2
        jp fdm_setup_loop_done
    fdm_setup_loop_shift1:
        ld iy, #fdm_loop_shift1
        jp fdm_setup_loop_done
    fdm_setup_loop_shift0:
        ld iy, #fdm_loop_shift0
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
        add hl, de
        ex de, hl ;// de = prerendered+prex

        ld bc, #_screenbuffer ;// bc now scr

    fdm_loop: ;// Want (bc) = (de)

        ld a, (de) ;// Put (de) into hl so we can shift with add hl, hl
        ld h, a ;// No little-endian business! The bytes are already in order!
        inc de
        ld a, (de)
        ld l, a
        jp (iy)

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
        bit 2, b ;// bc (scr) = screenbuf+1024? Must be aligned with _at!!!
        jp nz, fdm_loop_done
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
        pop iy

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
    unsigned char x = lander.fuel/(SCREEN_WIDTH/8);
    unsigned char shift = (lander.fuel/2)%8;

    if (lander.fuel == 0) {
        return;
    }
    for (i = 0; i < x; i++) {
        *(((unsigned char *)screenbuffer)+i) = 0xff;
    }
    *(((unsigned char *)screenbuffer)+i) |= ((char)0x80 >> shift);
}

// Only for sprites guaranteed to be fully onscreen
void draw_static_sprite_noclip(const unsigned char image[8], unsigned short x, unsigned char y)
{
    unsigned char i, rshift, lshift, localx;
    unsigned char *screenbyte;
    unsigned int start;

    localx = x-camera;
    rshift = localx%8;
    lshift = 8-rshift;
    start = localx/8+y*16;
    screenbyte = (unsigned char *)screenbuffer+start;
    for (i = 0; i < 8; i++) {
        *screenbyte |= (image[i] >> rshift);
        *(screenbyte+1) |= (image[i] << lshift);
        screenbyte += 16;
    }
}

// World x, not screen x.
void draw_live_sprite(const unsigned char animation[8][4],
        unsigned char frame, unsigned short x, unsigned char y, char offset)
{
    unsigned int yoffset = y*16;

    unsigned char i, rshift, lshift, imgbyte, *screenbyte, screenx;
    unsigned int start;
    int scratch;

    if (x > camera+SCREEN_WIDTH || x+8 < camera) {
        return;
    }

    scratch = x-camera+offset;
    screenx = (unsigned char)scratch;

    rshift = screenx%8;
    lshift = 8-rshift;
    start = screenx/8+yoffset;

    screenbyte = (unsigned char *)screenbuffer+start;
    if (scratch >= 0 && scratch <= SCREEN_WIDTH-8) {
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            *screenbyte |= (imgbyte >> rshift);
            *(screenbyte+1) |= (imgbyte << lshift);
            screenbyte += 16;
        }
    } else if (scratch < 0 && scratch > -8) {
        screenbyte = (unsigned char *)screenbuffer+yoffset;
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            *(screenbyte) |= (imgbyte << lshift);
            screenbyte += 16;
        }
    } else if (scratch < SCREEN_WIDTH && scratch+8 > SCREEN_WIDTH) {
        for (i = 0; i < 8; i++) {
            imgbyte = animation[i][frame];
            *screenbyte |= (imgbyte >> rshift);
            screenbyte += 16;
        }
    }
}
