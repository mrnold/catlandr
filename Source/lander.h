#define LANDER_WIDTH 8
#define LANDER_HEIGHT 8

struct lander_t {
    unsigned short x;
    unsigned char y;
    struct {
        unsigned short x;
        unsigned char y;
    } previous;
    struct {
        char x, y;
    } momentum;
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
    unsigned char crashed;
    unsigned char landed;
    unsigned char perched;
    unsigned char *bitmap;
};

void draw_lander(void);
void init_lander(void);

extern struct lander_t lander;
extern unsigned int camera;
