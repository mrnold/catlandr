#define KITTY_WIDTH 8
#define KITTY_HEIGHT 8

enum kitty_states {
    RUNNING_RIGHT,
    RUNNING_LEFT,
    JUMPING_LEFT,
    JUMPING_RIGHT,
    SNACKING_RIGHT,
    SNACKING_LEFT,
    SITTING
};

struct kitty_t {
    unsigned short x;
    unsigned char y;
    const unsigned char (*bitmap)[][ANIMATION_STAGES];
    unsigned char state;
    unsigned char stage;
    unsigned char batting;
    struct {
        signed char x;
        signed char y;
    } speed;
};

void draw_kitty(void);
void move_kitty(void);

#define init_kitty()                 \
    kitty.y = 0;                     \
    kitty.x = random16()&0x3FF;      \
    if (kitty.x < SCREEN_WIDTH) {    \
        kitty.x = SCREEN_WIDTH;      \
    }                                \
    kitty.speed.x = 0;               \
    kitty.speed.y = 0;               \
    kitty.stage = 0;                 \
    kitty.state = SITTING;           \
    kitty.bitmap = &cat_sittingleft; \
    kitty.batting = false;
