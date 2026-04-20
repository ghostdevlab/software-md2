#include <iostream>
#include <SDL.h>
#include "Image.h"

typedef struct Screen {
    SDL_Window *window;
    SDL_Surface *canvas;
    SDL_Surface *pixels;

    int w, h;

    Image* image;
} TScreen;


void toggleFullscreen(SDL_Window* Window) {
    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    bool IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
    SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag);
    SDL_ShowCursor(IsFullscreen);
}

void createWindow(TScreen& out, int width, int height, int fullscreen) {
    SDL_Init(SDL_INIT_VIDEO);
    out.window = SDL_CreateWindow("SDL pixels",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  width, height,
                                  SDL_WINDOW_SHOWN);

    if (fullscreen) {
        toggleFullscreen(out.window);
    }

    out.canvas = SDL_GetWindowSurface(out.window);
    out.pixels = SDL_CreateRGBSurfaceWithFormat(0, width, height, 16, SDL_PIXELFORMAT_RGB565);

    out.w = out.pixels->w;
    out.h = out.pixels->h;

    out.image = new Image(out.w, out.h, (uint16_t*)out.pixels->pixels);

//    initZBuf(out.zBuf, out.w, out.h);
}

void lock(TScreen& screen) {
    SDL_LockSurface(screen.pixels);
//    screen.image.buf = (unsigned char *) screen.pixels->pixels;
//    screen.image.pitch = screen.pixels->pitch;

}

void unlock(TScreen& screen) {
    SDL_UnlockSurface(screen.pixels);
}

void clear(TScreen& screen) {
    screen.image->clear();
//    memset(screen.image, 0, screen.w * screen.h * sizeof(unsigned short));
//    resetZBuf(screen.zBuf);
}

int main() {
    TScreen screen;
#ifdef DEBUG
    int debug = 1;
#else
    int debug = 0;
#endif

    if (debug) {
        createWindow(screen, 1024, 576, false);
    } else {
//        createWindow(screen, 1024, 576, true);
        createWindow(screen, 1920, 1080, true);
    }

    int quit = 0;
    unsigned int t1 = SDL_GetTicks();
    unsigned long frames = 0;
    unsigned int start = SDL_GetTicks();


    while (!quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }

//            handleKeyEvents(ev);

            if (ev.type == SDL_QUIT) {
                quit = true;
            }
        }

        unsigned int t2 = SDL_GetTicks();
        float dt = (t2 - t1) / 1000.0f;
        t1 = t2;
        frames++;

        lock(screen);
        int totalTris = 100;
        for(int i = 0; i < 100; i++) {
            GouraudTrianglePoint trianglePoint[3] = {
                    { 100 + i, 50,  RGB565(255, 0, 0) },
                    {  20 + i, 100, RGB565(0, 255, 0) },
                    { 300 + i, 500, RGB565(0, 0, 255) }
            };


            screen.image->drawGouraudTriangle(trianglePoint, sizeof(FlatTrianglePoint));
        }

        unlock(screen);

        // copy to window
        SDL_BlitSurface(screen.pixels, NULL, screen.canvas, NULL);
        SDL_UpdateWindowSurface(screen.window);


        char str[256];

        sprintf(str, "FPS: %5.2lf, tris %d", (1000.0 * frames / (SDL_GetTicks() - start)), totalTris);

        if (*str && (frames % 100) == 0) {
            std::cout<<str<<std::endl;
        }

    }

    return 0;
}
