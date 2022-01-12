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