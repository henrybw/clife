#ifndef _LIFE_H
#define _LIFE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct cell {
    uint32_t x;
    uint32_t y;
    size_t age;  // 0 for dead cells, 1 or higher for live cells
    size_t live_neighbors;
    bool alive;
    bool next_state;
    bool dirty;  // XXX HBW - we really don't need this
} cell;

bool cell_next_state(cell *self);

typedef struct cell_pool {
    cell **cells;
    size_t count;
    size_t capacity;
} cell_pool;

cell_pool *cell_pool_create(size_t capacity);
cell *cell_pool_get(cell_pool *pool, size_t idx);
void cell_pool_push(cell_pool *pool, cell *new_cell);
void cell_pool_clear(cell_pool *pool);
void cell_pool_destroy(cell_pool *pool);

#define cell_pool_foreach(pool, _cell_iter)                                     \
    for (size_t __pool_i = 0; __pool_i < (pool)->count; __pool_i++)             \
        for (cell *_cell_iter = cell_pool_get((pool), __pool_i);                \
             _cell_iter;                                                        \
             _cell_iter = NULL)

typedef struct universe {
    uint32_t width;
    uint32_t height;
    cell *cells;
    cell_pool *changed;
    cell_pool *dirty;  // Used during evolution; stays empty most of the time
    size_t generation;
} universe;

universe *universe_create(uint64_t width, uint64_t height, bool *seed);
cell *universe_cell_at(universe *univ, uint32_t x, uint32_t y);
void universe_print(universe *univ);
void universe_print_verbose(universe *univ);
void universe_evolve(universe *univ);
void universe_destroy(universe *univ);

// Naming conventions for variables/parameters used by these macros:
//
//  - No leading underscore (param): The macro expects 'param' to be an already-
//    declared variable, or an expression that produces a value.
//
//  - One leading underscore (_param): The macro expects '_param' to be an
//    undefined identifier that the macro will define and populate within the
//    body. Used mostly to implement iterator cursors in foreach macros.
//
//  - Two leading underscores (__var): Macros use this naming scheme for private
//    variables used by the macro itself for internal bookkeeping. This is also
//    used for naming private helper macros. In other words, don't declare
//    variables or use identifiers with double underscores!

#define universe_foreach_pos(univ, _x_iter, _y_iter)                            \
    for (uint32_t _y_iter = 0; _y_iter < (univ)->height; _y_iter++)             \
        for (uint32_t _x_iter = 0; _x_iter < (univ)->width; _x_iter++)          \

// The purpose of this second for loop here is to declare the given identifier
// (in this case '_cell') in the scope of the universe_foreach_pos loop. It's
// a hack / abuse of C's for loop construct that emulates injecting
// 'cell *_cell_iter = ...;' into the beginning of the foreach loop body.
#define universe_foreach_cell(univ, _cell_iter)                                 \
    universe_foreach_pos(univ, __cell_x, __cell_y)                              \
        for (cell *_cell_iter = universe_cell_at((univ), __cell_x, __cell_y);   \
             _cell_iter;                                                        \
             _cell_iter = NULL)

// XXX HBW - I do have to admit, this macro / for loop abuse is getting a little
// out of hand. Maybe I should introduce a 'foreach_declare' macro or something.
#define universe_foreach_neighbor(univ, cur_cell, _x_iter, _y_iter)             \
    for (int32_t __neighbor_dy = -1; __neighbor_dy <= 1; __neighbor_dy++)       \
        for (int32_t __neighbor_dx = -1; __neighbor_dx <= 1; __neighbor_dx++)   \
            /* Add the width to guarantee we won't have a negative result,      \
             * so we can just use modulo to wrap the coordinates. */            \
            for (uint32_t _x_iter = (((univ)->width + (cur_cell)->x +           \
                                      __neighbor_dx) % (univ)->width),          \
                          _y_iter = (((univ)->height + (cur_cell)->y +          \
                                      __neighbor_dy) % (univ)->height),         \
                          __neighbor_sentinel = 1;                              \
                 /* Skips over the cell whose neighbors are being examined.     \
                  * We put this here instead of in its own if statement to      \
                  * avoid unintentional block confusion (you can follow the     \
                  * if with an else clause). */                                 \
                 (__neighbor_dx != 0 || __neighbor_dy != 0) &&                  \
                 __neighbor_sentinel;                                           \
                     __neighbor_sentinel = 0)

#define universe_foreach_neighbor_cell(univ, cur_cell, _neighbor)               \
    universe_foreach_neighbor((univ), (cur_cell),                               \
                              __neighbor_cell_x, __neighbor_cell_y)             \
        for (cell *_neighbor = universe_cell_at((univ),                         \
                                                __neighbor_cell_x,              \
                                                __neighbor_cell_y);             \
             _neighbor;                                                         \
             _neighbor = NULL)

#endif
