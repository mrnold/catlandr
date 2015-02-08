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
void printxy(unsigned char, unsigned char, const char * const);
void updatescreen(void);
