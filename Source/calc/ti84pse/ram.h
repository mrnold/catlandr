#define PRERENDER_ADDRESS   0x8100
#define MOON_ADDRESS        0xa100
#define KIBBLES_ADDRESS     0xa600
#define LANDER_ADDRESS      0xa700
#define KITTY_ADDRESS       0xa800
#define FRAMES_ADDRESS      0xa900
#define DROPPED_ADDRESS     0xa910
#define TIME_ADDRESS        0xa920
#define CAMERA_ADDRESS      0xa930
#define PREVCAMERA_ADDRESS  0xa940
#define GAMESTATE_ADDRESS   0xa950
#define LANDINGPAD_ADDRESS  0xa960
#define FRAMELOCK_ADDRESS   0xa970
#define IDLELOCK_ADDRESS    0xa980
#define TICKS_ADDRESS       0xa990
#define PREVKEY0_ADDRESS    0xa9a0
#define PREVKEY6_ADDRESS    0xa9a0
#define COUNTDOWN_ADDRESS   0xa9b0
#define KPT_ADDRESS         0xa9c0
#define BACKUPGRAPH_ADDRESS 0xb000
#define TEXTCOL_ADDRESS     0x86d7
#define TEXTROW_ADDRESS     0x86d8
#define SCREENBUF1_ADDRESS  0xca00
#define SCREENBUF0_ADDRESS  0xfc00

#define DIVHLBY10           0x400f
#define VPUTMAP             0x455e
#define VPUTS               0x4561
#define SET_ABS_DEST_ADDR   0x5285
#define SET_ABS_SRC_ADDR    0x4647
#define SET_MM_NUM_BYTES    0x464f
#define MM_LDIR_SET_SIZE    0x524d
#define MM_LDIR             0x52ed

#define ASAPVAR             0xd6fc

extern __at (PRERENDER_ADDRESS) unsigned char prerendered[SCREEN_HEIGHT][(MOON_WIDTH-1)/8];
extern __at (BACKUPGRAPH_ADDRESS) unsigned char *backupgraph;
