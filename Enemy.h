//
// Created by Duch on 21.04.2026.
//

#ifndef MD2_ENEMY_H
#define MD2_ENEMY_H


#include "Q2Model.h"
#include "Q2Anim.h"

typedef struct EnemyPakDefinition {
    char modelName[64];
    char texture[64];
    char painTexture[64];
    char* sounds[15];
} TEnemyPakDefinition;

typedef struct Enemy {
    TQ2Model* model;
    TQ2AnimList* animList;
} TEnemy;

TEnemy* loadEnemy(EnemyPakDefinition* enemy);

#endif //MD2_ENEMY_H
