//
// Created by Duch on 21.04.2026.
//

#include <cstdlib>
#include <cstring>
#include "Enemy.h"
#include "Pak.h"


EnemyPakDefinition enemyPakDefinition[] = {
        {
            "models/monsters/gunner/tris.md2",
            "models/monsters/gunner/skin.pcx",
            "models/monsters/gunner/pain.pcx",
            {
                    "sound/gunner/death1.wav",
                    "sound/gunner/Gunatck1.wav",
                    "sound/gunner/Gunatck2.wav",
                    "sound/gunner/Gunatck3.wav",
                    "sound/gunner/Gunidle1.wav",
                    "sound/gunner/gunpain1.wav",
                    "sound/gunner/Gunpain2.wav",
                    "sound/gunner/Gunsrch1.wav",
                    "sound/gunner/sight1.wav",
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
            },
        },
        {
                "models/monsters/soldier/tris.md2",
                "models/monsters/soldier/skin.pcx",
                "models/monsters/soldier/pain.pcx",
                {
                        "sound/soldier/Solatck1.wav",
                        "sound/soldier/Solatck2.wav",
                        "sound/soldier/Solatck3.wav",
                        "sound/soldier/SOLDETH1.wav",
                        "sound/soldier/SOLDETH2.wav",
                        "sound/soldier/SOLDETH3.wav",
                        "sound/soldier/SOLIDLE1.wav",
                        "sound/soldier/SOLPAIN1.wav",
                        "sound/soldier/SOLPAIN2.wav",
                        "sound/soldier/SOLPAIN3.wav",
                        "sound/soldier/SOLSGHT1.wav",
                        "sound/soldier/SOLSRCH1.wav",
                        nullptr,
                        nullptr,
                        nullptr,
                },
        }
};

TEnemyAsset* loadEnemyAsset(EnemyPakDefinition* enemyDef) {
    TEnemyAsset *enemy = (TEnemyAsset*)malloc(sizeof(TEnemyAsset));

    char* assets = "assets/pak0.pak";

    enemy->model = loadModel(assets, enemyDef->modelName);
    enemy->skin = loadPCX(assets, enemyDef->texture);
    enemy->pain = loadPCX(assets, enemyDef->painTexture);

    enemy->animList = buildAnimationList(enemy->model);

    return enemy;
}

TEnemy* createEnemy(TEnemyAsset* enemyAsset) {
    TEnemy *enemy = (TEnemy*) malloc(sizeof(TEnemy));

    enemy->asset = enemyAsset;

    enemy->animationNo = 0;
    enemy->targetAnimNo = 0;

    enemy->modelFrame = allocateFrame(enemyAsset->model);
    enemy->modelProjectedFrame = allocateFrame(enemyAsset->model);

    enemy->modelProgress = 0;
    enemy->speed = 15;

    return enemy;
}

void updateAnim(TEnemy* enemy, float dt) {

    auto animationDef = enemy->asset->animList->anim[enemy->animationNo];

    int prevFrame = enemy->modelProgress/enemy->speed;

    enemy->modelProgress += dt;
//  enemy->modelProgress = 200;

    int deathAnim = 0;
    int walk = 0;
    if (strncmp("death", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
        deathAnim = 1;
    }

    if (strncmp("walk", enemy->asset->animList->anim[enemy->animationNo].name, 4) == 0) {
        walk = 1;
    }

    int walkStart = 5;
    int walkEnd = 5;

    int modelCurrentFrame = (enemy->modelProgress/enemy->speed);

    if (modelCurrentFrame > (walk ? (animationDef.endFrame - walkEnd) : animationDef.endFrame)) {
        int nextFrame = animationDef.startFrame;
        if (deathAnim) {
            nextFrame = animationDef.endFrame;
        }

        if (walk) {
            nextFrame = animationDef.startFrame + walkStart;
        }

        enemy->modelProgress = nextFrame * enemy->speed;
        modelCurrentFrame = nextFrame;
    }

    if (enemy->targetAnimNo != enemy->animationNo) {
        int modelNextFrame = enemy->asset->animList->anim[enemy->targetAnimNo].startFrame;
        float progress = (float)((int)enemy->modelProgress % enemy->speed) / enemy->speed;
        if (prevFrame != modelCurrentFrame) {
            enemy->animationNo = enemy->targetAnimNo;
            enemy->modelProgress = enemy->asset->animList->anim[enemy->animationNo].startFrame * enemy->speed;
        }
        printf("Current frame %d, next frame %d - %d %5.3lf\n", modelCurrentFrame, modelNextFrame, enemy->targetAnimNo, progress);
        getTModel(*enemy->asset->model, *enemy->modelFrame, modelCurrentFrame, modelNextFrame, progress);
    } else {
        int modelNextFrame = (modelCurrentFrame + 1);

        if (modelNextFrame > (walk ? (animationDef.endFrame - walkEnd) : animationDef.endFrame)) {
            modelNextFrame = deathAnim ? animationDef.endFrame : animationDef.startFrame;

            if (walk) {
                modelNextFrame = animationDef.startFrame + walkStart;
            }
        }
        getTModel(*enemy->asset->model, *enemy->modelFrame, modelCurrentFrame, modelNextFrame, (float)((int)enemy->modelProgress % enemy->speed) / enemy->speed);
    }

//    printf("ANIM %s %d %d\n", enemy->asset->animList->anim[enemy->animationNo].name, modelCurrentFrame, modelNextFrame);



}

void sort(float* vertexes, int vertCount, int vertSize);

void transform(TEnemy* enemy, Matrix *matrix) {
    matrix->mul(
            (float*)enemy->modelFrame->vertexes,
            (float*)enemy->modelProjectedFrame->vertexes,
            enemy->modelFrame->vertCount,
            6);

    sort((float*)enemy->modelProjectedFrame->vertexes, enemy->modelFrame->vertCount, 6);
}

void drawShaded(Image* image, TEnemy* enemy) {
    for(int i = 0; i<enemy->modelFrame->trisCount; i++) {
        TQ2ModelFrame* modelProjectedFrame = enemy->modelProjectedFrame;

//        int u0 = (int)(modelProjectedFrame->vertexes[i * 3 + 0].u * 255);
//        int v0 = (int)(modelProjectedFrame->vertexes[i * 3 + 0].v * 255);
//
//        int u1 = (int)(modelProjectedFrame->vertexes[i * 3 + 1].u * 255);
//        int v1 = (int)(modelProjectedFrame->vertexes[i * 3 + 1].v * 255);
//
//        int u2 = (int)(modelProjectedFrame->vertexes[i * 3 + 2].u * 255);
//        int v2 = (int)(modelProjectedFrame->vertexes[i * 3 + 2].v * 255);
//
//        uint16_t col1 = RGB565(u0, v0, 255);
//        uint16_t col2 = RGB565(u1, v1, 255);
//        uint16_t col3 = RGB565(u2, v2, 255);

        auto c1 = (uint16_t )(255 - 10 * (modelProjectedFrame->vertexes[i * 3 + 0].z));
        auto c2 = (uint16_t )(255 - 10 * (modelProjectedFrame->vertexes[i * 3 + 1].z));
        auto c3 = (uint16_t )(255 - 10 * (modelProjectedFrame->vertexes[i * 3 + 2].z));

//        auto avg = (c1 + c2 + c3) / 3;

        uint16_t col1 = RGB565(c1, c1, c1);
        uint16_t col2 = RGB565(c2, c2, c2);
        uint16_t col3 = RGB565(c3, c3, c3);
//

//        col1 = RGB565(255, 0, 0);
//        col2 = RGB565(0, 255, 0);
//        col3 = RGB565(0, 0, 255);

//        uint16_t col1 = RGB565(255, 0, 0);
//        uint16_t col2 = RGB565(0, 255, 0);
//        uint16_t col3 = RGB565(0, 0, 255);

//        printf("z: %d  %d\n", (int)modelProjectedFrame->vertexes[i * 3 + 0].z, c1);

        GouraudTrianglePoint gouraudTrianglePoint[3] = {
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 0].x / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 0].y / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        col1
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 1].x / modelProjectedFrame->vertexes[i * 3 + 1].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 1].y / modelProjectedFrame->vertexes[i * 3 + 1].w),
                        col2
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 2].x / modelProjectedFrame->vertexes[i * 3 + 2].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 2].y / modelProjectedFrame->vertexes[i * 3 + 2].w),
                        col3
                }

        };
        image->drawGouraudTriangle(
                gouraudTrianglePoint,
                sizeof(GouraudTrianglePoint)
        );

//            image->drawWireframeTriangle((WireframePoint*)texTrianglePoint, sizeof(TexTrianglePoint), RGB565(0, 255, 0));
    }
}

void drawTex(Image* image, TEnemy* enemy) {
    for(int i = 0; i<enemy->modelFrame->trisCount; i++) {
        TQ2ModelFrame* modelProjectedFrame = enemy->modelProjectedFrame;
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
        image->drawTexTriangle(
                enemy->isHurt ? enemy->asset->pain : enemy->asset->skin,
                texTrianglePoint,
                sizeof(TexTrianglePoint)
                );

//            image->drawWireframeTriangle((WireframePoint*)texTrianglePoint, sizeof(TexTrianglePoint), RGB565(0, 255, 0));
    }
}

void drawWire(Image* image, TEnemy* enemy) {
    for(int i = 0; i<enemy->modelFrame->trisCount; i++) {
        TQ2ModelFrame* modelProjectedFrame = enemy->modelProjectedFrame;
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
        image->drawWireframeTriangle((WireframePoint*)texTrianglePoint, sizeof(TexTrianglePoint), RGB565(0, 255, 0));
    }
}


void drawFlat(Image* image, TEnemy* enemy) {
    for(int i = 0; i<enemy->modelFrame->trisCount; i++) {
        TQ2ModelFrame* modelProjectedFrame = enemy->modelProjectedFrame;
        FlatTrianglePoint flatTrianglePoint[3] = {
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 0].x / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 0].y / modelProjectedFrame->vertexes[i * 3 + 0].w),
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 1].x / modelProjectedFrame->vertexes[i * 3 + 1].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 1].y / modelProjectedFrame->vertexes[i * 3 + 1].w),
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 2].x / modelProjectedFrame->vertexes[i * 3 + 2].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 2].y / modelProjectedFrame->vertexes[i * 3 + 2].w),
                }

        };
        image->drawFlatTriangle((FlatTrianglePoint *)flatTrianglePoint, sizeof(FlatTrianglePoint), RGB565(255, 255, 255));
    }
}

void drawFlatShaded(Image* image, TEnemy* enemy) {
    for(int i = 0; i<enemy->modelFrame->trisCount; i++) {
        TQ2ModelFrame* modelProjectedFrame = enemy->modelProjectedFrame;
        FlatTrianglePoint flatTrianglePoint[3] = {
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 0].x / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 0].y / modelProjectedFrame->vertexes[i * 3 + 0].w),
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 1].x / modelProjectedFrame->vertexes[i * 3 + 1].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 1].y / modelProjectedFrame->vertexes[i * 3 + 1].w),
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 2].x / modelProjectedFrame->vertexes[i * 3 + 2].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 2].y / modelProjectedFrame->vertexes[i * 3 + 2].w),
                }

        };

        uint16_t avgZ = (modelProjectedFrame->vertexes[i * 3 + 0].z + modelProjectedFrame->vertexes[i * 3 + 1].z +   modelProjectedFrame->vertexes[i * 3 + 2].z)/3;

        uint16_t c = 255 - avgZ * 10;

        image->drawFlatTriangle((FlatTrianglePoint *)flatTrianglePoint, sizeof(FlatTrianglePoint), RGB565(c, c, c));
    }
}

void drawTexFix(Image* image, TEnemy* enemy) {
    for(int i = 0; i<enemy->modelFrame->trisCount; i++) {
        TQ2ModelFrame* modelProjectedFrame = enemy->modelProjectedFrame;
        TexTriangleFixPoint texTrianglePoint[3] = {
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 0].x / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 0].y / modelProjectedFrame->vertexes[i * 3 + 0].w),
                        modelProjectedFrame->vertexes[i * 3 + 0].z,
                        modelProjectedFrame->vertexes[i * 3 + 0].u, modelProjectedFrame->vertexes[i * 3 + 0].v
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 1].x / modelProjectedFrame->vertexes[i * 3 + 1].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 1].y / modelProjectedFrame->vertexes[i * 3 + 1].w),
                        modelProjectedFrame->vertexes[i * 3 + 1].z,
                        modelProjectedFrame->vertexes[i * 3 + 1].u, modelProjectedFrame->vertexes[i * 3 + 1].v
                },
                {
                        (int32_t) (modelProjectedFrame->vertexes[i * 3 + 2].x / modelProjectedFrame->vertexes[i * 3 + 2].w),
                        (int32_t )(modelProjectedFrame->vertexes[i * 3 + 2].y / modelProjectedFrame->vertexes[i * 3 + 2].w),
                        modelProjectedFrame->vertexes[i * 3 + 2].z,
                        modelProjectedFrame->vertexes[i * 3 + 2].u, modelProjectedFrame->vertexes[i * 3 + 2].v
                }

        };
        image->drawTexTriangleFix(
                enemy->isHurt ? enemy->asset->pain : enemy->asset->skin,
                texTrianglePoint,
                sizeof(TexTriangleFixPoint)
        );

//            image->drawWireframeTriangle((WireframePoint*)texTrianglePoint, sizeof(TexTrianglePoint), RGB565(0, 255, 0));
    }
}

static float tri_avg_z(float* v, int triIndex, int vertSize)
{
    int base = triIndex * 3 * vertSize;

    float z0 = v[base + 2];
    float z1 = v[base + vertSize + 2];
    float z2 = v[base + 2 * vertSize + 2];

    return (z0 + z1 + z2) / 3.0f;
}

static void swapTri(float* v, int a, int b, int vertSize)
{
    if (a == b) return;

    int triSize = 3 * vertSize;

    for (int i = 0; i < triSize; i++) {
        float tmp = v[a * triSize + i];
        v[a * triSize + i] = v[b * triSize + i];
        v[b * triSize + i] = tmp;
    }
}

static int partition(float* v, int low, int high, int vertSize)
{
    float pivot = tri_avg_z(v, high, vertSize);
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (tri_avg_z(v, j, vertSize) > pivot) {
            i++;
            swapTri(v, i, j, vertSize);
        }
    }

    swapTri(v, i + 1, high, vertSize);
    return i + 1;
}

static void quicksort(float* v, int low, int high, int vertSize)
{
    if (low < high) {
        int pi = partition(v, low, high, vertSize);
        quicksort(v, low, pi - 1, vertSize);
        quicksort(v, pi + 1, high, vertSize);
    }
}

void sort(float* vertexes, int vertCount, int vertSize)
{
    if (!vertexes || vertCount < 3) return;

    int triCount = vertCount / 3;
    quicksort(vertexes, 0, triCount - 1, vertSize);
}