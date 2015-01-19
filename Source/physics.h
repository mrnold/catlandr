struct physics_object {
    struct {
        unsigned short x;
        unsigned char y;
    } position;

    struct {
        unsigned short x;
        unsigned char y;
    } update;

    struct {
        char x, y;
    } velocity;

    struct {
        char x, y;
    } acceleration;
};

unsigned char move_object(struct physics_object *);
void init_object(struct physics_object *);
void init_physics(void);
void collisions(void);

extern unsigned int previouscamera;
extern unsigned int camera;
extern unsigned int t;

#define GRAVITY 2
#define ACCEL_MAX 10
#define SPEED_MAX 15
#define IMPACT_MAX 10

enum collisionstate {
    OBJ_MOVING,
    OBJ_HIGH_IMPACT_RIGHT,
    OBJ_HIGH_IMPACT_LEFT,
    OBJ_HIGH_IMPACT_DOWN,
    OBJ_LOW_IMPACT_RIGHT,
    OBJ_LOW_IMPACT_LEFT,
    OBJ_LOW_IMPACT_DOWN
};
