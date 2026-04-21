//
// Created by Duch on 21.04.2026.
//

#include <cstring>
#include <cstdlib>
#include "Q2Anim.h"

TQ2AnimList* buildAnimationList(TQ2Model* model) {
    char lastName[32] = { 0 };

    int countUnique = 0;
    for(int i = 0; i<model->frameCount; i++) {
        int len = strlen(model->frames[i].name);
        if (strncmp(lastName, model->frames[i].name, len - 2) != 0) {
            printf("extract frame %d %s\n", i, model->frames[i].name);
            memcpy(lastName, model->frames[i].name, len);
            lastName[len] = 0;
            countUnique++;
        }
    }

    TQ2AnimList* out = (TQ2AnimList*)malloc(sizeof(TQ2AnimList));
    out->animCount = countUnique;
    out->anim = (TQ2Anim*)malloc(sizeof(TQ2Anim) * countUnique);

    lastName[0] = 0;
    int index = 0;
    int previousStart = -1;
    for(int i = 0; i<model->frameCount; i++) {
        int len = strlen(model->frames[i].name);
        if (strncmp(lastName, model->frames[i].name, len - 2) != 0) {
            if (previousStart != -1) {
                memcpy(out->anim[index].name, lastName, strlen(lastName) + 1);
                out->anim[index].startFrame = previousStart;
                out->anim[index].endFrame = i - 1;
                index++;
            }
            previousStart = i;
            memcpy(lastName, model->frames[i].name, len);
            lastName[len] = 0;
        }
    }

    if (previousStart != -1) {
        memcpy(out->anim[index].name, lastName, strlen(lastName) + 1);
        out->anim[index].startFrame = previousStart;
        out->anim[index].endFrame = model->frameCount - 1;
        index++;
    }


    printf("Unique frame count : %d parsed %d\n", countUnique, index);

    return out;
}