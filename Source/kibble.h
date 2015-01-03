#define KIBBLE_MAX 5
#define KIBBLE_WIDTH 8
#define KIBBLE_HEIGHT 8

struct kibble_t {
    unsigned short x;
    unsigned char y;
    struct {
        char x, y;
    } speed;
    const unsigned char (*bitmap)[][KIBBLE_FALLSTAGES];
    unsigned char stage;
    unsigned char ready;
    unsigned char eaten;
    unsigned char landed;
};

void draw_kibbles(void);
void init_kibbles(void);
void move_kibbles(void);
void create_kibble(unsigned char, unsigned short, unsigned char, char, char);
struct kibble_t *find_kibbles(unsigned short);

extern struct kibble_t kibbles[KIBBLE_MAX];
