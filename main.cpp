#include <iostream>
#include <SDL.h>
#include <map>
#include <SDL_image.h>
#include "Image.h"
#include "Pak.h"
#include "Matrix.h"
#include "Q2Anim.h"
#include "Enemy.h"

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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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

// Klucz to unikalna kombinacja freq/format/channels
std::map<uint32_t, SDL_AudioDeviceID> audioDevices;

void play(WavFile* wavFile) {
    if (true) return;
    SDL_AudioSpec wavSpec;
    Uint8 *wavBuffer;
    Uint32 wavLength;

    if (SDL_LoadWAV_RW(SDL_RWFromMem(wavFile->data, wavFile->size), 1, &wavSpec, &wavBuffer, &wavLength)) {
        // Tworzymy prosty klucz (np. freq | format << 16)
        uint32_t key = wavSpec.freq ^ wavSpec.format ^ wavSpec.channels;

        if (audioDevices.find(key) == audioDevices.end()) {
            // Pierwszy raz widzimy ten format - otwieramy nowe urządzenie
            audioDevices[key] = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
            SDL_PauseAudioDevice(audioDevices[key], 0);
        }

        SDL_QueueAudio(audioDevices[key], wavBuffer, wavLength);

        // Pamiętaj: SDL_FreeWAV można wywołać dopiero gdy dźwięk skończy grać,
        // albo trzymać bufory w cache (mapie) razem z deviceId.
    }
}


WavFile* sounds[20];


int main() {
    dumpFileList("assets/pak0.pak");

    int enemyIndex = SOLDIER;

    TEnemyAsset* enemyAsset = loadEnemyAsset(enemyPakDefinition + enemyIndex);
    TEnemy* enemy = createEnemy(enemyAsset);

    int index = 0;
    while(enemyPakDefinition[enemyIndex].sounds[index]) {
        sounds[index] = loadWav("assets/pak0.pak", enemyPakDefinition[enemyIndex].sounds[index]);
        index++;
    }
    sounds[index] = nullptr;

    TScreen screen;
#ifdef DEBUG
    int debug = 1;
#else
    int debug = 0;
#endif

    if (debug) {
        createWindow(screen, 1024, 576, false);
    } else {
        createWindow(screen, 1024, 576, false);
//        createWindow(screen, 1920, 1080, true);
    }

    int quit = 0;
    unsigned int t1 = SDL_GetTicks();
    unsigned long frames = 0;
    unsigned int start = SDL_GetTicks();

//    Image* texture = generateTexture(RGB565(255, 0, 0), RGB565(0, 0, 255));
//    Image* texture = loadPCX("assets/pak0.pak", "models/monsters/gunner/skin.pcx");

    int soundIndex = 0;
    int drawType = 4;

    play(sounds[8]);
    int lastStep = -1;
    float totalTime = 0.0f;

    while (!quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }

            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_1) {
                printf("sound %d\n", soundIndex);
                play(sounds[soundIndex]);
                soundIndex++;
                if (sounds[soundIndex] == nullptr) soundIndex = 0;
            }

            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_2) {
                enemy->isHurt = !enemy->isHurt;
            }

            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_4) {
                drawType = (drawType + 1) % 6;
            }

            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_3) {
                enemy->targetAnimNo = enemy->animationNo + 1;
//                enemy->animationNo++;
//                if (enemy->animationNo >= enemy->asset->animList->animCount) {
//                    enemy->animationNo = 0;
//                }
//                enemy->modelProgress = enemy->asset->animList->anim[enemy->animationNo].startFrame * enemy->speed;
//
//                if (strncmp("death", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
//                    play(sounds[0]);
//                }
//
//                if (strncmp("pain", enemy->asset->animList->anim[enemy->animationNo].name, 4) == 0) {
//                    play(sounds[5 + rand()%2]);
//                }
//
//                if (strncmp("attak1", enemy->asset->animList->anim[enemy->animationNo].name, 6) == 0) {
//                    play(sounds[3]);
//                } else
//                if (strncmp("attak", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
//                    play(sounds[1 + rand()%3]);
//                }
            }

//            handleKeyEvents(ev);

            if (ev.type == SDL_QUIT) {
                quit = true;
            }
        }

        unsigned int t2 = SDL_GetTicks();
        float dt = (float)(t2 - t1) / 1000.0f;
        t1 = t2;
        frames++;

//        printf("total time %5.3lf\n", totalTime);

        // constant dt for video
//        dt = 0.0055f;

        totalTime += dt;

        static float animSpeed = (float)enemy->speed/60.0f;

        static float deathStartTime = 0.0f;

        if (lastStep >= 9) {
            animSpeed = 0.2f;
            if (totalTime > deathStartTime + 0.3 && totalTime < deathStartTime + 1.5) {
                animSpeed = 0.1f;
            }
            if (totalTime > deathStartTime + 1.5) {
                animSpeed = 1.0f;
            }

        }

        lock(screen);

        clear(screen);

//        drawType = 4;

        float animStart = 4.0f;

        if (lastStep == 0 && totalTime > animStart) {
            enemy->targetAnimNo = 0;
            lastStep = 2;
        }

        if (lastStep == 2 && totalTime > animStart + 1.8) {
            play(sounds[7]);
            lastStep = 3;
        }

        if (lastStep == 3 && totalTime > animStart + 2.0) {
            enemy->targetAnimNo = 1;
            lastStep = 4;
        }


        if (lastStep == 4 && totalTime > animStart + 3.8) {
            play(sounds[8]);
            lastStep = 5;
        }

        if (lastStep == 5 && totalTime > animStart + 4.0) {
            enemy->targetAnimNo = 2;
            lastStep = 6;
        }

        if (lastStep == 6 && totalTime > animStart + 5.8) {
            play(sounds[6]);
            lastStep = 7;
        }

        if (lastStep == 7 && totalTime > animStart + 6.0) {
            enemy->targetAnimNo = 7;
            enemy->isHurt = 2;
            lastStep = 8;
        }

        if (lastStep == 8 && totalTime > animStart + 7.0) {
            enemy->targetAnimNo = 9;
            enemy->isHurt = 2;
            lastStep = 9;
            deathStartTime = totalTime;
        }

        if (lastStep == 9 && totalTime > animStart + 8.5) {
            lastStep = 10;
            play(sounds[0]);
        }

//        if (frames > startVideoFrame + 800) {
//            drawType = 5;
//        } else if (frames > startVideoFrame + 600) {
//            drawType = 4;
//        } else if (frames > startVideoFrame + 400) {
//            drawType = 3;
//        } else if (frames > startVideoFrame + 200) {
//            drawType = 2;
//        } else if (frames > startVideoFrame) {
//            drawType = 1;
//        }

        int modelNextFrame = 0;
        updateAnim(enemy, 0.025 * 7.0 * animSpeed, modelNextFrame);


        static float camMove = 0.0f;
        static float modelRot = 0.0f;

        Matrix projection, camera, scale, rotY;
        projection.basicProjection(256, screen.w, screen.h);
        rotY.setRotationY(5 * 3.14/5 + modelRot);
        scale.setScale(0.3f);

        static int camStep = 0;
        if (lastStep >= 9) {
//            camMove += dt * 0.5;
            if (camStep == 0) {
                modelRot += dt * 4.0f;
            } else {
                camMove += dt * 2.0f;
            }

            if (camMove > 1.0f) camMove = 1.0f;

            if (modelRot > 3.14f * 2) {
                modelRot = 3.14f * 2;
                camStep = 1;
            }
        }

//        float eye[3] = { 0.0f, lastStep >= 9 ? 14.0f * camMove : 0.0f, lastStep >= 9 ? -14 + 10 * camMove : -14.0f };

        float eye[3] = { 0.0f, 0.0f, -14.0f } ;
        float target[3] { 0.0f, 0.0f, 0.0f };
        camera = lookAt(eye, target);

        Matrix composition = projection * camera * scale * rotY;

        transform(enemy, &composition);

        switch (drawType) {
            case 0:
                drawWire(screen.image, enemy);
                break;
            case 1:
                drawFlat(screen.image, enemy);
                break;
            case 2:
                drawFlatShaded(screen.image, enemy);
                break;
            case 3:
                drawShaded(screen.image, enemy);
                break;
            case 4:
                drawTex(screen.image, enemy);
                break;
            case 5:
                drawTexFix(screen.image, enemy);
                break;
        }

//        drawPoints(screen.image, enemy);

        unlock(screen);

        // copy to window
        SDL_BlitSurface(screen.pixels, NULL, screen.canvas, NULL);
        SDL_UpdateWindowSurface(screen.window);

                char filename[256];
        snprintf(filename, sizeof(filename), "out/frame_%06d.png", frames);

        if (IMG_SavePNG(screen.pixels, filename) != 0) {
            printf("Failed to save PNG %s: %s\n", filename, IMG_GetError());
        }

        char str[256];

        sprintf(str, "FPS: %5.2lf, tris %d", (1000.0 * frames / (SDL_GetTicks() - start)), enemy->modelFrame->trisCount);

        if (*str && (frames % 200) == 0) {
            std::cout<<str<<std::endl;
        }

    }

    return 0;
}





//// SHOW FRAMES
//int main() {
//    dumpFileList("assets/pak0.pak");
//
//    int enemyIndex = SOLDIER;
//
//    TEnemyAsset* enemyAsset = loadEnemyAsset(enemyPakDefinition + enemyIndex);
//    TEnemy* enemy = createEnemy(enemyAsset);
//
//    int32_t* points = new int32_t[enemy->modelProjectedFrame->vertCount * 2 * 2 * 3];
//
//    int index = 0;
//    while(enemyPakDefinition[enemyIndex].sounds[index]) {
//        sounds[index] = loadWav("assets/pak0.pak", enemyPakDefinition[enemyIndex].sounds[index]);
//        index++;
//    }
//    sounds[index] = nullptr;
//
//    TScreen screen;
//#ifdef DEBUG
//    int debug = 1;
//#else
//    int debug = 0;
//#endif
//
//    if (debug) {
//        createWindow(screen, 1024, 576, false);
//    } else {
//        createWindow(screen, 1024, 576, false);
////        createWindow(screen, 1920, 1080, true);
//    }
//
//    int quit = 0;
//    unsigned int t1 = SDL_GetTicks();
//    unsigned long frames = 0;
//    unsigned int start = SDL_GetTicks();
//
////    Image* texture = generateTexture(RGB565(255, 0, 0), RGB565(0, 0, 255));
////    Image* texture = loadPCX("assets/pak0.pak", "models/monsters/gunner/skin.pcx");
//
//    int soundIndex = 0;
//    int drawType = 0;
//
//    play(sounds[8]);
//    int lastStep = -1;
//    float totalTime = 0.0f;
//
//    enemy->targetAnimNo = 10;
//
//    while (!quit) {
//        SDL_Event ev;
//        while (SDL_PollEvent(&ev)) {
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
//                quit = true;
//            }
//
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_1) {
//                printf("sound %d\n", soundIndex);
//                play(sounds[soundIndex]);
//                soundIndex++;
//                if (sounds[soundIndex] == nullptr) soundIndex = 0;
//            }
//
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_2) {
//                enemy->isHurt = !enemy->isHurt;
//            }
//
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_4) {
//                drawType = (drawType + 1) % 6;
//            }
//
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_3) {
//                enemy->animationNo++;
//                if (enemy->animationNo >= enemy->asset->animList->animCount) {
//                    enemy->animationNo = 0;
//                }
//                enemy->modelProgress = enemy->asset->animList->anim[enemy->animationNo].startFrame * enemy->speed;
//
//                if (strncmp("death", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
//                    play(sounds[0]);
//                }
//
//                if (strncmp("pain", enemy->asset->animList->anim[enemy->animationNo].name, 4) == 0) {
//                    play(sounds[5 + rand()%2]);
//                }
//
//                if (strncmp("attak1", enemy->asset->animList->anim[enemy->animationNo].name, 6) == 0) {
//                    play(sounds[3]);
//                } else
//                if (strncmp("attak", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
//                    play(sounds[1 + rand()%3]);
//                }
//            }
//
////            handleKeyEvents(ev);
//
//            if (ev.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//
//        unsigned int t2 = SDL_GetTicks();
//        float dt = (float)(t2 - t1) / 1000.0f;
//        t1 = t2;
//        frames++;
//
//        printf("total time %5.3lf\n", totalTime);
//
//        // constant dt for video
////        dt = 0.0055f;
//
//        totalTime += dt;
//
//        static float animSpeed = 0.0005f;
//
//        static float deathStartTime = 0.0f;
//
//        if (lastStep >= 9) {
//            animSpeed = 0.2f;
//            if (totalTime > deathStartTime + 0.3 && totalTime < deathStartTime + 1.5) {
//                animSpeed = 0.1f;
//            }
//            if (totalTime > deathStartTime + 1.5) {
//                animSpeed = 1.0f;
//            }
//
//        }
//
//        lock(screen);
//
//        clear(screen);
//
//        drawType = 0;
//
//        float animStart = 4.0f;
//
//        if (lastStep == 0 && totalTime > animStart) {
//            enemy->targetAnimNo = 0;
//            lastStep = 2;
//        }
//
//        if (lastStep == 2 && totalTime > animStart + 1.8) {
//            play(sounds[7]);
//            lastStep = 3;
//        }
//
//        if (lastStep == 3 && totalTime > animStart + 2.0) {
//            enemy->targetAnimNo = 1;
//            lastStep = 4;
//        }
//
//
//        if (lastStep == 4 && totalTime > animStart + 3.8) {
//            play(sounds[8]);
//            lastStep = 5;
//        }
//
//        if (lastStep == 5 && totalTime > animStart + 4.0) {
//            enemy->targetAnimNo = 2;
//            lastStep = 6;
//        }
//
//        if (lastStep == 6 && totalTime > animStart + 5.8) {
//            play(sounds[6]);
//            lastStep = 7;
//        }
//
//        if (lastStep == 7 && totalTime > animStart + 6.0) {
//            enemy->targetAnimNo = 7;
//            enemy->isHurt = 2;
//            lastStep = 8;
//        }
//
//        if (lastStep == 8 && totalTime > animStart + 7.0) {
//            enemy->targetAnimNo = 9;
//            enemy->isHurt = 2;
//            lastStep = 9;
//            deathStartTime = totalTime;
//        }
//
//        if (lastStep == 9 && totalTime > animStart + 8.5) {
//            lastStep = 10;
//            play(sounds[0]);
//        }
//
////        if (frames > startVideoFrame + 800) {
////            drawType = 5;
////        } else if (frames > startVideoFrame + 600) {
////            drawType = 4;
////        } else if (frames > startVideoFrame + 400) {
////            drawType = 3;
////        } else if (frames > startVideoFrame + 200) {
////            drawType = 2;
////        } else if (frames > startVideoFrame) {
////            drawType = 1;
////        }
//
//        int modelNextFrame;
//        updateAnim(enemy, 250 * animSpeed, modelNextFrame);
//
//
//        static float camMove = 0.0f;
//        static float modelRot = 0.0f;
//
//        Matrix projection, camera, scale, rotY;
//        projection.basicProjection(256, screen.w, screen.h);
//        rotY.setRotationY(4 * 3.14/5 + modelRot);
//        scale.setScale(0.3f);
//
//        static int camStep = 0;
//        if (lastStep >= 9) {
////            camMove += dt * 0.5;
//            if (camStep == 0) {
//                modelRot += dt * 4.0f;
//            } else {
//                camMove += dt * 2.0f;
//            }
//
//            if (camMove > 1.0f) camMove = 1.0f;
//
//            if (modelRot > 3.14f * 2) {
//                modelRot = 3.14f * 2;
//                camStep = 1;
//            }
//        }
//
//        float eye[3] = { 0.0f, lastStep >= 9 ? 14.0f * camMove : 0.0f, lastStep >= 9 ? -14 + 10 * camMove : -14.0f };
//        float target[3] { 0.0f, 0.0f, 0.0f };
//        camera = lookAt(eye, target);
//
//        Matrix composition = projection * camera * scale * rotY;
//
//        transform(enemy, &composition);
//
//        switch (drawType) {
//            case 0:
//                drawWire(screen.image, enemy);
//                break;
//            case 1:
//                drawFlat(screen.image, enemy);
//                break;
//            case 2:
//                drawFlatShaded(screen.image, enemy);
//                break;
//            case 3:
//                drawShaded(screen.image, enemy);
//                break;
//            case 4:
//                drawTex(screen.image, enemy);
//                break;
//            case 5:
//                drawTexFix(screen.image, enemy);
//                break;
//        }
//
//        for(int j = 0; j<enemy->modelProjectedFrame->vertCount * 3; j++) {
//            points[j * 2 + 0] =(int32_t) (enemy->modelProjectedFrame->vertexes[j].x / enemy->modelProjectedFrame->vertexes[j].w);
//            points[j * 2 + 1] =(int32_t) (enemy->modelProjectedFrame->vertexes[j].y / enemy->modelProjectedFrame->vertexes[j].w);
//        }
//
//        getTModel(
//                *enemy->asset->model,
//                *enemy->modelFrame,
//                modelNextFrame,
//                modelNextFrame,
//                1.0
//        );
//        transform(enemy, &composition);
//
////        int base = enemy->modelProjectedFrame->vertCount * 2;
////        for(int j = 0; j<enemy->modelProjectedFrame->vertCount; j++) {
////            points[base + j * 2 + 0] =(int32_t) (enemy->modelProjectedFrame->vertexes[j].x / enemy->modelProjectedFrame->vertexes[j].w);
////            points[base + j * 2 + 1] =(int32_t) (enemy->modelProjectedFrame->vertexes[j].y / enemy->modelProjectedFrame->vertexes[j].w);
////        }
////
////        drawInterpolations(screen.image, points, enemy->modelProjectedFrame->vertCount);
//
////        drawPoints(screen.image, enemy);
//
//
//        unlock(screen);
//
//        // copy to window
//        SDL_BlitSurface(screen.pixels, NULL, screen.canvas, NULL);
//        SDL_UpdateWindowSurface(screen.window);
//
//                char filename[256];
//        snprintf(filename, sizeof(filename), "out/frame_%06d.png", frames);
//
//        if (IMG_SavePNG(screen.pixels, filename) != 0) {
//            printf("Failed to save PNG %s: %s\n", filename, IMG_GetError());
//        }
//
//        char str[256];
//
//        sprintf(str, "FPS: %5.2lf, tris %d", (1000.0 * frames / (SDL_GetTicks() - start)), enemy->modelFrame->trisCount);
//
//        if (*str && (frames % 200) == 0) {
//            std::cout<<str<<std::endl;
//        }
//
//    }
//
//    return 0;
//}
//



//// TEXTURE ANIM
////
////
//int main() {
//    dumpFileList("assets/pak0.pak");
//    TScreen screen;
//#ifdef DEBUG
//    int debug = 1;
//#else
//    int debug = 0;
//#endif
//
//    if (debug) {
//        createWindow(screen, 1024, 576, false);
//    } else {
//        createWindow(screen, 1024, 576, false);
////        createWindow(screen, 1920, 1080, true);
//    }
//
//    bool quit = false;
//    unsigned long frames = 0;
//    uint32_t start = SDL_GetTicks();
//    uint32_t lastFrame = SDL_GetTicks() + 2000;
//
//    Image* texture = loadPCX("assets/pak0.pak", "models/items/ammo/cells/medium/skin.pcx");
//
//    while (!quit) {
//        SDL_Event ev;
//        while (SDL_PollEvent(&ev)) {
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
//                quit = true;
//            }
//
//            if (ev.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//
//        lock(screen);
//        clear(screen);
//
//        FlatTrianglePoint points[3] = {
//                {300, 50},
//                { 150, 250},
//                { 500, 400 }
//        };
//
//        static float percentage = 200.0f;
//
//        float dt = 0.0f;
//        uint32_t currentTime = SDL_GetTicks();
//        if (currentTime > lastFrame) {
//            dt += (float)(currentTime - lastFrame) / 1000.0f;
//            lastFrame = currentTime;
//        }
//        percentage+= 0.1f * dt;
//
//        screen.image->drawFlatTriangleSlow(
//                points,
//                sizeof(FlatTrianglePoint),
//                RGB565(200, 0, 0),
//                percentage,
//                10
//                );
//
//        screen.image->drawWireframeTriangle(
//                (WireframePoint*)points,
//                sizeof(FlatTrianglePoint),
//                RGB565(200, 255, 255)
//        );
//
//        GouraudTrianglePoint gouraudPoints[3] = {
//                {300, 50,  RGB565(255, 0, 255)},
//                {150, 250, RGB565(0, 255, 255)},
//                {500, 400, RGB565(255, 255, 0) }
//        };
//
//        screen.image->drawGouraudTriangleSlow(
//                gouraudPoints,
//                sizeof(GouraudTrianglePoint),
//                percentage
//                );
//
//
////        TexTrianglePoint textPoints[3] = {
////                {300, 50, 0.45f, 0.25f },
////                { 150, 250,  0.28f, 0.52f },
////                { 500, 400,  0.28f, 1.0f  }
////        };
////
////        float texOut[4];
////        screen.image->drawTexTriangleSlow(
////                texture,
////                textPoints,
////                sizeof(TexTrianglePoint),
////                percentage,
////                texOut
////        );
////
////        screen.image->drawWireframeTriangle(
////                (WireframePoint*)points,
////                sizeof(FlatTrianglePoint),
////                RGB565(200, 255, 255)
////        );
////
////        int texPosY = 100;
////        int texPosX = 600;
////        int scale = 3;
////
////        for(int y = 0; y<texture->height * scale; y++) {
////            for(int x = 0; x<texture->width * scale; x++) {
////                ((uint16_t*)screen.pixels->pixels)[(texPosY + y) * screen.w + texPosX + x] = texture->buffer[(y/scale) * texture->width + (x/scale)];
////            }
////        }
////
////        WireframePoint textureSrcTriangle[3] = {
////                { texPosX + (int)(texture->width * scale * textPoints[0].u) , texPosY + (int)(texture->height * scale * textPoints[0].v)},
////                { texPosX + (int)(texture->width * scale * textPoints[1].u) , texPosY + (int)(texture->height * scale * textPoints[1].v)},
////                { texPosX + (int)(texture->width * scale * textPoints[2].u) , texPosY + (int)(texture->height * scale * textPoints[2].v)},
////        };
////
////
////        screen.image->line(
////                2 + scale + texPosX + (int)(texture->width * scale * texOut[0]),
////                2 + scale + texPosY + (int)(texture->height * scale * texOut[1]),
////                scale + texPosX + (int)(texture->width * scale * texOut[2]),
////                 scale + texPosY + (int)(texture->height * scale * texOut[3]),
////                RGB565(255, 255, 255)
////                );
////
////        screen.image->drawWireframeTriangle(
////                textureSrcTriangle,
////                sizeof(WireframePoint),
////                RGB565(255, 0, 0)
////        );
////
////        screen.image->putPixel(
////                2 + scale + texPosX + (int)(texture->width * scale * texOut[2]),
////                2 + scale + texPosY + (int)(texture->height * scale * texOut[3]),
////                RGB565(0, 255, 0),
////                10
////                );
////
////        uint16_t secondColor = RGB565(0, 255, 0);
////        if (percentage > 0.55f) {
////            secondColor = RGB565(255, 255, 0);
////        }
////
////        screen.image->putPixel(
////                2 + scale + texPosX + (int)(texture->width * scale * texOut[0]),
////                2 + scale + texPosY + (int)(texture->height * scale * texOut[1]),
////                secondColor,
////                10
////        );
//
//
//
//        unlock(screen);
//
//        SDL_BlitSurface(screen.pixels, NULL, screen.canvas, NULL);
//        SDL_UpdateWindowSurface(screen.window);
//
//        char filename[256];
//        snprintf(filename, sizeof(filename), "out/frame_%06d.png", frames);
//
//        if (IMG_SavePNG(screen.pixels, filename) != 0) {
//            printf("Failed to save PNG %s: %s\n", filename, IMG_GetError());
//        }
//
//        frames++;
//
//        char str[256];
//
//        sprintf(str, "FPS: %5.2lf", (1000.0 * frames / (SDL_GetTicks() - start)));
//
//        if (*str && (frames % 200) == 0) {
//            std::cout << str << std::endl;
//        }
//
//    }
//
//    return 0;
//
//}


// ZCORRECT



//int main() {
//    dumpFileList("assets/pak0.pak");
//    TScreen screen;
//#ifdef DEBUG
//    int debug = 1;
//#else
//    int debug = 0;
//#endif
//
//    if (debug) {
//        createWindow(screen, 1024, 576, false);
//    } else {
//        createWindow(screen, 1024, 576, false);
////        createWindow(screen, 1920, 1080, true);
//    }
//
//    bool quit = false;
//    unsigned long frames = 0;
//    uint32_t start = SDL_GetTicks();
//    uint32_t lastFrame = SDL_GetTicks() + 2000;
//
////    Image* texture = loadPCX("assets/pak0.pak", "models/items/ammo/cells/medium/skin.pcx");
//
//    Image* texture = generateTexture(RGB565(255, 0, 255), RGB565(255, 255, 0));
//
//    while (!quit) {
//        SDL_Event ev;
//        while (SDL_PollEvent(&ev)) {
//            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
//                quit = true;
//            }
//
//            if (ev.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//
//        lock(screen);
//        clear(screen);
//
//        FlatTrianglePoint points[3] = {
//                {300, 50},
//                { 150, 250},
//                { 500, 400 }
//        };
//
//        static float percentage = 0.0f;
//
//        float dt = 0.0f;
//        uint32_t currentTime = SDL_GetTicks();
//        if (currentTime > lastFrame) {
//            dt += (float)(currentTime - lastFrame) / 1000.0f;
//            lastFrame = currentTime;
//        }
//
//        TexTriangleFixPoint textPoints1[3] = {
//                {200, 100, 2.0f, 0.0f, 0.0f },
//                { 200, 500,  2.0f,0.0f, 1.0f },
//                { 400, 350,  5.0f, 1.0f, 1.0f  }
//        };
//
//        TexTriangleFixPoint textPoints2[3] = {
//                { 200, 100, 2.0f,0.0f, 0.0f },
//                { 400, 350,  5.0f, 1.0f, 1.0f  },
//                { 400, 200,  5.0f, 1.0f, 0.0f }
//        };
//
//        screen.image->drawTexTriangleFix(
//                texture,
//                textPoints1,
//                sizeof(TexTriangleFixPoint)
//        );
//
//        screen.image->drawTexTriangleFix(
//                texture,
//                textPoints2,
//                sizeof(TexTriangleFixPoint)
//        );
//
////        screen.image->drawWireframeTriangle(
////                (WireframePoint*)points,
////                sizeof(FlatTrianglePoint),
////                RGB565(200, 255, 255)
////        );
//
//
//
//
//        unlock(screen);
//
//        SDL_BlitSurface(screen.pixels, NULL, screen.canvas, NULL);
//        SDL_UpdateWindowSurface(screen.window);
//
//        char filename[256];
//        snprintf(filename, sizeof(filename), "out/frame_%06d.png", frames);
//
//        if (IMG_SavePNG(screen.pixels, filename) != 0) {
//            printf("Failed to save PNG %s: %s\n", filename, IMG_GetError());
//        }
//
//        frames++;
//
//        char str[256];
//
//        sprintf(str, "FPS: %5.2lf", (1000.0 * frames / (SDL_GetTicks() - start)));
//
//        if (*str && (frames % 200) == 0) {
//            std::cout << str << std::endl;
//        }
//
//    }
//
//    return 0;
//
//}