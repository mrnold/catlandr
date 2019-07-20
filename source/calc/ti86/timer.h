#define idle() __asm__("ei\nhalt")

void set_timer(void) __naked;
