#define EVENT_MAX 16

typedef enum {
    NONE,
    QUIT,
    WORLD_REDRAW,
    IMPULSE_LEFT,
    IMPULSE_RIGHT,
    IMPULSE_DOWN,
    IMPULSE_UP,
    PHYSICS,
    READ_KEYS,
    CLEAR_SCREEN,
    CLEAR_BUFFER,
    SCREEN_COPY,
    DRAW_MOON,
    DRAW_LANDER,
    SIGNAL_GO
} event_t;

void init_event(void);
void add_event(event_t);
event_t get_event(void);
