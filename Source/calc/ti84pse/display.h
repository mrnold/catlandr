#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64

void prerender(void);
void save_graphbuffer(void) __naked;
void restore_graphbuffer(void) __naked;
