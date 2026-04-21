#include <iostream>
#include <SDL.h>
#include <map>
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

char *gunnerSounds[10] = {
        "sound/gunner/death1.wav",
        "sound/gunner/Gunatck1.wav",
        "sound/gunner/Gunatck2.wav",
        "sound/gunner/Gunatck3.wav",
        "sound/gunner/Gunidle1.wav",
        "sound/gunner/gunpain1.wav",
        "sound/gunner/Gunpain2.wav",
        "sound/gunner/Gunsrch1.wav",
        "sound/gunner/sight1.wav",
        nullptr
};

WavFile* sounds[10];


int main() {
    dumpFileList("assets/pak0.pak");

    TEnemyAsset* enemyAsset = loadEnemyAsset(enemyPakDefinition + GUNNER);
    TEnemy* enemy = createEnemy(enemyAsset);

    int index = 0;
    while(gunnerSounds[index]) {
        sounds[index] = loadWav("assets/pak0.pak", gunnerSounds[index]);
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
//        createWindow(screen, 1024, 576, true);
        createWindow(screen, 1920, 1080, true);
    }

    int quit = 0;
    unsigned int t1 = SDL_GetTicks();
    unsigned long frames = 0;
    unsigned int start = SDL_GetTicks();

//    Image* texture = generateTexture(RGB565(255, 0, 0), RGB565(0, 0, 255));
//    Image* texture = loadPCX("assets/pak0.pak", "models/monsters/gunner/skin.pcx");

    int soundIndex = 0;

    play(sounds[0]);

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

            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_3) {
                enemy->animationNo++;
                if (enemy->animationNo >= enemy->asset->animList->animCount) {
                    enemy->animationNo = 0;
                }
                enemy->modelProgress = enemy->asset->animList->anim[enemy->animationNo].startFrame * enemy->speed;

                if (strncmp("death", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
                    play(sounds[0]);
                }

                if (strncmp("pain", enemy->asset->animList->anim[enemy->animationNo].name, 4) == 0) {
                    play(sounds[5 + rand()%2]);
                }

                if (strncmp("attak", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
                    play(sounds[1 + rand()%3]);
                }
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

        updateAnim(enemy, dt * 200);

        Matrix projection, shift, scale, rotY;
        projection.basicProjection(256, screen.w, screen.h);
        scale.setScale(0.3f);
        rotY.setRotationY(4 * 3.14/5);
        shift.setTransform(0, 0, 13);

        Matrix composition = projection * shift * scale * rotY;

        transform(enemy, &composition);

        draw(screen.image, enemy);

        unlock(screen);

        // copy to window
        SDL_BlitSurface(screen.pixels, NULL, screen.canvas, NULL);
        SDL_UpdateWindowSurface(screen.window);


        char str[256];

        sprintf(str, "FPS: %5.2lf, tris %d", (1000.0 * frames / (SDL_GetTicks() - start)), enemy->modelFrame->trisCount);

        if (*str && (frames % 200) == 0) {
//            std::cout<<str<<std::endl;
        }

    }

    return 0;
}
