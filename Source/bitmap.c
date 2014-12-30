#include "bitmap.h"
#include "lander.h"

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

unsigned char img_downcrash[LANDER_HEIGHT] = {
    0b10000001,
    0b01000010,
    0b10000001,
    0b01000110,
    0b11100111,
    0b00111110,
    0b01111100,
    0b11001010
};

unsigned char img_rightcrash[LANDER_HEIGHT] = {
    0b01000010,
    0b00100100,
    0b01000010,
    0b00101111,
    0b00011101,
    0b00001111,
    0b00001111,
    0b00110011
};

unsigned char img_leftcrash[LANDER_HEIGHT] = {
    0b00100100,
    0b00010010,
    0b00100100,
    0b10101110,
    0b10111100,
    0b11101110,
    0b11111110,
    0b11110010
};

unsigned char img_thrustright[LANDER_HEIGHT][THRUST_STAGES] = {
    { 0b00000000, 0b00000000, 0b00100000, 0b01110000 },
    { 0b00000000, 0b01100000, 0b01111000, 0b11011100 },
    { 0b11101010, 0b10111010, 0b10011110, 0b10001111 },
    { 0b00000000, 0b01100000, 0b01111000, 0b11011100 },
    { 0b00000000, 0b00000000, 0b00100000, 0b01110000 },
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 }
};
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

unsigned char cat_runright[8][CAT_RUNSTAGES] = {
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000100, 0b10001000, 0b01001000 },
    { 0b01000100, 0b01000111, 0b10000111, 0b10000111 },
    { 0b10000111, 0b10000111, 0b10000111, 0b10000111 },
    { 0b10000111, 0b11111110, 0b11111110, 0b01111110 },
    { 0b01111110, 0b01111110, 0b01111110, 0b01111110 },
    { 0b01111110, 0b01100110, 0b01100011, 0b01100110 },
    { 0b10101010, 0b01000010, 0b10000001, 0b00101000 }
};

unsigned char cat_runleft[8][CAT_RUNSTAGES] = {
    { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00100000, 0b00010001, 0b00010010 },
    { 0b00100010, 0b11100010, 0b11100001, 0b11100001 },
    { 0b11100001, 0b11100001, 0b11100001, 0b11100001 },
    { 0b11100001, 0b01111111, 0b01111111, 0b01111110 },
    { 0b01111110, 0b01111110, 0b01111110, 0b01111110 },
    { 0b01111110, 0b01100110, 0b01100011, 0b01100110 },
    { 0b01010101, 0b01000010, 0b10000001, 0b00010100 }
};

unsigned char cat_sitting[8][CAT_RUNSTAGES] = {
    { 0b00000010, 0b00000010, 0b00000011, 0b00000010 },
    { 0b10010101, 0b10010001, 0b01010010, 0b10010010 },
    { 0b01110001, 0b01110001, 0b01110001, 0b01110001 },
    { 0b01111001, 0b01111001, 0b01111001, 0b01111001 },
    { 0b00111101, 0b00111101, 0b00111101, 0b00111101 },
    { 0b00111111, 0b00111111, 0b00111111, 0b00111111 },
    { 0b00111110, 0b00111110, 0b00111110, 0b00111110 },
    { 0b01110110, 0b01110110, 0b01110110, 0b01110110 }
};
