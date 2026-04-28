//
// Created by Duch on 21.04.2026.
//

#ifndef MD2_ENEMY_H
#define MD2_ENEMY_H


#include "Q2Model.h"
#include "Q2Anim.h"
#include "Image.h"
#include "Matrix.h"

typedef struct EnemyPakDefinition {
    char modelName[64];
    char texture[64];
    char painTexture[64];
    char* sounds[20];
} TEnemyPakDefinition;

typedef struct {
    TQ2Model* model;
    Image* skin;
    Image* pain;
    TQ2AnimList* animList;
} TEnemyAsset;

typedef struct {
    TEnemyAsset* asset;
    int isHurt;

    int animationNo;
    int targetAnimNo;
    float modelProgress;
    int speed;

    TQ2ModelFrame* modelFrame;
    TQ2ModelFrame* modelProjectedFrame;
} TEnemy;

#define GUNNER 0
#define SOLDIER 1
#define TANK 2

extern EnemyPakDefinition enemyPakDefinition[];

TEnemyAsset* loadEnemyAsset(EnemyPakDefinition* enemyDef);
TEnemy *createEnemy(TEnemyAsset* enemyAsset);
void updateAnim(TEnemy* enemy, float dt, int& modelNextFrame);
void transform(TEnemy* enemy, Matrix *matrix);
void drawPoints(Image* image, TEnemy* enemy);
void drawFlat(Image* image, TEnemy* enemy);
void drawFlatShaded(Image* image, TEnemy* enemy);
void drawWire(Image* image, TEnemy* enemy);
void drawTex(Image* image, TEnemy* enemy);
void drawTexFix(Image* image, TEnemy* enemy);
void drawShaded(Image* image, TEnemy* enemy);

void drawInterpolations(Image* image, int32_t* points, int vertCount);

#endif //MD2_ENEMY_H
