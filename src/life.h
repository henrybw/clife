#ifndef LIFE_H
#define LIFE_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

typedef uint32_t cell;  // Stores number of generations survived (when positive)
static const cell CELL_DEAD = 0;

// TODO - some kind of translation to absolute coordinates? (maybe define where
// the origin point is?)
typedef struct universe {
    cell *cell_bufs[2];
    size_t width, height;
    int cur_buf;
    int cur_generation;
} universe;

universe *create_universe(size_t min_width, size_t min_height);
// TODO - how do we seed the universe/add live cells?
void universe_expand(universe *univ);
void universe_tick(universe *univ);

static inline cell *universe_front_buffer(universe *univ)
{
    return univ->cell_bufs[univ->cur_buf];
}

static inline cell *universe_back_buffer(universe *univ)
{
    return univ->cell_bufs[!univ->cur_buf];
}

static inline void universe_swap_buffers(universe *univ)
{
    univ->cur_buf = !univ->cur_buf;
}

void destroy_universe(universe *univ);


#endif
