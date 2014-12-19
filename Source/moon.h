#define MOON_WIDTH 1025
#define LANDINGPAD_WIDTH 20
#define MOON_SAFERAM 0x8100

void generate_moon(void);
void world_redraw(void);
void draw_moon(void);
void init_moon(void);

__at (MOON_SAFERAM) extern unsigned char moon[];
extern unsigned short landingpad;
