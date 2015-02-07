union keyrow_0 {
    struct {
        unsigned char K_F5   : 1;
        unsigned char K_F4   : 1;
        unsigned char K_F3   : 1;
        unsigned char K_F2   : 1;
        unsigned char K_F1   : 1;
        unsigned char K_2ND  : 1;
        unsigned char K_EXIT : 1;
        unsigned char K_MORE : 1;
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
