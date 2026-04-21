//
// Created by Duch on 21.04.2026.
//

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Pak.h"

void dumpFileList(char *path) {
    FILE* f = fopen(path, "rb");

    PakHeader header;

    fread(&header, sizeof(PakHeader), 1, f);

    int fileCount = header.size / sizeof(PakFileMeta);

    printf("%c%c%c%c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
    printf("File count: %d\n", fileCount);

    PakFileMeta *list = new PakFileMeta[fileCount];

    fseek(f, header.offset, SEEK_SET);
    fread(list, sizeof(PakFileMeta), fileCount, f);

    for(int i = 0; i<fileCount; i++) {
        printf("File %d: %s\n", i, list[i].name);
    }

    delete[] list;
    fclose(f);
}

TQ2Model* loadModel(char *pakFilePath, char* fileName) {
    FILE* f = fopen(pakFilePath, "rb");

    PakHeader header;
    fread(&header, sizeof(PakHeader), 1, f);
    int fileCount = header.size / sizeof(PakFileMeta);

    PakFileMeta *list = new PakFileMeta[fileCount];

    fseek(f, header.offset, SEEK_SET);
    fread(list, sizeof(PakFileMeta), fileCount, f);

    PakFileMeta *modelMeta = nullptr;

    for(int i = 0; i<fileCount && modelMeta == nullptr; i++) {
        if (strcmp(list[i].name, fileName) == 0) {
            modelMeta = list + i;
        }
    }

    Q2Model* out = nullptr;

    if (modelMeta != nullptr) {
        out = openFile(f, modelMeta->offset);
    }

    delete[] list;

    return out;
}

WavFile* loadWav(char *pakFilePath, char* fileName) {
    FILE* f = fopen(pakFilePath, "rb");

    PakHeader header;
    fread(&header, sizeof(PakHeader), 1, f);
    int fileCount = header.size / sizeof(PakFileMeta);

    PakFileMeta *list = new PakFileMeta[fileCount];
    fseek(f, header.offset, SEEK_SET);
    fread(list, sizeof(PakFileMeta), fileCount, f);

    WavFile * out = nullptr;

    PakFileMeta *modelMeta = nullptr;

    for(int i = 0; i<fileCount && modelMeta == nullptr; i++) {
        if (strcmp(list[i].name, fileName) == 0) {
            modelMeta = list + i;
        }
    }

    if (modelMeta != nullptr) {
        out = (WavFile*)malloc(sizeof(WavFile));
        out->size = modelMeta->size;
        out->data = (uint8_t*)malloc(modelMeta->size);
        fseek(f, modelMeta->offset, SEEK_SET);
        fread(out->data, out->size, 1, f);
    } else {
        printf("NOT FOUND\n");
    }

    delete[] list;

    return out;
}