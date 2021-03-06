#include <SDL.h>
#include <stdio.h>
#include <unistd.h>

#include "life.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static const size_t WIDTH = 128;
static const size_t HEIGHT = 98;
static const uint8_t PIXEL_SIZE = 8;
#define FG_COLOR 85, 255, 55
#define BG_COLOR 4, 45, 55
static const uint8_t CELL_MIN_ALPHA = 50;
static const double CELL_GRADIENT = 10.0;

void cell_render(cell *cur_cell, SDL_Renderer *rend)
{
    SDL_Rect cell_box = {
        .x = cur_cell->x * PIXEL_SIZE,
        .y = cur_cell->y * PIXEL_SIZE,
        .w = PIXEL_SIZE,
        .h = PIXEL_SIZE
    };
    double step = MAX(CELL_GRADIENT - cur_cell->age + 1, 1);
    uint8_t alpha = MAX((255.0 * step) / CELL_GRADIENT, CELL_MIN_ALPHA);
    SDL_SetRenderDrawColor(rend, FG_COLOR, alpha);
    SDL_RenderFillRect(rend, &cell_box);
}

void universe_render(universe *univ, SDL_Renderer *rend)
{
    universe_foreach_cell(univ, cur_cell) {
        if (cur_cell->alive) {
            cell_render(cur_cell, rend);
        }
    }
}

int main()
{
    int status = EXIT_SUCCESS;
    int rc;
    if ((rc = SDL_Init(SDL_INIT_EVERYTHING)) != 0) {
        fprintf(stderr, "SDL_Init failed with return code %d\n", rc);
        status = EXIT_FAILURE;
        goto sdl_failed;
    }

    SDL_Window *win = SDL_CreateWindow("Game of Life", 15, 15,
                                       WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE,
                                       SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow failed with error %s\n", SDL_GetError());
        status = EXIT_FAILURE;
        goto window_failed;
    }

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1,
                                            SDL_RENDERER_ACCELERATED |
                                            SDL_RENDERER_PRESENTVSYNC |
                                            SDL_RENDERER_TARGETTEXTURE);
    if (!rend) {
        fprintf(stderr, "SDL_CreateRenderer failed with error %s\n",
                SDL_GetError());
        status = EXIT_FAILURE;
        goto renderer_failed;
    }

    bool seed[WIDTH * HEIGHT] = { 0 };
    seed[1 * WIDTH + 2] = true;
    seed[2 * WIDTH + 3] = true;
    seed[3 * WIDTH + 1] = true;
    seed[3 * WIDTH + 2] = true;
    seed[3 * WIDTH + 3] = true;
    universe *univ = universe_create(WIDTH, HEIGHT, seed);
    bool done = false;

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN &&
                 event.key.keysym.sym == SDLK_q)) {
                done = true;
                break;
            } else if (event.type == SDL_MOUSEMOTION) {
                if (SDL_GetMouseState(NULL, NULL) &&
                    SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    SDL_MouseMotionEvent *motion =
                        (SDL_MouseMotionEvent *)&event;
                    uint32_t x = motion->x / PIXEL_SIZE;
                    uint32_t y = motion->y / PIXEL_SIZE;
                    cell *tgt_cell = universe_cell_at(univ, x, y);
                    tgt_cell->alive = true;
                    tgt_cell->age = 1;
                    cell_render(tgt_cell, rend);
                }
            }
        }
        SDL_SetRenderDrawColor(rend, BG_COLOR, 255);
        SDL_RenderClear(rend);

        universe_render(univ, rend);
        universe_evolve(univ);

        SDL_RenderPresent(rend);
    }

    universe_destroy(univ);
    SDL_DestroyRenderer(rend);
renderer_failed:
    SDL_DestroyWindow(win);
window_failed:
    SDL_Quit();
sdl_failed:
    return status;
}
