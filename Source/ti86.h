struct keyrow_0 {
    unsigned char K_F5   : 1;
    unsigned char K_F4   : 1;
    unsigned char K_F3   : 1;
    unsigned char K_F2   : 1;
    unsigned char K_F1   : 1;
    unsigned char K_2ND  : 1;
    unsigned char K_EXIT : 1;
    unsigned char K_MORE : 1;
};

struct keyrow_6 {
    unsigned char K_DOWN  : 1;
    unsigned char K_LEFT  : 1;
    unsigned char K_RIGHT : 1;
    unsigned char K_UP    : 1;
    unsigned char         : 4;
};

void putchar(char c) __naked;
void idle(void) __naked;
void clear_screen(void) __naked;
void timer_isr(void) __naked;
void setup_timer(void (*)(void));
void set_timer(void) __naked;
unsigned char random8(void);
unsigned short random16(void);
void scan_row_6(struct keyrow_6 *);
void scan_row_0(struct keyrow_0 *);
