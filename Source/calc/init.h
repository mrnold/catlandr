#ifdef CALCULATOR_MODEL
    #if CALCULATOR_MODEL==86
        #include "ti86/init.h"
    #else
        #error "Invalid calculator model specified!"
    #endif
#else
    #error No calculator model defined!
#endif
