//
// Created by Duch on 10.04.2026.
//

#ifndef SOFTWARERENDERBASIC_MATRIX_H
#define SOFTWARERENDERBASIC_MATRIX_H


class Matrix {
public:
    Matrix();

    void setIdentity();
    void basicProjection(float dist, int width, int height);
    void flatProjection(float dist, int width, int height);

    void mul(float *in, float *out, int vectCount);

    Matrix operator*(const Matrix& other) const;

    void setTransform(float x, float y, float z);
    void setRotationX(float angle);
    void setRotationY(float angle);
    void setRotationZ(float angle);
    void setScale(float s);

    float data[4 * 4];
};

Matrix lookAt(float* eye, float* target);

#endif //SOFTWARERENDERBASIC_MATRIX_H
