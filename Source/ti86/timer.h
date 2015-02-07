#define idle() __asm__("ei\nhalt")

void timer_isr(void) __naked;
void setup_timer(void (*)(void));
void set_timer(void) __naked;
