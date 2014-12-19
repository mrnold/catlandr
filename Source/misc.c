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
