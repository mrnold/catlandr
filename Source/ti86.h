#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

extern __at (0x8100) unsigned char prerendered[SCREEN_HEIGHT][(MOON_WIDTH-1)/8];
extern __at (0xca00) unsigned char screenbuffer1[SCREEN_HEIGHT][SCREEN_WIDTH/8];
extern __at (0xfc00) unsigned char screenbuffer2[SCREEN_HEIGHT][SCREEN_WIDTH/8];
extern __at (0xb000) unsigned char *backupgraph;
extern unsigned char *screenbuffer;

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

void putchar(char c) __naked;
void idle(void) __naked;
void clear_screen(void) __naked;
void clear_buffer(void) __naked;
void timer_isr(void) __naked;
void setup_timer(void (*)(void));
void set_timer(void) __naked;
unsigned char random8(void);
unsigned short random16(void);
void scan_row_6(union keyrow_6 *);
void scan_row_0(union keyrow_0 *);
void screencopy(void) __naked;
void prerender(void);
void draw_moon(void) __naked;
void printxy(unsigned char, unsigned char, const char * const);
void draw_status(void);
void draw_live_sprite(const unsigned char [8][4], unsigned char, unsigned short, unsigned char, char, char);
void save_graphbuffer(void) __naked;
void restore_graphbuffer(void) __naked;
void flipscreen(void);
