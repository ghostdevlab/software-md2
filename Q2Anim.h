//
// Created by Duch on 21.04.2026.
//

#ifndef MD2_Q2ANIM_H
#define MD2_Q2ANIM_H

#include <cstdint>
#include "Q2Model.h"

typedef struct {
    char name[32];
    int startFrame;
    int endFrame;
} TQ2Anim;

typedef struct {
    TQ2Anim *anim;
    uint32_t animCount;
} TQ2AnimList;

TQ2AnimList* buildAnimationList(TQ2Model* model);

#endif //MD2_Q2ANIM_H
