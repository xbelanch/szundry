#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

// SDL Check Code
int scc(int code)
{
    if (code < 0) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    } else return code;
}

// SDL Check Pointer
void *scp(void *ptr)
{
    if (ptr == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    } else return ptr;
}

// Check video display and drivers before SDL initialization
void scv()
{
    int dps = scc(SDL_GetNumVideoDisplays());
    printf("Number of video displays available: %d\n"
           "=====================================\n", dps);
    int dvrs = scc(SDL_GetNumVideoDrivers());
    printf("Number of video drivers avalaible: %d\n"
           "====================================\n", dvrs);
    for (int i = 0; i < dvrs; ++i) {
        if (strcmp(SDL_GetVideoDriver(i), SDL_GetCurrentVideoDriver()) == 0) {
            printf("%d: %s(*)\n", i, SDL_GetCurrentVideoDriver());
        } else {
            printf("%d: %s\n", i, SDL_GetVideoDriver(i));
        }
    }
}

// Check SDL Screen and renderer drivers
void scs(SDL_Renderer *renderer)
{
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	int num = scc(SDL_GetNumRenderDrivers());
    printf("Number of rendering drivers: %d\n"
           "==============================\n", num);

	for (int i = 0; i < num; i++) {
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(i, &info);
        if (i == 0) {
            printf("%d: %s(*) [%d, %d, %d]\n", i, info.name, info.num_texture_formats, info.max_texture_height, info.max_texture_height);
        } else {
            printf("%d: %s\n", i, info.name);
        }
	}
}

SDL_Surface *generateProceduralSurface(size_t width, size_t height)
{
    /* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
       as expected by OpenGL for textures */
    SDL_Surface *surface;
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_LockSurface(surface);
    SDL_memset(surface->pixels, 0, height * width);
    Uint32* pixels = (Uint32*)surface->pixels;

    for (int j = 0; j < surface->h; ++j) {
        for (int i = 0 ; i < surface->w; ++i) {
            pixels[j * surface->w + i] = (i ^ j) << 24 | (i ^ j) << 16 | (i ^ j) << 8 | 0xff;

        }
    }

    SDL_UnlockSurface(surface);
    return surface;
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv[0];

    scc(SDL_Init(SDL_INIT_VIDEO));
    scv();
    SDL_Window *window =
        scp(SDL_CreateWindow("Tunnel Effect", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE));

    SDL_Renderer *renderer =
        scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
    scs(renderer);

    SDL_Surface *surface = scp(generateProceduralSurface(WINDOW_WIDTH, WINDOW_HEIGHT));
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
        Uint32 *upixels = (Uint32 *)pixels;
        // Get or modify pixels
        #define TILE_W 32
        #define TILE_H 32
        for (size_t i = 0; i < TILE_H; ++i) {
            for (size_t j = 0; j < TILE_W; ++j) {
                upixels[i * surface->w + j] = 0xff00ffff;
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



// -- TUNNEL_

// void generateTexture(Tunnel* tunnel)
// {
// 	// generate tunnel texture table
// 	/*
// 	for (int y = 0; y < tunnel->bitmap->height * 2; y++)
// 	{
// 		for (int x = 0; x < tunnel->bitmap->width * 2; x++)
// 		{
// 			// basic texture
// 			tunnel->texture[y][x] =  x ^ y ;
// 			// tunnel->texture[y][x] = ((y & 0x0f) << 0x4) << 8 | ((x & 15) * 16);
// 			// checker board
// 			int idx = (((x ^ y) >> 4) & 1);
// 			if (idx == 1)
// 			{
// 				tunnel->texture[y][x] = 0x0;
// 			}
// 			else {
// 				tunnel->texture[y][x] = 0xff << 16 | 0xff << 8 | 0x0; // YELLOW
// 			}
// 		}
// 	}

// 	*/
// 	// generate texture from a png file
// 	SDL_Surface* surface = NULL;
// 	surface = IMG_Load("assets/rotter_lyud_inside2.png");
// 	if (NULL == surface)
// 		ERROR("IMG_Load: %s\n", IMG_GetError());

// 	int bpp = surface->format->BytesPerPixel;
// 	LOG("- BPP equal to %d bytes per pixel\n", bpp);


// 	for (int y = 0; y < tunnel->bitmap->height; y++)
// 	{
// 		// y = y % texHeight;

// 		for (int x = 0; x < tunnel->bitmap->width; x++)
// 		{
// 			// x = x % texWidth;

// 			Uint8 *p = (Uint8 *)surface->pixels + (y % texHeight) * surface->pitch + (x % texWidth) * bpp;

// 			switch (bpp) {
// 				case 1:
// 					tunnel->texture[y][x] = *p;
// 					break;

// 				case 2:
// 					tunnel->texture[y][x] = *(Uint16 *)p;
// 					break;

// 				case 3:
// 					if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
// 						tunnel->texture[y][x] = p[0] << 16 | p[2] << 8 | p[1];
// 					else
// 						tunnel->texture[y][x] = p[0] | p[1] << 8 | p[2] << 16;
// 					break;

// 				case 4:

// 					// uint32_t color = *(Uint32 *)p; //
// 					// rgba
// 					uint32_t color = p[0] << 16 | p[1] <<  8| p[2];
// 					tunnel->texture[y][x] = color;
// 					break;
// 			}
// 		}

// 	}

// }


// //generate non-linear transformation table, now for the bigger buffers (twice as big)

// void generateDistanceTable(Tunnel* tunnel, Screen *screen)
// {

// 	for (int y = 0; y < tunnel->bitmap->height * 2;  y++)
// 	{

// 		for (int x = 0; x < tunnel->bitmap->width * 2; x++)
// 		{
// 			int angle, distance;
// 			float ratio = 64.0;


// 			distance = (int)(ratio * texHeight / sqrt((float)((x - screen->render_w) * (x - screen->render_w) + (y - screen->render_h) * (y - screen->render_h)))) % texHeight;
// 			angle = (unsigned int)(0.5 * texWidth * atan2((float)(y - screen->render_h), (float)(x - screen->render_w)) / 3.1416);


// 			tunnel->distanceTable[y][x] = distance;
// 			tunnel->angleTable[y][x] = angle;
// 		}
// 	}
// }

// void updateTunnel(Tunnel *tunnel, Screen *screen)
// {

// 	float animation = gTimer.ticks / 1e+3;
// 	int shiftX = (int)(texWidth * 1.3 * animation);
// 	int shiftY = (int)(texHeight * 0.25 * animation);

// 	int shiftLookX = screen->render_w / 2 + (int)(screen->render_w / 2 * sin(animation));
// 	int shiftLookY = screen->render_h / 2 + (int)(screen->render_h / 2 * sin(animation * 2.0));

// 	SDL_LockTexture(tunnel->bitmap->texture, NULL, &(tunnel->bitmap->buffer), &(tunnel->bitmap->pitch));

// 	// time to update the buffer of bitmap
// 	for (int y = 0; y < tunnel->bitmap->height; y++)
// 	{

// 		for (int x = 0; x < tunnel->bitmap->width; x++)
// 		{
// 			int color = tunnel->texture[(unsigned int)(tunnel->distanceTable[x + shiftLookX][y + shiftLookY] + shiftX) % texWidth][(unsigned int)(tunnel->angleTable[x + shiftLookX][y + shiftLookY] + shiftY) % texHeight];
// 			tunnel->bitmap->buffer[y * tunnel->bitmap->width + x] = color;


// 		}
// 	}

// 	SDL_UnlockTexture(tunnel->bitmap->texture);
// }

// void renderTunnel(Tunnel *tunnel, Screen *screen)
// {
// 	SDL_RenderCopy(
// 		screen->renderer,
// 		tunnel->bitmap->texture,
// 		&(SDL_Rect){ 0, 0, tunnel->bitmap->width, tunnel->bitmap->height},
// 		&(SDL_Rect){ 0, 0, screen->render_w * screen->scale, screen->render_h * screen->scale}
// 	);
// }
// Tunnel* createTunnelEffect(Screen* screen)
// {
// 	Tunnel* tunnel = malloc(sizeof(Tunnel));
// 	SDL_memset(tunnel, 0, sizeof(Tunnel));

// 	// initialize bitmap member
// 	tunnel->bitmap = malloc(sizeof(Bitmap));
// 	SDL_memset(tunnel->bitmap, 0, sizeof(Bitmap));

// 	tunnel->bitmap->x = 0;
// 	tunnel->bitmap->y = 0;
// 	tunnel->bitmap->width = 640;
// 	tunnel->bitmap->height = 480;
// 	tunnel->bitmap->id = "Tunnel effect";

// 	// generate pattern texture
// 	generateTexture(tunnel);

// 	// generate table distance
// 	generateDistanceTable(tunnel, screen);

// 	// LOG
// 	LOG("- Bitmap %s created (%d)\n", tunnel->bitmap->id, &(tunnel->bitmap));

// 	// Create texture
// 	tunnel->bitmap->texture = SDL_CreateTexture(
// 		screen->renderer,
// 		SDL_PIXELFORMAT_BGRA32,
// 		SDL_TEXTUREACCESS_STREAMING,
// 		tunnel->bitmap->width,
// 		tunnel->bitmap->height);

// 	return tunnel;

// }
// #endif // !TUNNEL_H


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <stdbool.h>
// #include <string.h>

// #ifdef __unix__
// #include <SDL2/SDL.h>
// #elif _WIN32
// #include "SDL.h"
// #else
// // OSX?
// #endif

// #ifdef __EMSCRIPTEN__
// #include <emscripten.h>
// #endif




// #define WIDTH 160
// #define HEIGHT 120
// #define SCALE 4
// #define GAME_TITLE "Escape of the Chamber"



// int main(int argc, char* argv[])
// {

// 	SDL_Window* Window;
// 	SDL_Renderer* Renderer;
// 	SDL_Surface* Bitmap;
// 	SDL_Texture* Texture;

// 	bool success = true;
// 	bool isGameRunning = true;

// 	// Initialize SDL subsystem
// 	if (SDL_Init(SDL_INIT_VIDEO) < 0)
// 	{
// 		success = false;
// 	}
// 	else {
// 		Window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_SHOWN);
// 		Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

// 		if (NULL != Window)
// 		{
// 			// Create random pixel bitmap
// 			Bitmap = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);

// 			if (NULL != Bitmap)
// 			{
// 				// the value of bytesperpixel is equal to 4 bytes (0, 8, 16, 32)
// 				int bytesperpixel = Bitmap->format->BytesPerPixel;
// 				fprintf(stdout, "Value of bytesperpixel: %d\n", bytesperpixel);
// 				fprintf(stdout, "Value of sizeof(Uint32): %d\n", sizeof(Uint32));


// 				Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

// 				// Initialize texture pixels to a red opaque RGBA value
// 				Uint32* pixels = NULL;

// 				for (int steps = 0; steps < 16; steps++)
// 				{

// 					// set random pixels to bitmap
// 					/*
// 					for (int row = 0; row < Bitmap->h; row++)
// 					{
// 						for (int col = 0; col < Bitmap->w; col++)
// 						{
// 							// extract pointer pixel data address from Bitmap
// 							// Since the pixel to be written really is 32 - bit, the pointer must be 32 - bit to make it a single write.
// 							// First method -ugly-

// 							Uint32* pixel = (Uint8 *)Bitmap->pixels + (row * Bitmap->pitch + (col * sizeof(Uint32)));
// 							// set pixel color value
// 							*pixel = (rand() % 256 << 16) + (rand() % 256 << 8) + (rand() % 256);

// 							// Second method is much more clearer
// 							// pointer to the first pixel color of the Bitmap
// 							Uint32* pixel = Bitmap->pixels;
// 							pixel[row * Bitmap->w + col] = (rand() % 256 << 16) + (rand() % 256 << 8) + (rand() % 256);
// 						}
// 					}
// 					*/

// 					//

// 					// Transfer data from surface Bitmap to texture
// 					//Texture = SDL_CreateTextureFromSurface(Renderer, Bitmap);
// 					// This could be slower than create the texture from a simple buffer
// 					// https://stackoverflow.com/questions/26698378/how-to-use-sdl-createtexture
// 					/*
// 					SDL_SetRenderDrawColor(Renderer, 255, 0, 255, 255);
// 					SDL_Rect rectangle;

// 					rectangle.x = 0;
// 					rectangle.y = 0;
// 					rectangle.w = 256;
// 					rectangle.h = 256;
// 					SDL_RenderFillRect(Renderer, &rectangle);

// 					*/


// 					// Clear the renderer
// 					SDL_RenderClear(Renderer);

// 					int pitch = 0;
// 					// https://stackoverflow.com/questions/1382051/what-is-the-c-equivalent-for-reinterpret-cast
// 					SDL_LockTexture(Texture, NULL, (Uint32 *)(&pixels), &pitch);
// 					// Uint32 _rgba = 0xff << 32 + 0xff << 16 + 0x0 << 8 + 0;


// 					for (int y = 0; y < HEIGHT; y++)
// 					{
// 						for (int x = 0; x < WIDTH; x++)
// 						{
// 							// Alpha + Red + Green + Blue
// 							pixels[y * WIDTH + x]  = (0x0 << 24) + (rand() % 0xff << 16) + ( 0 << 8) + 0;
// 						}
// 					}

// 					SDL_UnlockTexture(Texture);
// 					// Copy texture to VRAM
// 					SDL_RenderCopy(Renderer, Texture, NULL, NULL);

// 					// Display VRAM data -> Window
// 					SDL_RenderPresent(Renderer);
// 					// delay 100 milliseconds
// 					SDL_Delay(100);

// 				}
// 			}
// 		}

// 		// We can burn Bitmap memory

// 		SDL_RenderPresent(Renderer);
// 		fprintf(stdout, "You should see an random pixel picture.\n");

// 		// Destroy the texture
// 		// Free memory for all of us!
// #if !defined __EMSCRIPTEN__
// 		fprintf(stdout, "Clean everything.\n");
// 		SDL_FreeSurface(Bitmap);
// 		SDL_DestroyTexture(Texture);
// 		// SDL_DestroyRenderer(Renderer);
// 		SDL_DestroyWindow(Window);
// 		SDL_Quit();
// #endif //


// 	}

// 	return success;

// }