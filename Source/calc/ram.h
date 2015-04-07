#ifdef CALCULATOR_MODEL
    #if CALCULATOR_MODEL==86
        #include "ti86/ram.h"
    #elif CALCULATOR_MODEL==8402
        #include "ti84pse/ram.h"
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
extern __at (TIME_ADDRESS) unsigned int t;
extern __at (CAMERA_ADDRESS) unsigned int camera;
extern __at (PREVCAMERA_ADDRESS) unsigned int previouscamera;
extern __at (GAMESTATE_ADDRESS) unsigned char gamestate;
extern __at (LANDINGPAD_ADDRESS) unsigned short landingpad;
extern __at (TICKS_ADDRESS) unsigned int ticks;

void savescores(void) __naked;
