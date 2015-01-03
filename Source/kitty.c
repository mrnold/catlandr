#include "bitmap.h"
#include "kibble.h"
#include "lander.h"
#include "misc.h"
#include "moon.h"
#include "physics.h"
#include "ti86.h"
#include "kitty.h"

struct kitty_t kitty;

void init_kitty(void)
{
    kitty.y = 0;
    kitty.x = 256;
    kitty.speed.x = 0;
    kitty.speed.y = 0;
    kitty.stage = 0;
    kitty.state = SITTING;
    kitty.bitmap = &cat_sittingleft;
    kitty.batting = false;
}

void draw_kitty(void)
{
    draw_live_sprite(*kitty.bitmap, kitty.stage, kitty.x, kitty.y, 0, OR);
}

void move_kitty(void)
{
    int scratch;
    unsigned int i;
    static unsigned char countdown;
    unsigned char max = SCREEN_HEIGHT;
    unsigned char nextstate = SITTING;
    static struct kibble_t *kibble = (struct kibble_t *)NULL;

    // Find highest peak under cat's feet so it can run along the surface
    for (i = kitty.x; i < kitty.x+KITTY_WIDTH; i++) {
        if (moon[i] < max) {
            max = moon[i];
        }
    }

    // Cat state machine
    if (kitty.state == SITTING) { // Initial state
        if (lander.x == kitty.x || lander.freedom.stopped) {
            nextstate = SITTING;
        } else if (lander.x+LANDER_WIDTH < kitty.x) {
            nextstate = RUNNING_LEFT;
        } else if (lander.x > kitty.x+KITTY_WIDTH) {
            nextstate = RUNNING_RIGHT;
        } else {
            nextstate = SITTING;
        }
    } else if (kitty.state == RUNNING_LEFT) {
        scratch = lander.x+LANDER_WIDTH+3*KITTY_WIDTH;
        kibble = find_kibbles(kitty.x);
        if (lander.x == kitty.x || kitty.speed.x == 0) {
            nextstate = SITTING;
        } else if (kibble != (struct kibble_t *)NULL) {
            nextstate = SNACKING_LEFT;
        } else if (lander.x > kitty.x) {
            nextstate = SITTING;
        } else if (kitty.x > scratch) {
            nextstate = RUNNING_LEFT;
        } else if (scratch > kitty.x) {
            if (lander.freedom.stopped) {
                nextstate = SITTING;
            } else {
                nextstate = JUMPING_LEFT;
            }
        }
    } else if (kitty.state == RUNNING_RIGHT) {
        scratch = lander.x-3*KITTY_WIDTH;
        kibble = find_kibbles(kitty.x+KITTY_WIDTH);
        if (lander.x == kitty.x || kitty.speed.x == 0) {
            nextstate = SITTING;
        } else if (kibble != (struct kibble_t *)NULL) {
            nextstate = SNACKING_RIGHT;
        } else if (lander.x < kitty.x) {
            nextstate = SITTING;
        } else if (kitty.x < scratch) {
            nextstate = RUNNING_RIGHT;
        } else if (scratch < kitty.x) {
            if (lander.freedom.stopped) {
                nextstate = SITTING;
            } else {
                nextstate = JUMPING_RIGHT;
            }
        }
    } else if (kitty.state == JUMPING_LEFT) {
        if (kitty.y >= max-KITTY_HEIGHT) {
            nextstate = SITTING;
        } else {
            nextstate = JUMPING_LEFT;
        }
    } else if (kitty.state == JUMPING_RIGHT) {
        if (kitty.y >= max-KITTY_HEIGHT) {
            nextstate = SITTING;
        } else {
            nextstate = JUMPING_RIGHT;
        }
    } else if (kitty.state == SNACKING_LEFT) {
        if (countdown == 0) {
            nextstate = SITTING;
        } else {
            nextstate = SNACKING_LEFT;
        }
    } else if (kitty.state == SNACKING_RIGHT) {
        if (countdown == 0) {
            nextstate = SITTING;
        } else {
            nextstate = SNACKING_RIGHT;
        }
    }

    // Next state has been determined, now apply it physically
    if (nextstate == SITTING) {
        if (kitty.batting) {
            kitty.bitmap = &cat_batting;
        } else { // Check previous state to decide which way to sit
            switch (kitty.state) {
                case JUMPING_LEFT:
                case RUNNING_LEFT:
                case SNACKING_LEFT:
                    kitty.bitmap = &cat_sittingleft; break;
                case JUMPING_RIGHT:
                case RUNNING_RIGHT:
                case SNACKING_RIGHT:
                    kitty.bitmap = &cat_sittingright; break;
            } // Pass through case: remember previous sprite
        }
        kitty.y = max-KITTY_HEIGHT;
        kitty.speed.y = 0;
        kitty.speed.x = 0;
        if (kitty.state == SNACKING_LEFT || kitty.state == SNACKING_RIGHT) {
            if (kibble != (struct kibble_t *)NULL) {
                kibble->eaten = true;
            }
        }
    } else if (nextstate == RUNNING_LEFT) {
        kitty.bitmap = &cat_runleft;
        kitty.y = max-KITTY_HEIGHT;
        kitty.speed.x = -4;
        kitty.speed.y = 0;
    } else if (nextstate == RUNNING_RIGHT) {
        kitty.bitmap = &cat_runright;
        kitty.y = max-KITTY_HEIGHT;
        kitty.speed.x = 4;
        kitty.speed.y = 0;
    } else if (nextstate == JUMPING_LEFT) {
        kitty.bitmap = &cat_runleft;
        if (kitty.state != JUMPING_LEFT) { // Just starting a jump
            kitty.speed.y = (lander.y-kitty.y)>>3;
        }
    } else if (nextstate == JUMPING_RIGHT) {
        kitty.bitmap = &cat_runright;
        if (kitty.state != JUMPING_RIGHT) {
            kitty.speed.y = (lander.y-kitty.y)>>3;
        }
    } else if (nextstate == SNACKING_LEFT) {
        if (kitty.batting) {
            kitty.bitmap = &cat_batting;
        } else {
            kitty.bitmap = &cat_snackingleft;
            if (kitty.state == RUNNING_LEFT) {
                countdown = 25;
                kitty.speed.x = 0;
            } else {
                if ((t&0x07) == 0) {
                    countdown--;
                }
            }
        }
        kitty.y = max-KITTY_HEIGHT;
        kitty.speed.y = 0;
        kitty.speed.x = 0;
    } else if (nextstate == SNACKING_RIGHT) {
        if (kitty.batting) {
            kitty.bitmap = &cat_batting;
        } else {
            kitty.bitmap = &cat_snackingright;
            if (kitty.state == RUNNING_RIGHT) {
                countdown = 25;
                kitty.speed.x = 0;
            } else {
                if ((t&0x07) == 0) {
                    countdown--;
                }
            }
        }
        kitty.y = max-KITTY_HEIGHT;
        kitty.speed.y = 0;
        kitty.speed.x = 0;
    }

    if (kitty.batting) {
        switch (nextstate) {
            case SITTING:
            case SNACKING_LEFT:
            case SNACKING_RIGHT:
                break;
            default:
                kitty.batting = false;
        }
    }

    // Move cat horizontally within world limits
    scratch = kitty.x+kitty.speed.x;
    if (scratch <= 0) {
        kitty.x = 0;
        kitty.speed.x = 0;
    } else if (scratch > MOON_WIDTH-KITTY_WIDTH) {
        kitty.x = MOON_WIDTH-KITTY_WIDTH;
        kitty.speed.x = 0;
    } else {
        kitty.x = scratch;
    }

    // Move cat vertically
    scratch = kitty.y+kitty.speed.y;
    if (scratch <= 0) {
        kitty.y = 0;
    } else if (scratch > max-KITTY_HEIGHT) {
        kitty.y = max-KITTY_HEIGHT;
    } else {
        kitty.y = scratch;
    }

    // Apply gravity
    if (kitty.y < max-KITTY_HEIGHT) {
        kitty.speed.y++;
    }

    // Move to next animation 
    if ((t&0x07) == 0) {
        kitty.stage = (kitty.stage+1)&(CAT_RUNSTAGES-1);
    }

    // Transition to next state for the next frame
    kitty.state = nextstate;
}
