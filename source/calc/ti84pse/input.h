#define K_KIBBLE keys.K_2ND
#define K_MENU   keys.K_YEQ
#define K_MOON   keys.K_WINDOW
#define K_QUIT   keys.K_DEL

union keyrow_0 {
    struct {
        unsigned char K_GRAPH  : 1;
        unsigned char K_TRACE  : 1;
        unsigned char K_ZOOM   : 1;
        unsigned char K_WINDOW : 1;
        unsigned char K_YEQ    : 1;
        unsigned char K_2ND    : 1;
        unsigned char K_MODE   : 1;
        unsigned char K_DEL    : 1;
    } keys;
    unsigned char raw;
};

union keyrow_6 {
    struct {
        unsigned char K_DOWN  : 1;
        unsigned char K_LEFT  : 1;
        unsigned char K_RIGHT : 1;
        unsigned char K_UP    : 1;
        unsigned char         : 4;
    } keys;
    unsigned char raw;
};

void scan_row_6(union keyrow_6 *);
void scan_row_0(union keyrow_0 *);
