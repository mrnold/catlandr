#ifdef CALCULATOR_MODEL
    #if CALCULATOR_MODEL==86
        #include "ti86/ram.h"
    #else
        #error "Invalid calculator model specified!"
    #endif
#else
    #error No calculator model defined!
#endif

extern __at (MOON_ADDRESS) unsigned char moon[MOON_WIDTH];
extern __at (KIBBLES_ADDRESS) struct kibble_t kibbles[KIBBLE_MAX];
extern __at (LANDER_ADDRESS) struct lander_t lander;
extern __at (KITTY_ADDRESS) struct kitty_t kitty;
extern __at (FRAMES_ADDRESS) unsigned int frames;
extern __at (DROPPED_ADDRESS) unsigned int dropped;

void memset(void *, unsigned char, unsigned short);
void savescores(void) __naked;
