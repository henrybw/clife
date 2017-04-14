#include <SDL.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "life.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static const size_t WIDTH = 1024;
static const size_t HEIGHT = 768;
static const uint8_t PIXEL_SIZE = 1;
#define FG_COLOR 85, 255, 55
#define BG_COLOR 4, 45, 55
// static const uint8_t CELL_MIN_ALPHA = 50;
// static const double CELL_GRADIENT = 10.0;

int main()
{
    int status = EXIT_SUCCESS;
    int rc;
    if ((rc = SDL_Init(SDL_INIT_EVERYTHING)) != 0) {
        fprintf(stderr, "SDL_Init failed with return code %d\n", rc);
        status = EXIT_FAILURE;
        goto sdl_failed;
    }

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    printf("Screen size is %d, %d\n", dm.w, dm.h);

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
                                            //SDL_RENDERER_PRESENTVSYNC |
                                            SDL_RENDERER_TARGETTEXTURE);
    if (!rend) {
        fprintf(stderr, "SDL_CreateRenderer failed with error %s\n",
                SDL_GetError());
        status = EXIT_FAILURE;
        goto renderer_failed;
    }

    bool seed[WIDTH * HEIGHT] = { 0 };
    // seed[1 * WIDTH + 2] = true;
    // seed[2 * WIDTH + 3] = true;
    // seed[3 * WIDTH + 1] = true;
    // seed[3 * WIDTH + 2] = true;
    // seed[3 * WIDTH + 3] = true;
    srand(time(NULL));
    for (size_t i = 0; i < WIDTH * HEIGHT; i++)
        seed[i] = (rand() % 2);

    bool done = false;
    uint32_t next_fps_tick = SDL_GetTicks() + 1000;
    uint32_t fps = 0;
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
                    // SDL_MouseMotionEvent *motion =
                    //     (SDL_MouseMotionEvent *)&event;
                    // uint32_t x = motion->x / PIXEL_SIZE;
                    // uint32_t y = motion->y / PIXEL_SIZE;
                }
            }
        }
        SDL_SetRenderDrawColor(rend, BG_COLOR, 255);
        SDL_RenderClear(rend);

        SDL_RenderPresent(rend);
        fps++;

        if (SDL_TICKS_PASSED(SDL_GetTicks(), next_fps_tick)) {
            printf("FPS: %d\n", fps);
            fps = 0;
            next_fps_tick = SDL_GetTicks() + 1000;
        }
    }

    SDL_DestroyRenderer(rend);
renderer_failed:
    SDL_DestroyWindow(win);
window_failed:
    SDL_Quit();
sdl_failed:
    return status;
}
