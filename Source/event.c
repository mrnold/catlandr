#include "lock.h"
#include "event.h"

lock_t evt_lock;
event_t events[EVENT_MAX];
unsigned char current_event;

void init_event(void)
{
    current_event = 0;
    init_lock(evt_lock);
}

void add_event(event_t evt)
{
    wait_lock(evt_lock);
    events[current_event] = evt;
    current_event++;
    if (current_event == EVENT_MAX) {
        current_event = 0;
    }
    drop_lock(evt_lock);
}

event_t get_event(void)
{
    event_t returned;
    wait_lock(evt_lock);
    if (current_event == 0) {
        returned = NONE;
    } else {
        current_event--;
        returned = events[current_event];
        events[current_event] = NONE;
    }
    drop_lock(evt_lock);
    return returned;
}
