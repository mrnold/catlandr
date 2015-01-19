#define LANDER_STAGES 4
#define THRUST_STAGES 4
#define CAT_RUNSTAGES 4
#define KIBBLE_FALLSTAGES 4

enum lander_stages {
    LANDER_FLYING,
    LANDER_DOWNCRASH,
    LANDER_RIGHTCRASH,
    LANDER_LEFTCRASH
};

extern const unsigned char img_lander[][LANDER_STAGES];
extern const unsigned char img_thrustleft[][THRUST_STAGES];
extern const unsigned char img_thrustdown[][THRUST_STAGES];
extern const unsigned char img_thrustright[][THRUST_STAGES];

extern const unsigned char cat_batting[][CAT_RUNSTAGES];
extern const unsigned char cat_runleft[][CAT_RUNSTAGES];
extern const unsigned char cat_runright[][CAT_RUNSTAGES];
extern const unsigned char cat_sittingleft[][CAT_RUNSTAGES];
extern const unsigned char cat_sittingright[][CAT_RUNSTAGES];
extern const unsigned char cat_snackingleft[][CAT_RUNSTAGES];
extern const unsigned char cat_snackingright[][CAT_RUNSTAGES];

extern const unsigned char kibble[][KIBBLE_FALLSTAGES];
