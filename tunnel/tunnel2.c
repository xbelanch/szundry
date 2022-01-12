#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512
#define TEXTURE_WIDTH 512
#define TEXTURE_HEIGHT 512

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include "./sdl_helper.h"

// Generate non-linear transformation table, now for the bigger buffers (twice as big)
struct LookupTable {
    unsigned int *distance;
    unsigned int *angle;
};

typedef struct LookupTable Table;

// Calculate lookup tables
// ======================
//
//
// Based on lodev.org/cgtutor/tunnel.html
// --------------------------------------
//
// The distance table contains for every pixel of the screen,
// the inverse of the distance to the center of the screen this pixel has.
//  This gives pixels of the center of the screen a very high value (those are very far away,
// as you can see on the picture above), while the pixels on the sides of the screen get
// a low value (these pixels represent parts of the tunnel close to the camera).
//
// The angle buffer calculates the angle of the current pixel
// (the angle it has to the pixel in the center of the screen),
// by using the atan2 function. The atan2 function belongs to the <cmath> header,
// and returns the angle in radians of a given point by giving its x and y coordinate.
// It's divided through pi, so that the texture will be wrapped exactly one time around the tunnel.

void createLookUpTable(Table *table, int widthScreen, int heightScreen)
{
    table->distance = malloc(sizeof(int) * widthScreen * heightScreen);
    table->angle = malloc(sizeof(int) * widthScreen * heightScreen);

    float big_number = (30.0 * TEXTURE_HEIGHT);
    for (int row = 0; row < heightScreen; ++row) {
        for (int col = 0; col < widthScreen; ++col) {
            float inverse_distance = sqrt(
                                          ((col - widthScreen / 2.0) * (col - widthScreen / 2.0))
                                          +
                                          ((row - heightScreen / 2.0) * (row - heightScreen / 2.0)));
            table->distance[row * widthScreen + col] = (unsigned int)(big_number / inverse_distance) % TEXTURE_HEIGHT;
            table->angle[row * widthScreen + col] = (unsigned int)(0.5 * TEXTURE_WIDTH *
                                                                    atan2(row - heightScreen / 2.0, col - widthScreen / 2.0) / 3.141516);
        }
    }
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv[0];
    Table tunnel = {0};
    createLookUpTable(&tunnel, WINDOW_WIDTH, WINDOW_HEIGHT);

    scc(SDL_Init(SDL_INIT_VIDEO));
    scv();
    SDL_Window *window =
        scp(SDL_CreateWindow("Tunnel Effect", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE));


    SDL_Renderer *renderer =
        scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
    scs(renderer);

    SDL_Surface *surface = scp(generateProceduralSurface(TEXTURE_WIDTH, TEXTURE_HEIGHT));
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, surface->w, surface->h);

    bool quit = false;
    while (!quit) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: {
                quit = true;
            } break;
            }
        }

        scc(SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff));
        scc(SDL_RenderClear(renderer));

        // Manipulate texture
        void *pixels;
        SDL_LockTexture(texture, 0, &pixels, &(surface->pitch));
        memcpy(pixels, surface->pixels, surface->w * surface->h * 4);
        Uint32 *buffer = (Uint32 *)pixels;
        Uint32 *pixel = surface->pixels;
        // Get or modify pixels
        for (size_t i = 0; i < TEXTURE_HEIGHT; ++i) {
            for (size_t j = 0; j < TEXTURE_WIDTH; ++j) {
                int c_x = (tunnel.distance[i * TEXTURE_WIDTH + j]) % TEXTURE_WIDTH;
                int c_y = (tunnel.angle[i * TEXTURE_WIDTH + j]) % TEXTURE_HEIGHT;
                // Get the pixel color from surface
                Uint32 color = pixel[c_y * surface->w + c_x];
                // and copy to buffer
                buffer[i * TEXTURE_WIDTH + j] = color;
            }
        }
        SDL_UnlockTexture(texture);
        // Copy texture to renderer
        scc(SDL_RenderCopy(renderer, texture, 0, 0));
        // Display VRAM data to Window
        SDL_RenderPresent(renderer);
        // Delay 100 milliseconds
        SDL_Delay(100);
    }

    // Destroy all the shitty stuff
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return (0);
}