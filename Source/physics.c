#include "bitmap.h"
#include "physics.h"
#include "game.h"
#include "kibble.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "ti86.h"

unsigned int t; // Time tick to be updated ~25 times/sec

static void apply_motion(struct physics_object *);
static unsigned char find_collisions(struct physics_object *);

void init_physics(void)
{
    t = 0;
}

void init_object(struct physics_object *obj)
{
    obj->update.x = 0;
    obj->update.y = 0;
    obj->position.x = 0;
    obj->position.y = 0;
    obj->velocity.x = 0;
    obj->velocity.y = 0;
    obj->acceleration.x = 0;
    obj->acceleration.y = 0;
}

unsigned char move_object(struct physics_object *obj)
{
    unsigned char r;
    apply_motion(obj);
    r = find_collisions(obj);
    obj->position.x = obj->update.x;
    obj->position.y = obj->update.y;
    return r;
}

void apply_motion(struct physics_object *obj)
{
    int scratch;

    // Apply gravity in the absence of other vertical aceleration
    if (obj->acceleration.y == 0) {
        scratch = GRAVITY;
    } else {
        scratch = 0;
    }

    // Apply vertical acceleration
    scratch += (int)obj->velocity.y+(int)(obj->acceleration.y>>1);
    if (scratch > SPEED_MAX) {
        obj->velocity.y = SPEED_MAX;
    } else if (scratch < -SPEED_MAX) {
        obj->velocity.y = -SPEED_MAX;
    } else {
        obj->velocity.y = scratch;
    }

    // Apply "air friction" in the absence of horizontal acceleration
    if (obj->acceleration.x == 0) {
        if (obj->velocity.x > 0) {
            scratch = -1;
        } else if (obj->velocity.x < 0) {
            scratch = 1;
        } else {
            scratch = 0;
        }
    } else {
        scratch = 0;
    }

    // Apply horizontal acceleration
    scratch += (int)obj->velocity.x+(int)(obj->acceleration.x>>1);
    if (scratch > SPEED_MAX) {
        obj->velocity.x = SPEED_MAX;
    } else if (scratch < -SPEED_MAX) {
        obj->velocity.x = -SPEED_MAX;
    } else {
        obj->velocity.x = scratch;
    }

    // Apply vertical velocity
    scratch = (int)obj->position.y+(int)(obj->velocity.y>>2);
    if (scratch < 0) {
        obj->update.y = 0;
        obj->velocity.y = 0; // Immediate drop from top of screen feels better
    } else if (scratch > SCREEN_HEIGHT-8) {
        obj->update.y = SCREEN_HEIGHT-8;
    } else {
        obj->update.y = scratch;
    }

    // Apply horizontal velocity
    scratch = (int)obj->position.x+(int)obj->velocity.x;
    if (scratch < 0) {
        obj->update.x = 0;
    } else if (scratch > MOON_WIDTH-8) {
        obj->update.x = MOON_WIDTH-8;
    } else {
        obj->update.x = scratch;
    }
}

unsigned char find_collisions(struct physics_object *obj)
{
    unsigned short i;
    unsigned char max;

    // Check if we are about to run into the side of a hill
    if (obj->velocity.x > 0) {
        for (i = obj->position.x+8; i < obj->update.x; i++) {
            if (moon[i] < obj->position.y+8) {
                obj->update.x = obj->position.x+8+i;
                if (obj->velocity.x > IMPACT_MAX) {
                    return OBJ_HIGH_IMPACT_RIGHT;
                } else {
                    return OBJ_LOW_IMPACT_RIGHT;
                }
            }
        }
    } else if (obj->velocity.x < 0) { // Same but from the right
        for (i = obj->position.x; i > obj->update.x; i--) {
            if (moon[i] < obj->position.y+8) {
                obj->update.x = obj->position.x+i;
                if (obj->velocity.y < -IMPACT_MAX) {
                    return OBJ_HIGH_IMPACT_LEFT;
                } else {
                    return OBJ_LOW_IMPACT_LEFT;
                }
            }
        }
    }

    max = SCREEN_HEIGHT;
    for (i = obj->update.x; i < obj->update.x+8; i++) {
        if (moon[i] < max) {
            max = moon[i];
        }
    }

    // Touched down
    if (obj->update.y > max-8) {
        obj->update.y = max-8;
        obj->acceleration.y = 0;
        if (obj->velocity.y > IMPACT_MAX) {
            obj->velocity.y = 0;
            return OBJ_HIGH_IMPACT_DOWN;
        } else {
            obj->velocity.y = 0;
            return OBJ_LOW_IMPACT_DOWN;
        }
    }


    return OBJ_MOVING;
}
