#define LANDER_WIDTH 8
#define LANDER_HEIGHT 8

enum lander_stops {
    STRANDED,
    CRASHED,
    LANDED
};

struct lander_t {
    unsigned short x;
    unsigned char y;
    struct {
        unsigned short x;
        unsigned char y;
    } previous;
    struct {
        signed char x;
        signed char y;
    } speed;
    struct {
        signed char x;
        signed char y;
    } acceleration;
    unsigned char hp_stage; // Horizontal positive
    unsigned char hp_firing;
    unsigned char hn_stage; // Horizontal negative
    unsigned char hn_firing;
    unsigned char vp_stage; // Vertical positive (screen down)
    unsigned char vp_firing;
    union {
        struct {
            unsigned char stranded : 1;
            unsigned char crashed : 1;
            unsigned char landed : 1;
        } stuck;
        unsigned char stopped;
    } freedom;
    const unsigned char (*bitmap)[][ANIMATION_STAGES];
    unsigned char stage;
    unsigned char fuel;
    unsigned char food;
};

void draw_lander(void);
void move_lander(void);
void stop_lander(unsigned char);

#define init_lander()                \
    lander.y = 0;                    \
    lander.x = SCREEN_WIDTH/2;       \
    lander.speed.x = 0;              \
    lander.speed.y = 0;              \
    lander.acceleration.x = 0;       \
    lander.acceleration.y = GRAVITY; \
    lander.bitmap = &img_lander;     \
    lander.hp_stage = 0;      \
    lander.hp_firing = 0;     \
    lander.hn_stage = 0;      \
    lander.hn_firing = 0;     \
    lander.vp_stage = 0;      \
    lander.vp_firing = 0;     \
    lander.freedom.stopped = false;  \
    lander.food = KIBBLE_MAX;        \
    lander.stage = LANDER_FLYING;
