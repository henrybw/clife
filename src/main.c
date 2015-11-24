#include "life.h"

#include <SDL.h>
#include <stdio.h>

int main()
{
    // Glider
    bool seed[] = {
        false, false, false, false, false, false, false, false, false, false,
        false, false, true,  false, false, false, false, false, false, false,
        false, false, false, true,  false, false, false, false, false, false,
        false, true,  true,  true,  false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false
    };
    universe *univ = universe_create(10, 10, seed);

    // TODO HBW - actually do something here instead of just dumping the
    // state of the universe...
    universe_foreach_cell(univ, cur_cell) {
        printf("cell (%d, %d) is %s and has %zu live neighbors\n",
               cur_cell->x, cur_cell->y,
               cur_cell->alive ? "alive" : "dead",
               cur_cell->live_neighbors);
        universe_foreach_neighbor_cell(univ, cur_cell, neighbor_cell) {
            printf("\tneighbor at (%d, %d) is %s\n",
                   neighbor_cell->x, neighbor_cell->y,
                   neighbor_cell->alive ? "alive" : "dead");
        }
    }

    universe_destroy(univ);
    return 0;
}
