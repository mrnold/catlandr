#define ANIMATION_STAGES 4

enum lander_stages {
    LANDER_FLYING,
    LANDER_DOWNCRASH,
    LANDER_RIGHTCRASH,
    LANDER_LEFTCRASH
};

extern const unsigned char img_lander[][ANIMATION_STAGES];
extern const unsigned char img_thrustleft[][ANIMATION_STAGES];
extern const unsigned char img_thrustdown[][ANIMATION_STAGES];
extern const unsigned char img_thrustright[][ANIMATION_STAGES];

extern const unsigned char cat_batting[][ANIMATION_STAGES];
extern const unsigned char cat_runleft[][ANIMATION_STAGES];
extern const unsigned char cat_runright[][ANIMATION_STAGES];
extern const unsigned char cat_sittingleft[][ANIMATION_STAGES];
extern const unsigned char cat_sittingright[][ANIMATION_STAGES];
extern const unsigned char cat_snackingleft[][ANIMATION_STAGES];
extern const unsigned char cat_snackingright[][ANIMATION_STAGES];

extern const unsigned char kibble[][ANIMATION_STAGES];
