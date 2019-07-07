#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

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

void prerender(void);
void save_graphbuffer(void) __naked;
void restore_graphbuffer(void) __naked;
