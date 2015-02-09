#ifdef CALCULATOR_MODEL
    #if CALCULATOR_MODEL==86
        #include "ti86/display.h"
    #else
        #error "Invalid calculator model specified!"
    #endif
#else
    #error No calculator model defined!
#endif

enum sprite_mode {
    XOR,
    OR
};

extern void (*refresh)(void);
#define setup_refresh(callback) refresh = callback

void clear_screen(void) __naked;
void draw_live_sprite(const unsigned char [8][4], unsigned char, unsigned short, unsigned char, char, char);
void draw_moon(void) __naked;
void draw_status(void);
void print(const char * const);
void printnum(unsigned int);
void printxy(unsigned char, unsigned char, const char * const);
void printnumxy(unsigned char, unsigned char, unsigned int);
void updatescreen(void);

#define printhelp()                             \
    printxy(0, 0,  "EXIT to quit");             \
    printxy(0, 6,  "F1 for this menu");         \
    printxy(0, 12, "F2 for a new moon");        \
    printxy(0, 18, "2ND to drop a treat");      \
    printxy(0, 24, "Arrow keys for thrusters")

#define printscore()                   \
    printxy(85, 0, "Wins-Losses"); \
    printnumxy(88, 6, landings);      \
    print("-");                        \
    printnum(crashes)

#define printgamestart()               \
    printxy(89, 0,  "Land safely");    \
    printxy(85, 6,  "to feed kitty!"); \
    printxy(88, 12, "Thrust up to");   \
    printxy(88, 18, "get started!");   \
    printnumxy(88, 24, landings);      \
    print("-");                        \
    printnum(crashes)

#define printdestroyed()           \
    printxy(87, 18, "Lander");     \
    printxy(91, 24, "destroyed!")

#define printnogas() \
    printxy(84, 18, "Out of fuel!")

#define printvictory()                \
    printxy(84, 18, "Safe landing!"); \
    printxy(90, 24, "Kitten fed!")
