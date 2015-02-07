
#define PRERENDER_ADDRESS   0x8100
#define MOON_ADDRESS        0xa100
#define KIBBLES_ADDRESS     0xa600
#define LANDER_ADDRESS      0xa700
#define KITTY_ADDRESS       0xa800
#define BACKUPGRAPH_ADDRESS 0xb000
#define TEXTCOL_ADDRESS     0xc37c
#define TEXTROW_ADDRESS     0xc37d
#define SCREENBUF1_ADDRESS  0xca00
#define SCREENBUF0_ADDRESS  0xfc00

#define setup_refresh(callback) refresh = callback
#define idle() __asm__("ei\nhalt")

extern __at (PRERENDER_ADDRESS) unsigned char prerendered[SCREEN_HEIGHT][(MOON_WIDTH-1)/8];
extern __at (MOON_ADDRESS) unsigned char moon[MOON_WIDTH];
extern __at (KIBBLES_ADDRESS) struct kibble_t kibbles[KIBBLE_MAX];
extern __at (LANDER_ADDRESS) struct lander_t lander;
extern __at (KITTY_ADDRESS) struct kitty_t kitty;
extern __at (BACKUPGRAPH_ADDRESS) unsigned char *backupgraph;

extern unsigned char *screenbuffer;
extern void (*refresh)(void);

union keyrow_0 {
    struct {
        unsigned char K_F5   : 1;
        unsigned char K_F4   : 1;
        unsigned char K_F3   : 1;
        unsigned char K_F2   : 1;
        unsigned char K_F1   : 1;
        unsigned char K_2ND  : 1;
        unsigned char K_EXIT : 1;
        unsigned char K_MORE : 1;
    } keys;
    unsigned char raw;
};

union keyrow_6 {
    struct {
        unsigned char K_DOWN  : 1;
        unsigned char K_LEFT  : 1;
        unsigned char K_RIGHT : 1;
        unsigned char K_UP    : 1;
        unsigned char         : 4;
    } keys;
    unsigned char raw;
};

enum sprite_mode {
    XOR,
    OR
};

void clear_screen(void) __naked;
void clear_buffer(void) __naked;
void timer_isr(void) __naked;
void setup_timer(void (*)(void));
void set_timer(void) __naked;
unsigned char random8(void);
unsigned short random16(void);
void scan_row_6(union keyrow_6 *);
void scan_row_0(union keyrow_0 *);
void prerender(void);
void draw_moon(void) __naked;
void printxy(unsigned char, unsigned char, const char * const);
void draw_status(void);
void draw_live_sprite(const unsigned char [8][4], unsigned char, unsigned short, unsigned char, char, char);
void save_graphbuffer(void) __naked;
void restore_graphbuffer(void) __naked;
void flipscreen(void);
void init_calculator(void);
