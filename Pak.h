//
// Created by Duch on 21.04.2026.
//

#ifndef MD2_PAK_H
#define MD2_PAK_H

#pragma pack(push, 1)

#include <cstdint>
#include "Q2Model.h"

typedef struct {
    char magic[4];
    uint32_t offset;
    uint32_t size;
} PakHeader;

typedef struct {
    char name[56];
    uint32_t offset;
    uint32_t size;
} PakFileMeta;

#pragma pack(pop)

void dumpFileList(char *path);

TQ2Model* loadModel(char *pakFilePath, char* fileName);

#endif //MD2_PAK_H
