#ifndef FLASHPAGE_H
#define FLASHPAGE_H

typedef struct {
    unsigned target_slot;
} riotboot_flashwrite_t;

static inline int riotboot_flashwrite_init(riotboot_flashwrite_t *state,
                                           int target_slot)
{
    (void)state;
    (void)target_slot;
    return 0;
}

#endif /* FLASHPAGE_H */
