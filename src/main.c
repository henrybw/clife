#include "life.h"

#include <SDL.h>
#include <stdio.h>
#include <unistd.h>

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

    for (int i = 0; i < 10; i++) {
        universe_print(univ);
        universe_evolve(univ);
        sleep(1);
    }

    universe_destroy(univ);
    return 0;
}
