#ifndef LINALG3X3_H
#define LINALG3X3_H

#include "../../budgie/core.h"
#include "../../budgie/precision.h"
#include <stdio.h>
#include <assert.h>

typedef struct Matrix3x3 {
    buReal m[3][3];  // Row-major 3x3 matrix
} Matrix3x3;

extern const Matrix3x3 I3;

// Multiply matrix with vector: v' = M * v
buVector3 Matrix3x3MultiplyVector(Matrix3x3 M, buVector3 v);

// Multiply two 3x3 matrices: C = A * B
Matrix3x3 Matrix3x3Multiply(Matrix3x3 A, Matrix3x3 B);

// Multiply a 3x3 matrix by a scalar
Matrix3x3 Matrix3x3ScalarMultiply(Matrix3x3 M, buReal k);

// Add two 3x3 matrices: C = A + B
Matrix3x3 Matrix3x3Add(Matrix3x3 A, Matrix3x3 B);

buVector3 Matrix3x3Diagonal(Matrix3x3 M);

// Transpose of a 3x3 matrix
Matrix3x3 Matrix3x3Transpose(Matrix3x3 M);

// Determinant of a 3x3 matrix
buReal Matrix3x3Determinant(Matrix3x3 M);

// Inverse of a 3x3 matrix (returns zero matrix if non-invertible)
Matrix3x3 Matrix3x3Inverse(Matrix3x3 M);

Matrix3x3 Matrix3x3Reorthonormalize(Matrix3x3 R);

Matrix3x3 buRandomRotationMatrix();

void Matrix3x3ToAxisAngle(Matrix3x3 R, buVector3 *axis, buReal *angle);

void Matrix3x3Print(Matrix3x3 M);

#endif // LINALG3X3_H
