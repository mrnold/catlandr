#define EVENT_MAX 16

typedef enum {
    NONE,
    QUIT,
    SIGNAL_GO
} event_t;

void init_event(void);
void add_event(event_t);
event_t get_event(void);
