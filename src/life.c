#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "life.h"

/* Design Notes:
 *
 *  - want to optimize for fast neighbor calculation
 *      - conceptual convolution: neighbor "mask" applied over a 2D matrix
 *      - SMP: partition matrix, dispatch to thread pool (1 thread per core)
 *      - design around cache utilization
 *          - three pointers to prefetch cache line-sized chunks of cells
 *  - cell matrix is a "realized" subset of the absolute (infinite) universe
 *      - absolute (universe) -> realized (local view) -> observable (rendered)
 *      - matrix allocates cells from center of (absolute origin)
 *      - matrix expands (doubles via realloc) when a cell becomes alive at bound
 *      - rows/cols to scan can be constrained (to avoid the outer, empty space)
 */

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

#ifndef L1_CACHE_SIZE
#define L1_CACHE_SIZE 32768
#endif

#ifndef L2_CACHE_SIZE
#define L2_CACHE_SIZE 262144
#endif

#ifndef L3_CACHE_SIZE
#define L3_CACHE_SIZE 4194304
#endif

/* A row is just a continguous set of cells next to each other, but purposefully
 * designed to fit in a single cache line (16 4-byte cells = 64 bytes). */
static const size_t ROW_SIZE = CACHE_LINE_SIZE / sizeof(cell);

/* L1 cache size = 32768 bytes / 64 bytes/row = 512 rows
 * 512 * 16 cells/rows = 8192 cells */
static const size_t BATCH_SIZE = L1_CACHE_SIZE / sizeof(cell);

/* The cell universe is designed to be allocated in terms of "pages" that fit
 * entirely in the L3 cache:
 *
 * L3 cache size = 4194304 bytes / 64 bytes/row = 65536 rows
 * 65536 rows * 16 cells/row = 1048576 cells (1024x1024)
 * 1048576 cells / 8192 cells/batch = 128 batches */
static const size_t CELL_PAGE_SIZE = L3_CACHE_SIZE / sizeof(cell);

/* This keeps track of 3 running pointers to logical rows of cells in a flattened
 * matrix of cells: a "middle" row with a cell of interest and 2 adjacent
 * neighboring cells, and the neighboring logical "top" and "bottom" rows. This
 * is effectively a subset of cells that can be used to compute neighbor counts
 * in a cache-efficient way: all cells needed for examining the neighbors of 14
 * cells (16 in the middle row, minus the 2 on the ends) can be accessed entirely
 * from 3 cache lines.
 *
 * NOTE: This is a *view* - it is not designed to accomodate mutation. */
typedef struct surrounding_ctx {
    cell *top;
    cell *middle;
    cell *bottom;
} surrounding_ctx;

static size_t page_aligned_grid_size(size_t min_width, size_t min_height)
{
    // We need a square grid, so we have to use the largest size constraint
    size_t dim = fmax(min_width, min_height);
    // We want the minimum number of pages that can hold the entire grid
    size_t area = dim * dim;
    return area + CELL_PAGE_SIZE - (area % CELL_PAGE_SIZE);
}

universe *create_universe(size_t min_width, size_t min_height)
{
    universe *univ = calloc(1, sizeof(*univ));
    size_t size = page_aligned_grid_size(min_width, min_height);
    univ->width = sqrt(size);
    univ->height = sqrt(size);

    void *ptrs[2];
    for (int i = 0; i < 2; i++) {
        int rc = posix_memalign(ptrs[i], CACHE_LINE_SIZE, size * sizeof(cell));
        if (rc) {
            fprintf(stderr, "posix_memalign failed: %d\n", rc);
            return NULL;
        }
        univ->cell_bufs[i] = ptrs[i];
    }

    return univ;
}

void universe_expand(universe *univ)
{
    // TODO - realloc cell buffers
    // TODO - figure out how to translate the cells so they remain in the "same place"
}

void universe_tick(universe *univ)
{
    // TODO
}

void destroy_universe(universe *univ)
{
    for (int i = 0; i < 2; i++)
        free(univ->cell_bufs[i]);
    free(univ);
}
