//
// Created by Duch on 10.04.2026.
//

#ifndef SOFTWARERENDERBASIC_IMAGE_H
#define SOFTWARERENDERBASIC_IMAGE_H

#include <cstdint>

#pragma pack(push, 1)

typedef struct {
    int32_t x,y;
} FlatTrianglePoint;

typedef struct {
    int32_t x,y;
    uint16_t color;
} GouraudTrianglePoint;

#pragma pack(pop)

class Image {
public:
    Image();
    Image(int w, int h);
    Image(int w, int h, uint16_t *ptr);
    Image(const Image& t);

    void fill(uint16_t color) const;
    void clear() const;
    void putPixel(int x, int y, uint16_t color, int size) const;
    void line(int x1, int y1, int x2, int y2, uint16_t color) const;
    void dim() const;
    void copyTo(Image* target);

    void hLineFlat(int x1, int y, int x2, uint16_t color);

    void drawFlatTriangle(FlatTrianglePoint* points, uint32_t pointSize, uint16_t color);
    void drawGouraudTriangle(GouraudTrianglePoint* points, uint32_t pointSize);

    ~Image();
private:
    const int32_t width;
    const int32_t height;
    const uint16_t *buffer;
    const int bufferOwner;
};

#define RGB565(r, g, b) ((uint16_t)(((r & 0xF8) << 8)  + ((g & 0xFC) << 3) + (b >> 3)))

#endif //SOFTWARERENDERBASIC_IMAGE_H
