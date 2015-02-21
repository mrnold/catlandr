#ifdef CALCULATOR_MODEL
    #if CALCULATOR_MODEL==86
        #include "ti86/timer.h"
    #elif CALCULATOR_MODEL==8402
        #include "ti84pse/timer.h"
    #else
        #error "Invalid calculator model specified!"
    #endif
#else
    #error No calculator model defined!
#endif

void setup_timer(void (*)(void));
void timer_isr(void) __naked;
