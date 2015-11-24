#include "life.h"

#include <stdlib.h>

bool cell_next_state(cell *self)
{
    if (self->alive) {
        return (self->live_neighbors == 2 || self->live_neighbors == 3);
    } else {
        return (self->live_neighbors == 3);
    }
}

universe *universe_create(uint64_t width, uint64_t height, bool seed[])
{
    universe *univ = calloc(sizeof(universe), 1);  // Zero-initializes
    univ->width = width;
    univ->height = height;
    univ->cells = calloc(sizeof(cell), width * height);

    universe_foreach_pos(univ, x, y) {
        cell *cur_cell = universe_cell_at(univ, x, y);
        cur_cell->x = x;
        cur_cell->y = y;
        cur_cell->alive = seed[y * width + x];
        cur_cell->age = (cur_cell->alive) ? 1 : 0;
        universe_foreach_neighbor(univ, cur_cell, neighbor_x, neighbor_y) {
            if (seed[neighbor_y * width + neighbor_x]) {
                cur_cell->live_neighbors++;
            }
        }
    }

    return univ;
}

cell *universe_cell_at(universe *univ, uint32_t x, uint32_t y)
{
    if (x < univ->width && y < univ->height) {
       return &univ->cells[y * univ->width + x];
    } else {
       return NULL;
    }
}

void universe_evolve(universe *univ)
{
    univ->generation++;
}

void universe_destroy(universe *univ)
{
    free(univ->cells);
    free(univ);
}
