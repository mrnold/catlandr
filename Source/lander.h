#define LANDER_WIDTH 8
#define LANDER_HEIGHT 8

enum lander_stops {
    STRANDED,
    CRASHED,
    LANDED
};

struct lander_t {
/*    unsigned short x;
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
    } acceleration;*/
    struct physics_object phys;
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
    const unsigned char *bitmap;
    unsigned char fuel;
    unsigned char food;
};

void draw_lander(void);
void init_lander(void);
void move_lander(void);
void stop_lander(unsigned char);

extern struct lander_t lander;
