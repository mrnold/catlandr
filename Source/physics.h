void init_physics(void);
void apply_input(void);
void collisions(void);
void physics(void);

extern unsigned int camera;
extern unsigned int t;

#define GRAVITY 2
#define ACCEL_MAX 10
#define SPEED_MAX 15
#define IMPACT_MAX 10
