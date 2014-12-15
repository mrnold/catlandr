void putchar(char c) __naked;
void idle(void) __naked;
void clear_screen(void) __naked;
void timer_isr(void) __naked;
void setup_timer(void (*)(void));
void set_timer(void) __naked;
unsigned char random8(void);
unsigned short random16(void);
