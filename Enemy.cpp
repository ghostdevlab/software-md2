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

    enemy->modelFrame = allocateFrame(enemyAsset->model);
    enemy->modelProjectedFrame = allocateFrame(enemyAsset->model);

    enemy->modelProgress = 0;
    enemy->speed = 15;

    return enemy;
}

void updateAnim(TEnemy* enemy, float dt) {

    auto animationDef = enemy->asset->animList->anim[enemy->animationNo];

    enemy->modelProgress += dt;
//  enemy->modelProgress = 200;

    int deathAnim = 0;
    if (strncmp("death", enemy->asset->animList->anim[enemy->animationNo].name, 5) == 0) {
        deathAnim = 1;
    }

    int modelCurrentFrame = (enemy->modelProgress/enemy->speed);

    if (modelCurrentFrame > animationDef.endFrame) {
        enemy->modelProgress = (deathAnim ? animationDef.endFrame : animationDef.startFrame) * enemy->speed;
        modelCurrentFrame = deathAnim ? animationDef.endFrame : animationDef.startFrame;
    }

    int modelNextFrame = (modelCurrentFrame + 1);

    if (modelNextFrame > animationDef.endFrame) {
        modelNextFrame = deathAnim ? animationDef.endFrame : animationDef.startFrame;
    }


    getTModel(*enemy->asset->model, *enemy->modelFrame, modelCurrentFrame, modelNextFrame, (float)((int)enemy->modelProgress % enemy->speed) / enemy->speed);

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

void draw(Image* image, TEnemy* enemy) {
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