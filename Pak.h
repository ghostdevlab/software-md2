//
// Created by Duch on 21.04.2026.
//

#ifndef MD2_PAK_H
#define MD2_PAK_H

#pragma pack(push, 1)

#include <cstdint>
#include "Q2Model.h"
#include "Image.h"

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

typedef struct {
    uint8_t *data;
    uint32_t size;
} WavFile;

typedef struct {
    uint8_t manufacturer;
    uint8_t version;
    uint8_t encoding;
    uint8_t bitsPerPixel;
    uint16_t xmin, ymin;
    uint16_t xmax, ymax;
    uint16_t hres, vres;
    uint8_t palette16[48];
    uint8_t reserved;
    uint8_t colorPlanes;
    uint16_t bytesPerLine;
    uint16_t paletteType;
    uint16_t hscreenSize;
    uint16_t vscreenSize;
    uint8_t filler[54];
} PCXHeader;

#pragma pack(pop)

void dumpFileList(char *path);
TQ2Model* loadModel(char *pakFilePath, char* fileName);
WavFile* loadWav(char *pakFilePath, char* fileName);
Image* loadPCX(char *pakFilePath, char* fileName);

#endif //MD2_PAK_H
