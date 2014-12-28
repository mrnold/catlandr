#define KITTY_WIDTH 8
#define KITTY_HEIGHT 8

enum kitty_states {
    RUNNING_RIGHT,
    RUNNING_LEFT,
    SITTING
};

struct kitty_t {
    unsigned short x;
    unsigned char y;
    unsigned char (*bitmap)[8][CAT_RUNSTAGES];
    unsigned char state;
    unsigned char stage;
};

void draw_kitty(void);
void init_kitty(void);
void move_kitty(void);

extern struct kitty_t kitty;