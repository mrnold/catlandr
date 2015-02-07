#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define setup_refresh(callback) refresh = callback

enum sprite_mode {
    XOR,
    OR
};

void clear_screen(void) __naked;
void clear_buffer(void) __naked;
void prerender(void);
void draw_moon(void) __naked;
void printxy(unsigned char, unsigned char, const char * const);
void draw_status(void);
void draw_live_sprite(const unsigned char [8][4], unsigned char, unsigned short, unsigned char, char, char);
void save_graphbuffer(void) __naked;
void restore_graphbuffer(void) __naked;
void flipscreen(void);

extern unsigned char *screenbuffer;
extern void (*refresh)(void);
