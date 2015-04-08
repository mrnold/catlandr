#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64

#define printhelp()                             \
    printxy(0, 0,  "MODE to quit");             \
    printxy(0, 6,  "Y= for this menu");         \
    printxy(0, 12, "WINDOW for a new moon");    \
    printxy(0, 18, "2ND to drop a treat");      \
    printxy(0, 24, "Arrow keys for thrusters")

#define printscore()                   \
    printxy(57, 0, "Wins-Losses");     \
    printnumxy(61, 6, landings);       \
    print("-");                        \
    printnum(crashes)

#define printgamestart()               \
    printxy(0, 36, "Land safely to feed kitty!");    \
    printxy(0, 42, "Thrust up to get started!");   \
    printscore();

#define printdestroyed()           \
    printxy(0, 36, "Lander destroyed!")

#define printnogas() \
    printxy(0, 36, "Out of fuel!")

#define printvictory()                \
    printxy(0, 36, "Safe landing! Kitten fed!"); \

void prerender(void);
void save_graphbuffer(void) __naked;
void restore_graphbuffer(void) __naked;
