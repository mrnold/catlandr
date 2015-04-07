#define KIBBLE_MAX 5
#define KIBBLE_WIDTH 8
#define KIBBLE_HEIGHT 8

struct kibble_t {
    unsigned short x;
    unsigned char y;
    struct {
        char x, y;
    } speed;
    const unsigned char (*bitmap)[][ANIMATION_STAGES];
    unsigned char stage;
    unsigned char ready;
    unsigned char eaten;
    unsigned char landed;
};

void draw_kibbles(void);
void move_kibbles(void);
void create_kibble(unsigned char, unsigned short, unsigned char, char, char);
struct kibble_t *find_kibbles(unsigned short);

#define init_kibbles()                   \
    for (i = 0; i < KIBBLE_MAX; i++) {   \
        kibbles[i].y = SCREEN_HEIGHT-8;  \
        kibbles[i].x = i*KIBBLE_WIDTH+1; \
        kibbles[i].bitmap = &kibble;     \
        kibbles[i].stage = 0;            \
        kibbles[i].ready = true;         \
        kibbles[i].eaten = false;        \
        kibbles[i].landed = false;       \
        kibbles[i].speed.x = 0;          \
        kibbles[i].speed.y = 1;          \
    }
