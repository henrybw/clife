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

cell_pool *cell_pool_create(size_t capacity)
{
    cell_pool *pool = calloc(sizeof(cell_pool), 1);
    pool->cells = calloc(sizeof(cell *), capacity);
    pool->capacity = capacity;
    return pool;
}

cell *cell_pool_get(cell_pool *pool, size_t idx)
{
    return (idx < pool->count) ? pool->cells[idx] : NULL;
}

void cell_pool_push(cell_pool *pool, cell *new_cell)
{
    if (pool->count == pool->capacity) {
        return;  // XXX HBW - silently failing will suffice, but isn't ideal
    }
    pool->cells[pool->count] = new_cell;
    pool->count++;
}

void cell_pool_clear(cell_pool *pool)
{
    // Doesn't actually free anything
    pool->count = 0;
}

void cell_pool_destroy(cell_pool *pool)
{
    // We didn't allocate any of the cells stored in here, which means we don't
    // need to free the cells individually here either.
    free(pool->cells);
    free(pool);
}

universe *universe_create(uint64_t width, uint64_t height, bool seed[])
{
    universe *univ = calloc(sizeof(universe), 1);  // Zero-initializes
    univ->width = width;
    univ->height = height;
    univ->cells = calloc(sizeof(cell), width * height);

    // At most, every cell could be in these pools
    univ->dirty = cell_pool_create(width * height);
    univ->changed = cell_pool_create(width * height);

    universe_foreach_pos(univ, x, y) {
        cell *cur_cell = universe_cell_at(univ, x, y);
        cur_cell->x = x;
        cur_cell->y = y;
        cur_cell->alive = seed[y * width + x];
        cur_cell->age = (cur_cell->alive) ? 1 : 0;
        universe_foreach_neighbor(univ, cur_cell, neighbor_x, neighbor_y) {
            if (seed[neighbor_y * width + neighbor_x]) {
                cur_cell->live_neighbors++;
                cell_pool_push(univ->changed, cur_cell);
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

    // Update cells based on the previous generation's dirty pool
    cell_pool_foreach(univ->changed, cur_cell) {
        bool next_state = cell_next_state(cur_cell);
        if (cur_cell->alive != next_state) {
            cur_cell->alive = next_state;
            cell_pool_push(univ->dirty, cur_cell);
        }
        cur_cell->age = (cur_cell->alive) ? cur_cell->age + 1 : 0;
    }

    // Update the live neighbor counts once everything has settled
    cell_pool_foreach(univ->dirty, cur_cell) {
        cur_cell->live_neighbors = 0;
        universe_foreach_neighbor_cell(univ, cur_cell, neighbor_cell) {
            if (neighbor_cell->alive) {
                cur_cell->live_neighbors++;
            }
        }
    }

    cell_pool *tmp = univ->changed;
    univ->changed = univ->dirty;
    univ->dirty = tmp;
    cell_pool_clear(univ->dirty);
}

void universe_destroy(universe *univ)
{
    cell_pool_destroy(univ->dirty);
    cell_pool_destroy(univ->changed);
    free(univ->cells);
    free(univ);
}