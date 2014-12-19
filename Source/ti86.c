#include "ti86.h"

static unsigned char r8;
static unsigned short r16;
static void (*callback_function)(void);

unsigned char screenbuffer[1024];

void putchar(char c) __naked
{
    c;
    __asm
        di
        call 0x4a2b
        ei
        ret
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

void clear_buffer(void) __naked
{
    __asm
        push af
        push bc
        push de
        push hl
        ld hl, #_screenbuffer
        xor a
        ld (hl), a
        ld bc, #0x03ff
        ld de, #_screenbuffer+1
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
void scan_row_6(struct keyrow_6 *k6)
{
    unsigned char *cast = (unsigned char *)k6;
    keyport = 0xfe;
    *cast = ~keyport;
}
void scan_row_0(struct keyrow_0 *k0)
{
    unsigned char *cast = (unsigned char *)k0;
    keyport = 0xbf;
    *cast = ~keyport;
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

void draw_vertical(unsigned short x, unsigned char height)
{
    unsigned char *i;
    unsigned char *screen = (unsigned char *)screenbuffer;
    unsigned int start = x/8+height*16;

    for (i = screen+start; i < screen+1024; i += 16) {
        *i |= (0x80 >> (x%8));
    }
}
