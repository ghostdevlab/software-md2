//
// Created by Duch on 10.04.2026.
//

#include <valarray>
#include "Image.h"
#include <cstring>
#include <iostream>

Image::Image(int w, int h) : width(w), height(h), buffer(new uint16_t[w * h]), bufferOwner(1) {}

Image::Image(int w, int h, uint16_t *ptr) : width(w), height(h), buffer(ptr), bufferOwner(0) {}

Image::Image(const Image &t) : width(t.width), height(t.height), buffer(t.buffer), bufferOwner(t.bufferOwner){}

Image::Image() : width(-1), height(-1), buffer(nullptr), bufferOwner(0) {}

void Image::clear() const {
    memset((void*)buffer, 0, width * height * 2);
}

void Image::fill(uint16_t color) const {
    uint32_t packed = (((uint32_t)color) << 16) + color;
    for (uint32_t i = 0; i < width * height/2; i++) {
        ((uint32_t *) buffer)[i] = packed;
    }
}

void Image::putPixel(int x, int y, uint16_t color, int size) const {
    for (int yy = y - size / 2; yy < y + (size / 2) + (size & 1); yy++) {
        for (int xx = x - size / 2; xx < x + size / 2 + (size & 1); xx++) {

            if (xx < 0 || yy < 0) continue;
            if (xx >= width || yy >= height) continue;

            ((uint16_t *) buffer)[yy * width + xx] = color;
        }
    }
}

Image::~Image() {
    if (bufferOwner) {
        delete[] buffer;
    }
}



void Image::line(int x1, int y1, int x2, int y2, uint16_t color) const {
    if (x1 == x2 && y1 == y2) return;

    if (x1 <= 0 && x2 <= 0) return;
    if (y1 <= 0 && y2 <= 0) return;
    if (x2 >= width && x1 >= width) return;
    if (y2 >= height && y1 >= height) return;

//    std::cout<<"L "<<x1<<", "<<y1<<", "<<x2<<", "<<y2<<std::endl;
    if (abs(x1 - x2) > abs(y1 - y2)) {
        if (x2 < x1) {
            int tmp = x2;
            x2 = x1;
            x1 = tmp;

            tmp = y2;
            y2 = y1;
            y1 = tmp;
        }
        float dy = (float)(y2 - y1) / (x2 - x1 + 1);

        for (int x = x1; x<=x2; x++) {
            int y = (int)(dy * (x - x1) + y1);

            if (y < 0 || y >= (int)height || x < 0 || x >= (int)width) continue;
            putPixel(x, y, color, 2);
//            ((uint32_t*)buffer)[y * width + x] = color;
        }
    } else {
        if (y2 < y1) {
            int tmp = x2;
            x2 = x1;
            x1 = tmp;

            tmp = y2;
            y2 = y1;
            y1 = tmp;
        }
        float dx = (float)(x2 - x1) / (y2 - y1 + 1);

        for (int y = y1; y<=y2; y++) {
            int x = (int)(dx * (y - y1) + x1);

            if (y < 0 || y >= (int)height || x < 0 || x >= (int)width) continue;
            putPixel(x, y, color, 2);
//            ((uint32_t*)buffer)[y * width + x] = color;
        }
    }
}

void Image::dim() const {
    uint16_t* pointer = ((uint16_t *) buffer);
    for(uint32_t i = 0; i<width * height; i++) {
        uint16_t color = pointer[i];
        int32_t r = (color>>11) & 0x1f;
        int32_t g = (color>>5) & 0x3F;
        int32_t b = (color) & 0x1f;

        r -= 5 * 3;
        g -= 5 * 3;
        b -= 5 * 3;

        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;

        pointer[i] =  (r << 11) + (g << 5) + b;
    }
}

void Image::copyTo(Image *target) {
    memcpy((void*)target->buffer, buffer, width * height * 2);
}

inline void *shiftBytes(void* ptr, uint32_t skipBytes) {
    return ((uint8_t*)ptr) + skipBytes;
}

template<typename T>
inline  void swap(T& a, T &b) {
    auto tmp = a;
    a = b;
    b = tmp;
}

void Image::hLineFlat(int x1, int y, int x2, uint16_t color) {
    if (x2 < 0) return;
    if (x1 >= width) return;

    int w = (x2 - x1 + 1);

    if (x1 < 0) {
        x1 = 0;
    }

    if (x2>= width) {
        x2 = width - 1;
    }

    uint16_t* buf = (uint16_t*)buffer + y * width + x1;

    for (int x = x1; x<=x2; x++, buf++){
        (*buf) = color;
    }

}

void Image::drawFlatTriangle(FlatTrianglePoint* points, uint32_t pointSize, uint16_t color) {
    FlatTrianglePoint *p0 = points;
    FlatTrianglePoint *p1 = (FlatTrianglePoint *) shiftBytes(points, pointSize);
    FlatTrianglePoint *p2 = (FlatTrianglePoint *) shiftBytes(points, 2 * pointSize);

    if (p0->y > p1->y) {
        swap(p0, p1);
    }

    if (p1->y > p2->y) {
        swap(p1, p2);
    }

    if (p0->y > p1->y) {
        swap(p0, p1);
    }

    if (p2->y <= 0) return;
    if (p0->y >= height) return;

    if (p0->y <= p1->y && p1->y > 0) {
        auto lx = (float) p0->x;
        auto rx = (float) p0->x;

        auto dx1 = (float) (p1->x - p0->x) / (p1->y - p0->y + 1);
        auto dx2 = (float) (p2->x - p0->x) / (p2->y - p0->y + 1);

        if (dx1 > dx2) {
            swap(dx1, dx2);
        }

        int sy = p0->y;
        int ey = p1->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;
            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for (int i = 0; i < ey - sy + 1; i++) {
            hLineFlat((int) (lx), sy + i, (int) (rx), color);
            lx += dx1;
            rx += dx2;
        }
    }


    if (p1->y <= p2->y && p2->y > 0) {
        auto dx1 = (float)(p2->x - p1->x) / (p2->y - p1->y + 1);  // 2 -> 3
        auto dx2 = (float)(p2->x - p0->x) / (p2->y - p0->y + 1);  // 1 -> 3

        auto rx = (float)(p0->x) + (p1->y - p0->y + 1) * dx2;
        auto lx = (float)(p1->x);

        if (lx > rx) {
            swap(lx, rx);
        }

        int sy = p1->y;
        int ey = p2->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;
            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for(int i=0; i<ey - sy + 1; i++) {
            hLineFlat((int)(lx), sy + i, (int)(rx), color);
            lx += dx1;
            rx += dx2;
        }

    }
}

void Image::drawGouraudTriangle(GouraudTrianglePoint *points, uint32_t pointSize) {
    GouraudTrianglePoint *p0 = points;
    GouraudTrianglePoint *p1 = (GouraudTrianglePoint *) shiftBytes(points, pointSize);
    GouraudTrianglePoint *p2 = (GouraudTrianglePoint *) shiftBytes(points, 2 * pointSize);

    if (p0->y > p1->y) {
        swap(p0, p1);
    }

    if (p1->y > p2->y) {
        swap(p1, p2);
    }

    if (p0->y > p1->y) {
        swap(p0, p1);
    }

    if (p2->y <= 0) return;
    if (p0->y >= height) return;

    if (p0->y <= p1->y && p1->y > 0) {
        auto lx = (float) p0->x;
        auto rx = (float) p0->x;

        int_fast32_t lColor[3] = {
                (((p0->color) >> 11) & 31) << 8,
                (((p0->color) >> 5) & 63) << 8,
                ((p0->color) & 31) << 8
        };

        int_fast32_t rColor[3] = {
                (((p0->color) >> 11) & 31) << 8,
                (((p0->color) >> 5) & 63) << 8,
                ((p0->color) & 31) << 8
        };

        int_fast32_t dC1[3] = {
                (((((p1->color) >> 11) & 31) - (((p0->color) >> 11) & 31)) << 8) / (p1->y - p0->y + 1),
                (((((p1->color) >> 5) & 63) - (((p0->color) >> 11) & 31)) << 8) / (p1->y - p0->y + 1),
                ((((p1->color) & 31) - (((p0->color) >> 11) & 31)) << 8) / (p1->y - p0->y + 1),
        };

        int_fast32_t dC2[3] = {
                (((((p2->color) >> 11) & 31) - (((p0->color) >> 11) & 31)) << 8) / (p2->y - p0->y + 1),
                (((((p2->color) >> 5) & 63) - (((p0->color) >> 11) & 31)) << 8) / (p2->y - p0->y + 1),
                ((((p2->color) & 31) - (((p0->color) >> 11) & 31)) << 8) / (p1->y - p2->y + 1),
        };

        auto dx1 = (float) (p1->x - p0->x) / (p1->y - p0->y + 1);
        auto dx2 = (float) (p2->x - p0->x) / (p2->y - p0->y + 1);

        int_fast32_t* ptr_dC1 = dC1;
        int_fast32_t* ptr_dC2 = dC2;

        if (dx1 > dx2) {
            swap(dx1, dx2);
            swap(ptr_dC1, ptr_dC2);
        }

        int sy = p0->y;
        int ey = p1->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;
            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for (int i = 0; i < ey - sy + 1; i++) {
//            hLineFlat((int) (lx), sy + i, (int) (rx), color);
            lx += dx1;
            rx += dx2;
        }
    }


    if (p1->y <= p2->y && p2->y > 0) {
        auto dx1 = (float)(p2->x - p1->x) / (p2->y - p1->y + 1);  // 2 -> 3
        auto dx2 = (float)(p2->x - p0->x) / (p2->y - p0->y + 1);  // 1 -> 3

        auto rx = (float)(p0->x) + (p1->y - p0->y + 1) * dx2;
        auto lx = (float)(p1->x);

        if (lx > rx) {
            swap(lx, rx);
        }

        int sy = p1->y;
        int ey = p2->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;
            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for(int i=0; i<ey - sy + 1; i++) {
//            hLineFlat((int)(lx), sy + i, (int)(rx), color);
            lx += dx1;
            rx += dx2;
        }

    }
}