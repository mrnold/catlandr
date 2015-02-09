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
        char x, y;
    } speed;
    struct {
        char x, y;
    } acceleration;
    struct {
        unsigned char hp_stage   : 2; // Horizontal positive
        unsigned char hp_firing  : 1;
        unsigned char hn_stage   : 2; // Horizontal negative
        unsigned char hn_firing  : 1;
        unsigned char vp_stage   : 2; // Vertical positive (screen down)
        unsigned char vp_firing  : 1;
    } thrust;
    union {
        struct {
            unsigned char stranded : 1;
            unsigned char crashed : 1;
            unsigned char landed : 1;
        } stuck;
        unsigned char stopped;
    } freedom;
    const unsigned char (*bitmap)[][LANDER_STAGES];
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
    lander.thrust.hp_stage = 0;      \
    lander.thrust.hp_firing = 0;     \
    lander.thrust.hn_stage = 0;      \
    lander.thrust.hn_firing = 0;     \
    lander.thrust.vp_stage = 0;      \
    lander.thrust.vp_firing = 0;     \
    lander.freedom.stopped = false;  \
    lander.fuel = 255;               \
    lander.food = KIBBLE_MAX;        \
    lander.stage = LANDER_FLYING;
