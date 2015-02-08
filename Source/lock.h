#define hash #
#define f(x) x
#define label(x) f(hash)_##x
#define offset(x) f(hash).x

#define wait_lock(x) \
    __asm \
        push hl \
        ld hl, label(x) \
        sra (hl) \
        jp c, offset(-2) \
        pop hl \
    __endasm;

#define init_lock(x) x = 0xfe;
#define drop_lock(x) init_lock(x)
#define is_locked(x) ((lock_t)x & 0x01)

typedef unsigned char lock_t;
