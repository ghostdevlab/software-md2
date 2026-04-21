#include <iostream>
#include <SDL.h>
#include "Image.h"
#include "Pak.h"
#include "Matrix.h"

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

#ifdef main
#undef main
#endif

int main() {
//    dumpFileList("assets/pak0.pak");
    TQ2Model* model = loadModel("assets/pak0.pak", "models/monsters/gunner/tris.md2");

    TQ2ModelFrame* modelFrame = allocateFrame(model);
    TQ2ModelFrame* modelProjectedFrame = allocateFrame(model);

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

    Image* texture = generateTexture(RGB565(255, 0, 0), RGB565(0, 0, 255));

    int modelProgress = 0;
    int speed = 30;

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

        clear(screen);
        modelProgress++;
//        modelProgress = 200;
        modelProgress = modelProgress % (model->frameCount * speed);
        int modelCurrentFrame = (modelProgress/speed);
        int modelNextFrame = (modelCurrentFrame + 1) % model->frameCount;
        getTModel(*model, *modelFrame, modelCurrentFrame, modelNextFrame, (float)(modelProgress % speed) / speed);
        Matrix projection, shift, scale, rotY;
        projection.basicProjection(256, screen.w, screen.h);
        scale.setScale(0.3f);
        rotY.setRotationY(4 * 3.14/5);
        shift.setTransform(0, 0, 15);

        Matrix composition = projection * shift * scale * rotY;
        composition.mul(
                (float*)modelFrame->vertexes,
                (float*)modelProjectedFrame->vertexes,
                modelFrame->vertCount,
                6);

        for(int i = 0; i<modelFrame->trisCount; i++) {
            TexTrianglePoint texTrianglePoint[3] = {
                    {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 0].x / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 0].y / modelProjectedFrame->vertexes[i * 3 + 0].w),
                            modelProjectedFrame->vertexes[i * 3 + 0].u, modelProjectedFrame->vertexes[i * 3 + 0].v
                    },
                    {
                            (int32_t) (modelProjectedFrame->vertexes[i * 3 + 1].x / modelProjectedFrame->vertexes[i * 3 + 1].w),
                            (int32_t )(modelProjectedFrame->vertexes[i * 3 + 1].y / modelProjectedFrame->vertexes[i * 3 + 1].w),
                            modelProjectedFrame->vertexes[i * 3 + 1].u, modelProjectedFrame->vertexes[i * 3 + 1].v
                    },
                    {
                            (int32_t) (modelProjectedFrame->vertexes[i * 3 + 2].x / modelProjectedFrame->vertexes[i * 3 + 2].w),
                            (int32_t )(modelProjectedFrame->vertexes[i * 3 + 2].y / modelProjectedFrame->vertexes[i * 3 + 2].w),
                            modelProjectedFrame->vertexes[i * 3 + 2].u, modelProjectedFrame->vertexes[i * 3 + 2].v
                    }

            };
            screen.image->drawTexTriangle(texture, texTrianglePoint, sizeof(TexTrianglePoint));
//            screen.image->drawWireframeTriangle((WireframePoint*)texTrianglePoint, sizeof(TexTrianglePoint), RGB565(0, 255, 0));
        }

        int totalTris = 100;
//        for(int i = 0; i < 100; i++) {
//            GouraudTrianglePoint trianglePoint[3] = {
//                    { 100 + i, 50,  RGB565(255, 0, 0) },
//                    {  20 + i, 100, RGB565(0, 255, 0) },
//                    { 300 + i, 500, RGB565(0, 0, 255) }
//            };
//
//            screen.image->drawGouraudTriangle(trianglePoint, sizeof(GouraudTrianglePoint ));
//        }

        for(int i = 0; i < 1; i++) {
            TexTrianglePoint trianglePoint[3] = {
                    { 100 + i, 50,  0.0f, 0.0f },
                    {  20 + i, 100, 0.0f, 1.0f, },
                    { 300 + i, 500, 1.0f, 1.0f }
            };
//            screen.image->drawTexTriangle(texture, trianglePoint, sizeof(TexTrianglePoint ));
//            screen.image->drawWireframeTriangle((WireframePoint*)trianglePoint, sizeof(TexTrianglePoint), RGB565(0, 255, 0));
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
