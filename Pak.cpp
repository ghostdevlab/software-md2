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

Image* LoadPCX_Image(FILE* f, long offset, long size)
{
    PCXHeader header;

    fseek(f, offset, SEEK_SET);
    fread(&header, sizeof(PCXHeader), 1, f);

    int width  = header.xmax - header.xmin + 1;
    int height = header.ymax - header.ymin + 1;

    if (header.bitsPerPixel != 8 || header.colorPlanes != 1) {
        printf("Obsługiwany tylko 8-bit PCX\n");
        return NULL;
    }

    Image* img = new Image(width, height);

    uint8_t* scanline = (uint8_t*)malloc(header.bytesPerLine);
    uint8_t* ptr = scanline;

    // Dekodowanie linia po linii (ważne przez padding!)
    for (int y = 0; y < height; y++) {

        int x = 0;

        while (x < header.bytesPerLine) {
            uint8_t c = fgetc(f);

            if ((c & 0xC0) == 0xC0) {
                int count = c & 0x3F;
                uint8_t value = fgetc(f);

                for (int i = 0; i < count && x < header.bytesPerLine; i++) {
                    scanline[x++] = value;
                }
            } else {
                scanline[x++] = c;
            }
        }

        // kopiujemy tylko width (bez paddingu)
        for (int i = 0; i < width; i++) {
            // tymczasowo zapisujemy indeks palety w dolnym bajcie
            img->buffer[y * width + i] = scanline[i];
        }
    }

    free(scanline);

    // 📌 wczytanie palety (z końca CHUNKA, nie pliku!)
    fseek(f, offset + size - 769, SEEK_SET);

    uint8_t marker = fgetc(f);
    if (marker != 12) {
        printf("Brak palety\n");
        delete img;
        return NULL;
    }

    uint8_t palette[256][3];
    fread(palette, 3, 256, f);

    // 📌 konwersja indeksów → RGB565
    for (int i = 0; i < width * height; i++) {
        uint8_t idx = (uint8_t)(img->buffer[i] & 0xFF);

        uint8_t r = palette[idx][0];
        uint8_t g = palette[idx][1];
        uint8_t b = palette[idx][2];

        img->buffer[i] = RGB565(r, g, b);
    }

    return img;
}

Image* loadPCX(char *pakFilePath, char* fileName) {
    FILE* f = fopen(pakFilePath, "rb");

    PakHeader header;
    fread(&header, sizeof(PakHeader), 1, f);
    int fileCount = header.size / sizeof(PakFileMeta);

    PakFileMeta *list = new PakFileMeta[fileCount];
    fseek(f, header.offset, SEEK_SET);
    fread(list, sizeof(PakFileMeta), fileCount, f);

    Image * out = nullptr;

    PakFileMeta *modelMeta = nullptr;

    for(int i = 0; i<fileCount && modelMeta == nullptr; i++) {
        if (strcmp(list[i].name, fileName) == 0) {
            modelMeta = list + i;
        }
    }

    if (modelMeta != nullptr) {
        out = LoadPCX_Image(f, modelMeta->offset, modelMeta->size);
    } else {
        printf("NOT FOUND\n");
    }

    delete[] list;

    return out;
}