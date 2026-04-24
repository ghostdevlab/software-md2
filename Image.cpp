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

void Image::hLineGouraud(int x1, int y, int x2, int_fast32_t *lColor, int_fast32_t *rColor) {
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

    int_fast32_t dColor[3] = {
            (rColor[0] - lColor[0]) / w,
            (rColor[1] - lColor[1]) / w,
            (rColor[2] - lColor[2]) / w,
    };

    int_fast32_t c[3] = {
            lColor[0],
            lColor[1],
            lColor[2]
    };

    for (int x = x1; x<=x2; x++, buf++){
        uint32_t r = (c[0] >> 16);
        uint32_t g = (c[1] >> 16);
        uint32_t b = (c[2] >> 16);

//        printf("RGB : %d %d %d\n", c[0], c[1], c[2]);

        uint16_t  color = (r << 11) | (g << 5) | (b);

        (*buf) = color;

        c[0] += dColor[0];
        c[1] += dColor[1];
        c[2] += dColor[2];
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
            swap(dx1, dx2);
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
                (((p0->color) >> 11) & 31) << 16,
                (((p0->color) >> 5) & 63) << 16,
                ((p0->color) & 31) << 16
        };

        int_fast32_t rColor[3] = {
                (((p0->color) >> 11) & 31) << 16,
                (((p0->color) >> 5) & 63) << 16,
                ((p0->color) & 31) << 16
        };

        int_fast32_t dC1[3] = {
                (((((p1->color) >> 11) & 31) - (((p0->color) >> 11) & 31)) << 16) / (p1->y - p0->y + 1),
                (((((p1->color) >> 5) & 63) - (((p0->color) >> 5) & 63)) << 16) / (p1->y - p0->y + 1),
                ((((p1->color) & 31) - (((p0->color)) & 31)) << 16) / (p1->y - p0->y + 1),
        };

        int_fast32_t dC2[3] = {
                (((((p2->color) >> 11) & 31) - (((p0->color) >> 11) & 31)) << 16) / (p2->y - p0->y + 1),
                (((((p2->color) >> 5) & 63) - (((p0->color) >> 5) & 63)) << 16) / (p2->y - p0->y + 1),
                ((((p2->color) & 31) - (((p0->color)) & 31)) << 16) / (p2->y - p0->y + 1),
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

            lColor[0] -= ptr_dC1[0] * sy;
            lColor[1] -= ptr_dC1[1] * sy;
            lColor[2] -= ptr_dC1[2] * sy;

            rColor[0] -= ptr_dC2[0] * sy;
            rColor[1] -= ptr_dC2[1] * sy;
            rColor[2] -= ptr_dC2[2] * sy;

            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for (int i = 0; i < ey - sy + 1; i++) {
            hLineGouraud((int) (lx), sy + i, (int) (rx), lColor, rColor);
            lx += dx1;
            rx += dx2;

            lColor[0] += ptr_dC1[0];
            lColor[1] += ptr_dC1[1];
            lColor[2] += ptr_dC1[2];

            rColor[0] += ptr_dC2[0];
            rColor[1] += ptr_dC2[1];
            rColor[2] += ptr_dC2[2];
        }
    }


    if (p1->y <= p2->y && p2->y > 0) {
        auto dx1 = (float)(p2->x - p1->x) / (p2->y - p1->y + 1);  // 2 -> 3
        auto dx2 = (float)(p2->x - p0->x) / (p2->y - p0->y + 1);  // 1 -> 3

        int_fast32_t dC1[3] = {
                (((((p2->color) >> 11) & 31) - (((p1->color) >> 11) & 31)) << 16) / (p2->y - p1->y + 1),
                (((((p2->color) >> 5) & 63) - (((p1->color) >> 5) & 63)) << 16) / (p2->y - p1->y + 1),
                ((((p2->color) & 31) - (((p1->color)) & 31)) << 16) / (p2->y - p1->y + 1),
        };

        int_fast32_t dC2[3] = {
                (((((p2->color) >> 11) & 31) - (((p0->color) >> 11) & 31)) << 16) / (p2->y - p0->y + 1),
                (((((p2->color) >> 5) & 63) - (((p0->color) >> 5) & 63)) << 16) / (p2->y - p0->y + 1),
                ((((p2->color) & 31) - (((p0->color)) & 31)) << 16) / (p2->y - p0->y + 1),
        };


        auto rx = (float)(p0->x) + (p1->y - p0->y + 1) * dx2;
        auto lx = (float)(p1->x);

        int_fast32_t lColor[3] = {
                (((p1->color) >> 11) & 31) << 16,
                (((p1->color) >> 5) & 63) << 16,
                ((p1->color) & 31) << 16
        };

        int_fast32_t rColor[3] = {
                ((((p0->color) >> 11) & 31) << 16) + (p1->y - p0->y + 1) * dC2[0],
                ((((p0->color) >> 5) & 63) << 16) + (p1->y - p0->y + 1) * dC2[1],
                (((p0->color) & 31) << 16) + (p1->y - p0->y + 1) * dC2[2]
        };

        int_fast32_t* ptr_dC1 = dC1;
        int_fast32_t* ptr_dC2 = dC2;

        if (lx > rx) {
            swap(lx, rx);
            swap(dx1, dx2);
            swap(ptr_dC1, ptr_dC2);
            swap(lColor[0], rColor[0]);
            swap(lColor[1], rColor[1]);
            swap(lColor[2], rColor[2]);
        }

        int sy = p1->y;
        int ey = p2->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;


            lColor[0] -= ptr_dC1[0] * sy;
            lColor[1] -= ptr_dC1[1] * sy;
            lColor[2] -= ptr_dC1[2] * sy;

            rColor[0] -= ptr_dC2[0] * sy;
            rColor[1] -= ptr_dC2[1] * sy;
            rColor[2] -= ptr_dC2[2] * sy;


            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for(int i=0; i<ey - sy + 1; i++) {
            hLineGouraud((int) (lx), sy + i, (int) (rx), lColor, rColor);
            lx += dx1;
            rx += dx2;


            lColor[0] += ptr_dC1[0];
            lColor[1] += ptr_dC1[1];
            lColor[2] += ptr_dC1[2];

            rColor[0] += ptr_dC2[0];
            rColor[1] += ptr_dC2[1];
            rColor[2] += ptr_dC2[2];
        }

    }
}

inline int_fast32_t toFixedFloat(float v) {
    return ((int_fast32_t)(v * 16777216));
}

inline int_fast32_t toFixedF(int v) {
    return ((int_fast32_t)v) << 10;
}

inline int unfixF(int_fast32_t v) {
    return (int)(v >> 10);
}

void Image::hLineTex(Image* texture, int x1, int y, int x2, int_fast32_t* cord1, int_fast32_t* cord2) {
//    auto buf = (unsigned short *)(i.buf + y * image.pitch);
    uint16_t* buf = (uint16_t*)buffer + y * width;
//
//    std::cout <<" line ("<<x1<<" "<<y<<" " <<x2<<") attr "<<
//        unfixF(cord1[0])<<" "<<unfixF(cord1[1])<<" "<<unfixF(cord2[0])<<" "<<unfixF(cord2[1])<<std::endl;

    if (x2 < 0) return;
    if (x1 >= width) return;

//    if (y < 0 || y >= image.h) {
//        SDL_Log("ERROR ACCESS %d", y);
//        return;
//    }

    if (x2 < x1) return;

    int w = (x2 - x1 + 1);

    int_fast32_t dc[2] = {
            (cord2[0] - cord1[0]) / w,
            (cord2[1] - cord1[1]) / w
    };

    int_fast32_t c[2] = {
            (cord1[0]),
            (cord1[1]),
    };

    if (x1 < 0) {
        c[0] -= x1 * dc[0];
        c[1] -= x1 * dc[1];
        x1 = 0;
    }

    if (x2>= width) {
        x2 = width - 1;
    }

    for (int x = x1; x<=x2; x++){
        int u = unfixF(c[0]);
        int v = unfixF(c[1]);


        auto texLine = (texture->buffer + v * texture->width);
//        buf[x] = v << 11;// v * texture->width;
//
        buf[x] = texLine[u];

        c[0] += dc[0];
        c[1] += dc[1];
    }
}

void Image::drawWireframeTriangle(WireframePoint* points, uint32_t pointSize, uint16_t color) {
    WireframePoint *p0 = points;
    WireframePoint *p1 = (WireframePoint *) shiftBytes(points, pointSize);
    WireframePoint *p2 = (WireframePoint *) shiftBytes(points, 2 * pointSize);

    line(p0->x, p0->y, p1->x, p1->y, color);
    line(p1->x, p1->y, p2->x, p2->y, color);
    line(p2->x, p2->y, p0->x, p0->y, color);
}

void Image::drawTexTriangle(Image* texture, TexTrianglePoint *points, uint32_t pointSize) {
    TexTrianglePoint *p0 = points;
    TexTrianglePoint *p1 = (TexTrianglePoint *) shiftBytes(points, pointSize);
    TexTrianglePoint *p2 = (TexTrianglePoint *) shiftBytes(points, 2 * pointSize);

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

    int_fast32_t unwrap0[2] = { (int_fast32_t)(p0->u * (texture->width - 1)), (int_fast32_t)(p0->v * (texture->height - 1)) };
    int_fast32_t unwrap1[2] = { (int_fast32_t)(p1->u * (texture->width - 1)), (int_fast32_t)(p1->v * (texture->height - 1)) };
    int_fast32_t unwrap2[2] = { (int_fast32_t)(p2->u * (texture->width - 1)), (int_fast32_t)(p2->v * (texture->height - 1)) };


    if (p0->y <= p1->y && p1->y > 0) {
        auto lx = (float) p0->x;
        auto rx = (float) p0->x;

        auto dx1 = (float) (p1->x - p0->x) / (p1->y - p0->y + 1);
        auto dx2 = (float) (p2->x - p0->x) / (p2->y - p0->y + 1);

        int_fast32_t lc[2] = {
                toFixedF(unwrap0[0]),
                toFixedF(unwrap0[1])
        };

        int_fast32_t rc[2] = {
                toFixedF(unwrap0[0]),
                toFixedF(unwrap0[1])
        };

        int_fast32_t dc1[2] = {
                toFixedF(unwrap1[0] - unwrap0[0]) / (p1->y - p0->y + 1),
                toFixedF(unwrap1[1] - unwrap0[1]) / (p1->y - p0->y + 1),
        };

        int_fast32_t dc2[2] = {
                toFixedF(unwrap2[0] - unwrap0[0]) / (p2->y - p0->y + 1),
                toFixedF(unwrap2[1] - unwrap0[1]) / (p2->y - p0->y + 1),
        };

        int_fast32_t *ptr_dc1 = dc1;
        int_fast32_t *ptr_dc2 = dc2;

        if (dx1 > dx2) {
            swap(dx1, dx2);
            swap(ptr_dc1, ptr_dc2);
        }

        int sy = p0->y;
        int ey = p1->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;

            lc[0] -= ptr_dc1[0] * sy;
            lc[1] -= ptr_dc1[1] * sy;
            rc[0] -= ptr_dc2[0] * sy;
            rc[1] -= ptr_dc2[1] * sy;

            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for (int i = 0; i < ey - sy + 1; i++) {
            hLineTex(texture, (int)lx, sy + i, rx, (lc), (rc));
            lx += dx1;
            rx += dx2;

            lc[0] += ptr_dc1[0];
            lc[1] += ptr_dc1[1];
            rc[0] += ptr_dc2[0];
            rc[1] += ptr_dc2[1];
        }
    }


    if (p1->y <= p2->y && p2->y > 0) {
        auto dx1 = (float)(p2->x - p1->x) / (p2->y - p1->y + 1);  // 2 -> 3
        auto dx2 = (float)(p2->x - p0->x) / (p2->y - p0->y + 1);  // 1 -> 3

        int_fast32_t dc1[2] = {
                toFixedF(unwrap2[0] - unwrap1[0]) / (p2->y - p1->y + 1),
                toFixedF(unwrap2[1] - unwrap1[1]) / (p2->y - p1->y + 1)
        };

        int_fast32_t dc2[2] = {
                toFixedF(unwrap2[0] - unwrap0[0]) / (p2->y - p0->y + 1),
                toFixedF(unwrap2[1] - unwrap0[1]) / (p2->y - p0->y + 1)
        };


        auto rx = (float)(p0->x) + (p1->y - p0->y + 1) * dx2;
        auto lx = (float)(p1->x);

        int_fast32_t rc[2] = {
                toFixedF(unwrap0[0]) + (p1->y - p0->y + 1) * dc2[0],
                toFixedF(unwrap0[1]) + (p1->y - p0->y + 1) * dc2[1]
        };

        int_fast32_t lc[2] = {
                toFixedF(unwrap1[0]),
                toFixedF(unwrap1[1])
        };

        int_fast32_t *ptr_dc1 = dc1;
        int_fast32_t *ptr_dc2 = dc2;

        if (lx > rx) {
            swap(lx, rx);
            swap(dx1, dx2);
            swap(lc[0], rc[0]);
            swap(lc[1], rc[1]);
            swap(ptr_dc1, ptr_dc2);
        }

        int sy = p1->y;
        int ey = p2->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;

            lc[0] += ptr_dc1[0] * sy;
            lc[1] += ptr_dc1[1] * sy;
            rc[0] += ptr_dc2[0] * sy;
            rc[1] += ptr_dc2[1] * sy;

            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for(int i=0; i<ey - sy + 1; i++) {
            hLineTex(texture, (int)(lx), sy + i, (int)(rx), (lc), (rc));
            lx += dx1;
            rx += dx2;

            lc[0] += ptr_dc1[0];
            lc[1] += ptr_dc1[1];
            rc[0] += ptr_dc2[0];
            rc[1] += ptr_dc2[1];
        }

    }
}

void Image::fillTexture(uint16_t col1, uint16_t col2) {
    for(int y = 0; y<height; y++) {
        for(int x = 0; x<width; x++) {

            int c = ((x/16)%2) ^ ((y/16)%2);

            *(((uint16_t*)buffer) + y * width + x) = (c == 0 ? col1 : col2);
        }
    }
}

Image *generateTexture(uint16_t col1, uint16_t col2) {
    Image *tex = new Image(256, 256);

    tex->fillTexture(col1, col2);

    return tex;
}

void Image::hLineTexFix(Image* texture, int x1, int y, int x2, int_fast32_t* cord1, int_fast32_t* cord2, int_fast32_t lz, int_fast32_t rz) {
    auto buf = (buffer + y * width);

    if (x2 < 0) return;
    if (x1 >= width) return;

//    if (y < 0 || y >= image.h) {
//        SDL_Log("ERROR ACCESS %d", y);
//        return;
//    }

    int w = (x2 - x1 + 1);

    int_fast32_t dc[2] = {
            (cord2[0] - cord1[0]) / w,
            (cord2[1] - cord1[1]) / w
    };

    int_fast32_t c[2] = {
            (cord1[0]),
            (cord1[1]),
    };

    int_fast32_t dz = (rz - lz) / w;
    int_fast32_t z = lz;

    if (x1 < 0) {
        c[0] -= x1 * dc[0];
        c[1] -= x1 * dc[1];
        z -= x1 * dz;
        x1 = 0;
    }

    if (x2>= width) {
        x2 = width - 1;
    }

    for (int x = x1; x<=x2; x++){
        float zz = 1.0f/z;

        int u = c[0] * zz;
        int v = c[1] * zz;

        auto texLine = texture->buffer + v * texture->width;

        buf[x] = texLine[u];

        c[0] += dc[0];
        c[1] += dc[1];

        z += dz;
    }
}


void Image::drawTexTriangleFix(Image* texture, TexTriangleFixPoint *points, uint32_t pointSize) {
    TexTriangleFixPoint *p0 = points;
    TexTriangleFixPoint *p1 = (TexTriangleFixPoint *) shiftBytes(points, pointSize);
    TexTriangleFixPoint *p2 = (TexTriangleFixPoint *) shiftBytes(points, 2 * pointSize);

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

    int_fast32_t unwrap0[2] = { (int_fast32_t)(p0->u * (texture->width - 1)), (int_fast32_t)(p0->v * (texture->height - 1)) };
    int_fast32_t unwrap1[2] = { (int_fast32_t)(p1->u * (texture->width - 1)), (int_fast32_t)(p1->v * (texture->height - 1)) };
    int_fast32_t unwrap2[2] = { (int_fast32_t)(p2->u * (texture->width - 1)), (int_fast32_t)(p2->v * (texture->height - 1)) };


    if (p0->y <= p1->y && p1->y > 0) {
        int_fast32_t lx = toFixedF(p0->x);
        int_fast32_t rx = toFixedF(p0->x);

        int_fast32_t lz = toFixedFloat(1.0f/p0->z);
        int_fast32_t rz = toFixedFloat(1.0f/p0->z);

        int_fast32_t lc[2] = {
                toFixedFloat((float)unwrap0[0]/p0->z),
                toFixedFloat((float)unwrap0[1]/p0->z)
        };

        int_fast32_t rc[2] = {
                toFixedFloat((float)unwrap0[0]/p0->z),
                toFixedFloat((float)unwrap0[1]/p0->z)
        };

        int_fast32_t dx1 = toFixedF(p1->x - p0->x) / (p1->y - p0->y + 1);
        int_fast32_t dz1 = toFixedFloat(1.0f/p1->z - 1.0f/p0->z) / (p1->y - p0->y + 1);

        int_fast32_t dc1[2] = {
                toFixedFloat(unwrap1[0]/p1->z - unwrap0[0]/p0->z) / (p1->y - p0->y + 1),
                toFixedFloat(unwrap1[1]/p1->z - unwrap0[1]/p0->z) / (p1->y - p0->y + 1),
        };


        int_fast32_t dx2 = toFixedF(p2->x - p0->x) / (p2->y - p0->y + 1);
        int_fast32_t dz2 = toFixedFloat(1.0f/p2->z - 1.0f/p0->z) / (p2->y - p0->y + 1);

        int_fast32_t dc2[2] = {
                toFixedFloat(unwrap2[0]/p2->z - unwrap0[0]/p0->z) / (p2->y - p0->y + 1),
                toFixedFloat(unwrap2[1]/p2->z - unwrap0[1]/p0->z) / (p2->y - p0->y + 1),
        };

        int_fast32_t *p_dc1 = dc1;
        int_fast32_t *p_dc2 = dc2;

        if (dx1 > dx2) {
            swap(dx1, dx2);
            swap(dz1, dz2);
            swap(p_dc1, p_dc2);
        }

        int sy = p0->y;
        int ey = p1->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;
            lz -= dz1 * sy;
            rz -= dz2 * sy;
            lc[0] -= p_dc1[0] * sy;
            lc[1] -= p_dc1[1] * sy;
            rc[0] -= p_dc2[0] * sy;
            rc[1] -= p_dc2[1] * sy;
            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for (int i = 0; i < ey - sy + 1; i++) {
            hLineTexFix(texture, (int)unfixF(lx), sy + i, (int)unfixF(rx), (lc), (rc), lz, rz);
            lx += dx1;
            rx += dx2;

            lz += dz1;
            rz += dz2;

            lc[0] += p_dc1[0];
            lc[1] += p_dc1[1];
            rc[0] += p_dc2[0];
            rc[1] += p_dc2[1];

        }
    }


    if (p1->y <= p2->y && p2->y > 0) {
        int_fast32_t dx1 = toFixedF(p2->x - p1->x) / (p2->y - p1->y + 1);  // 2 -> 3
        int_fast32_t dx2 = toFixedF(p2->x - p0->x) / (p2->y - p0->y + 1);  // 1 -> 3

        int_fast32_t dz1 = toFixedFloat(1.0f/p2->z - 1.0f/p1->z) / (p2->y - p1->y + 1);  // 2 -> 3
        int_fast32_t dz2 = toFixedFloat(1.0f/p2->z - 1.0f/p0->z) / (p2->y - p0->y + 1);  // 1 -> 3

        int_fast32_t dc1[2] = {
                toFixedFloat(unwrap2[0]/p2->z - unwrap1[0]/p1->z) / (p2->y - p1->y + 1),
                toFixedFloat(unwrap2[1]/p2->z - unwrap1[1]/p1->z) / (p2->y - p1->y + 1)
        };

        int_fast32_t dc2[2] = {
                toFixedFloat(unwrap2[0]/p2->z - unwrap0[0]/p0->z) / (p2->y - p0->y + 1),
                toFixedFloat(unwrap2[1]/p2->z - unwrap0[1]/p0->z) / (p2->y - p0->y + 1)
        };


        int_fast32_t rx = toFixedF(p0->x) + (p1->y - p0->y + 1) * dx2;
        int_fast32_t lx = toFixedF(p1->x);

        int_fast32_t rz = toFixedFloat(1.0f/p0->z) + (p1->y - p0->y + 1) * dz2;
        int_fast32_t lz = toFixedFloat(1.0f/p1->z);


        int_fast32_t rc[2] = {
                toFixedFloat(unwrap0[0]/p0->z) + (p1->y - p0->y + 1) * dc2[0],
                toFixedFloat(unwrap0[1]/p0->z) + (p1->y - p0->y + 1) * dc2[1]
        };

        int_fast32_t lc[2] = {
                toFixedFloat(unwrap1[0]/p1->z),
                toFixedFloat(unwrap1[1]/p1->z)
        };

        int_fast32_t *p_dc1 = dc1;
        int_fast32_t *p_dc2 = dc2;

        if (lx > rx) {
            swap(lx, rx);
            swap(lz, rz);
            swap(dz1, dz2);
            swap(lc[0], rc[0]);
            swap(lc[1], rc[1]);
            swap(dx1, dx2);
            swap(p_dc1, p_dc2);
        }

        int sy = p1->y;
        int ey = p2->y;

        if (sy < 0) {
            lx -= dx1 * sy;
            rx -= dx2 * sy;
            lz -= dz1 * sy;
            rz -= dz2 * sy;
            lc[0] -= p_dc1[0] * sy;
            lc[1] -= p_dc1[1] * sy;
            rc[0] -= p_dc2[0] * sy;
            rc[1] -= p_dc2[1] * sy;
            sy = 0;
        }

        if (ey >= height) {
            ey = height - 1;
        }

        for(int i=0; i<ey - sy + 1; i++) {
            hLineTexFix(texture, (int)unfixF(lx), sy + i, (int)unfixF(rx), (lc), (rc), lz, rz);
            lx += dx1;
            rx += dx2;

            lz += dz1;
            rz += dz2;

            lc[0] += p_dc1[0];
            lc[1] += p_dc1[1];
            rc[0] += p_dc2[0];
            rc[1] += p_dc2[1];
        }

    }
}