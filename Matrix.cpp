//
// Created by Duch on 10.04.2026.
//

#include "Matrix.h"
#include <cmath>
#include <cstring>

Matrix::Matrix() {
    setIdentity();
}

void Matrix::setIdentity() {
    for(int i = 0; i<16; i++) {
        data[i] = 0.0f;
    }
    data[0] = data[5] = data[10] = data[15] = 1.0f;
}

void Matrix::basicProjection(float dist, int width, int height) {
    float ratio = (float)width/ height;

    data[0] = ratio * dist; data[1] = 0.0f; data[2] = width/2; data[3] = 0.0f;
    data[4] = 0.0f; data[5] = -dist; data[6] = height/2; data[7] = 0.0f;
    data[8] = 0.0f; data[9] = 0.0f; data[10] = 1.0f; data[11] = 0.0f;
    data[12] = 0.0f; data[13] = 0.0f; data[14] = 1.0f; data[15] = 0.0f;
}

void Matrix::setScale(float s) {
    setIdentity();
    data[0] = s;
    data[5] = s;
    data[10] = s;
}

void Matrix::flatProjection(float dist, int width, int height) {
    float ratio = (float)width/ height;
    float w = 90.0f;

    data[0] = ratio * dist; data[1] = 0.0f; data[2] = 0.0; data[3] = w * width/2;
    data[4] = 0.0f; data[5] = -dist; data[6] = 0.0; data[7] = w * height/2;
    data[8] = 0.0f; data[9] = 0.0f; data[10] = 1.0f; data[11] = 0.0f;
    data[12] = 0.0f; data[13] = 0.0f; data[14] = 0.0f; data[15] = w;
}


void Matrix::mul(float *in, float *out, int vectCount, int vertSize) {
    for(int i = 0; i<vectCount; i++) {
        out[i * vertSize + 0] = data[0]  * in[i * vertSize + 0] + data[1]  * in[i * vertSize + 1] + data[2]  * in[i * vertSize + 2] + data[3]  * in[i * vertSize + 3];
        out[i * vertSize + 1] = data[4]  * in[i * vertSize + 0] + data[5]  * in[i * vertSize + 1] + data[6]  * in[i * vertSize + 2] + data[7]  * in[i * vertSize + 3];
        out[i * vertSize + 2] = data[8]  * in[i * vertSize + 0] + data[9]  * in[i * vertSize + 1] + data[10] * in[i * vertSize + 2] + data[11] * in[i * vertSize + 3];
        out[i * vertSize + 3] = data[12] * in[i * vertSize + 0] + data[13] * in[i * vertSize + 1] + data[14] * in[i * vertSize + 2] + data[15] * in[i * vertSize + 3];
        if (vertSize > 4) {
            memcpy(out + i * vertSize + 4, in + i * vertSize + 4, (vertSize - 4) * sizeof(float));
        }
    }
}


Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {

            result.data[row * 4 + col] = 0.0f;

            for (int k = 0; k < 4; k++) {
                result.data[row * 4 + col] +=
                        data[row * 4 + k] * other.data[k * 4 + col];
            }
        }
    }

    return result;
}


void Matrix::setTransform(float x, float y, float z)
{
    setIdentity();

    data[0 * 4 + 3] = x; // Tx
    data[1 * 4 + 3] = y; // Ty
    data[2 * 4 + 3] = z; // Tz
}


void Matrix::setRotationX(float angle)
{
    setIdentity();

    float c = cos(angle);
    float s = sin(angle);

    data[1 * 4 + 1] = c;
    data[1 * 4 + 2] = -s;
    data[2 * 4 + 1] = s;
    data[2 * 4 + 2] = c;
}

void Matrix::setRotationY(float angle)
{
    setIdentity();

    float c = cos(angle);
    float s = sin(angle);

    data[0 * 4 + 0] = c;
    data[0 * 4 + 2] = s;
    data[2 * 4 + 0] = -s;
    data[2 * 4 + 2] = c;
}

void Matrix::setRotationZ(float angle)
{
    setIdentity();

    float c = cos(angle);
    float s = sin(angle);

    data[0 * 4 + 0] = c;
    data[0 * 4 + 1] = -s;
    data[1 * 4 + 0] = s;
    data[1 * 4 + 1] = c;
}

void normalize(float* p) {
    float l = sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
    p[0] /= l;
    p[1] /= l;
    p[2] /= l;
}

void cross(float *out, float *a, float *b)
{
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

Matrix lookAt(float* eye, float* target)
{
    Matrix R;
    R.setIdentity();

    float up[3] = {0,1.0,0};

    float forward[3] = {
            target[0] - eye[0],
            target[1] - eye[1],
            target[2] - eye[2]
    };
    normalize(forward);

    float dotFU = forward[0]*up[0] + forward[1]*up[1] + forward[2]*up[2];

    if (fabs(dotFU) > 0.999f) {
        // choose a different up vector
        up[0] = 0.0f;
        up[1] = 0.0f;
        up[2] = 1.0f;
    }

    float right[3];
    cross(right, up, forward);
    normalize(right);

    float newUp[3];
    cross(newUp, forward, right);
    normalize(newUp);

    R.data[0*4 + 0] = right[0];
    R.data[0*4 + 1] = right[1];
    R.data[0*4 + 2] = right[2];

    R.data[1*4 + 0] = newUp[0];
    R.data[1*4 + 1] = newUp[1];
    R.data[1*4 + 2] = newUp[2];

    R.data[2*4 + 0] = forward[0];
    R.data[2*4 + 1] = forward[1];
    R.data[2*4 + 2] = forward[2];

    Matrix T;
    T.setTransform(-eye[0], -eye[1], -eye[2]);

    return R * T;
}