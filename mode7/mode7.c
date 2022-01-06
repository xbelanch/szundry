#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#define UNPACK_RGBA(color) (color>>(8 * 3) & 0xff),\
        (color>>(8 * 2) & 0xff),\
        (color>>(8 * 1) & 0xff),\
        (color>>(8 * 0) & 0xff)


void scc(int code)
{
    if (code < 0) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
}

void *scp(void *ptr)
{
    if (ptr == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

SDL_Surface *get_suface_from_file(const char *file_path)
{
    // Source code robbed from https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
    int req_format = STBI_rgb_alpha;
    int width, height, orig_format;
    unsigned char* pixels = stbi_load(file_path, &width, &height, &orig_format, req_format);
    if(pixels == NULL) {
        SDL_Log("Loading image failed: %s", stbi_failure_reason());
        exit(1);
    }

    // Set up the pixel format color masks for RGB(A) byte arrays.
    // Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (req_format == STBI_rgb) ? 8 : 0;
    const Uint32 rmask = 0xff000000 >> shift;
    const Uint32 gmask = 0x00ff0000 >> shift;
    const Uint32 bmask = 0x0000ff00 >> shift;
    const Uint32 amask = 0x000000ff >> shift;
#else // little endian, like x86
    const Uint32 rmask = 0x000000ff;
    const Uint32 gmask = 0x0000ff00;
    const Uint32 bmask = 0x00ff0000;
    const Uint32 amask = (req_format == STBI_rgb) ? 0 : 0xff000000;
#endif

    // STBI_rgb_alpha (RGBA)
    int depth = 32;
    int pitch = 4*width;

    return scp(SDL_CreateRGBSurfaceFrom((void*)pixels, width, height, depth, pitch, rmask, gmask, bmask, amask));

}

int ScreenWidth()
{
    return WINDOW_WIDTH * 4;
}

int ScreenHeight()
{
    return WINDOW_HEIGHT * 4;
}


SDL_Texture *spr_ground;
size_t nMapSize = 256; // 1024 later!
float fWorldX = 0.0f;
float fWorldY = 0.0f;
float fWorldA = 0.0f;

float fNear = 0.01f;
float fFar = 0.01f;
float fFoVHalf = 3.14159f / 4.0f;

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv[0];

    scc(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window *window =
        scp(SDL_CreateWindow("MODE7", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE));

    SDL_Renderer *renderer =
        scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

    SDL_Surface *surface_bitmap_map = get_suface_from_file("./m7_map.png");
    spr_ground = scp(SDL_CreateTextureFromSurface(renderer, surface_bitmap_map));
    SDL_FreeSurface(surface_bitmap_map);

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

		// Create Frustum corner points
		float fFarX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fFar;
		float fFarY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fFar;

		float fNearX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fNear;
		float fNearY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fNear;

		float fFarX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fFar;
		float fFarY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fFar;

		float fNearX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fNear;
		float fNearY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fNear;

        // Starting with furthest away line and work towards the camera point
		for (int y = 0; y < (ScreenHeight() / 2); y++) {
			// Take a sample point for depth linearly related to rows down screen
			float fSampleDepth = (float)y / ((float)ScreenHeight() / 2.0f);

			// Use sample point in non-linear (1/x) way to enable perspective
			// and grab start and end points for lines across the screen
			float fStartX = (fFarX1 - fNearX1) / (fSampleDepth) + fNearX1;
			float fStartY = (fFarY1 - fNearY1) / (fSampleDepth) + fNearY1;
			float fEndX = (fFarX2 - fNearX2) / (fSampleDepth) + fNearX2;
			float fEndY = (fFarY2 - fNearY2) / (fSampleDepth) + fNearY2;

			// Linearly interpolate lines across the screen
			for (int x = 0; x < ScreenWidth() ; x++) {
				float fSampleWidth = (float)x / (float)ScreenWidth();
				float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
				float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

				// Wrap sample coordinates to give "infinite" periodicity on maps
				fSampleX = fmod(fSampleX, 1.0f);
				fSampleY = fmod(fSampleY, 1.0f);

				// Sample symbol and colour from map sprite, and draw the
				// pixel to the screen
				// wchar_t sym = sprGround->SampleGlyph(fSampleX, fSampleY);
				// short col = sprGround->SampleColour(fSampleX, fSampleY);
				// Draw(x, y + (ScreenHeight() / 2), sym, col);

            }

        }


        scc(SDL_SetRenderDrawColor(renderer, UNPACK_RGBA(0xff00ffff)));
        scc(SDL_RenderClear(renderer));
        scc(SDL_RenderCopy(renderer, spr_ground, 0, 0));
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return (0);
}