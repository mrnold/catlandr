void loop_game(void);
void apply_input(void);
void gamesequence(void);

extern unsigned int crashes;
extern unsigned int landings;

#define init_game() \
    frames = 0;     \
    dropped = 0;
