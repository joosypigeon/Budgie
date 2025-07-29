#include "linalg3x3.h"
#include "../../budgie/random.h"

const Matrix3x3 I3 = (Matrix3x3){
                    1.0, 0.0, 0.0,
                    0.0, 1.0, 0.0,
                    0.0, 0.0, 1.0
};

// Multiply matrix with vector: v' = M * v
inline buVector3 Matrix3x3MultiplyVector(Matrix3x3 M, buVector3 v)
{
    return (buVector3){
        M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z,
        M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z,
        M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z
    };
}

// Multiply two 3x3 matrices: C = A * B
inline Matrix3x3 Matrix3x3Multiply(Matrix3x3 A, Matrix3x3 B)
{
    Matrix3x3 C;

    for (int i = 0; i < 3; ++i) {         // Row of A
        for (int j = 0; j < 3; ++j) {     // Column of B
            C.m[i][j] = A.m[i][0] * B.m[0][j]
                      + A.m[i][1] * B.m[1][j]
                      + A.m[i][2] * B.m[2][j];
        }
    }

    return C;
}

// Multiply a 3x3 matrix by a scalar
inline Matrix3x3 Matrix3x3ScalarMultiply(Matrix3x3 M, buReal k)
{
    return (Matrix3x3){
                k*M.m[0][0], k*M.m[0][1], k*M.m[0][2],
                k*M.m[1][0], k*M.m[1][1], k*M.m[1][2],              
                k*M.m[2][0], k*M.m[2][1], k*M.m[2][2]};
}

// Add two 3x3 matrices: C = A + B
inline Matrix3x3 Matrix3x3Add(Matrix3x3 A, Matrix3x3 B)
{
    return (Matrix3x3){
        A.m[0][0]+B.m[0][0], A.m[0][1]+B.m[0][1], A.m[0][2]+B.m[0][2],
        A.m[1][0]+B.m[1][0], A.m[1][1]+B.m[1][1], A.m[1][2]+B.m[1][2],
        A.m[2][0]+B.m[2][0], A.m[2][1]+B.m[2][1], A.m[2][2]+B.m[2][2]
    };
}

inline buVector3 Matrix3x3Diagonal(Matrix3x3 M) {
    return (buVector3){M.m[0][0], M.m[1][1], M.m[2][2]};
}

Matrix3x3 Matrix3x3Transpose(Matrix3x3 M)
{
    return (Matrix3x3){
        M.m[0][0], M.m[1][0], M.m[2][0],
        M.m[0][1], M.m[1][1], M.m[2][1],
        M.m[0][2], M.m[1][2], M.m[2][2]};
}

buReal Matrix3x3Determinant(Matrix3x3 M)
{
    buReal a = M.m[0][0], b = M.m[0][1], c = M.m[0][2];
    buReal d = M.m[1][0], e = M.m[1][1], f = M.m[1][2];
    buReal g = M.m[2][0], h = M.m[2][1], i = M.m[2][2];

    return a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
}

Matrix3x3 Matrix3x3Inverse(Matrix3x3 M)
{
    buReal det = Matrix3x3Determinant(M);
    if (det == 0.0f) return (Matrix3x3){0};

    buReal a = M.m[0][0], b = M.m[0][1], c = M.m[0][2];
    buReal d = M.m[1][0], e = M.m[1][1], f = M.m[1][2];
    buReal g = M.m[2][0], h = M.m[2][1], i = M.m[2][2];

    Matrix3x3 adj = (Matrix3x3){
        (e*i - f*h), -(b*i - c*h),  (b*f - c*e),
       -(d*i - f*g),  (a*i - c*g), -(a*f - c*d),
        (d*h - e*g), -(a*h - b*g),  (a*e - b*d)};

    buReal invDet = 1.0f / det;

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            adj.m[r][c] *= invDet;

    return adj;
}

static buVector3 getColumn(Matrix3x3 M, size_t column) {
    return (buVector3){M.m[0][column], M.m[1][column], M.m[2][column]};
}

static Matrix3x3 Matrix3x3FromColumns(buVector3 c1,buVector3 c2, buVector3 c3) {
    return (Matrix3x3){
                c1.x, c2.x, c3.x,
                c1.y, c2.y, c3.y,
                c1.z, c2.z, c3.z
    };
} 

Matrix3x3 Matrix3x3Reorthonormalize(Matrix3x3 R) {
    buVector3 c1 = getColumn(R, 0);
    buVector3 c2 = getColumn(R, 1);

    // Step 1: normalize r1
    c1 = buVector3Normalise(c1);

    // Step 2: make r2 orthogonal to r1
    c2 = buVector3Difference(c2, buVector3Scalar(c1, buVector3Dot(c1, c2)));
    c2 = buVector3Normalise(c2);

    // Step 3: r3 = r1 × r2 (automatically orthogonal to both)
    buVector3 c3 = buVector3Cross(c1, c2);

    // Should be approximately 1
    assert(buAbs(buVector3Norm(c1) - 1.0f) < 1e-5);
    assert(buAbs(buVector3Norm(c2) - 1.0f) < 1e-5);
    assert(buAbs(buVector3Norm(c3) - 1.0f) < 1e-5);

    return Matrix3x3FromColumns(c1, c2, c3);
}

Matrix3x3 buQuaternionToMatrix3x3(buQuaternion q) {
    buReal r = q.r, i = q.i, j = q.j, k = q.k;

    buReal ii = i * i;
    buReal jj = j * j;
    buReal kk = k * k;
    buReal ij = i * j;
    buReal ik = i * k;
    buReal jk = j * k;
    buReal ri = r * i;
    buReal rj = r * j;
    buReal rk = r * k;

    Matrix3x3 M = {{
        {1.0f - 2.0f * (jj + kk),     2.0f * (ij - rk),         2.0f * (ik + rj)},
        {    2.0f * (ij + rk),     1.0f - 2.0f * (ii + kk),     2.0f * (jk - ri)},
        {    2.0f * (ik - rj),         2.0f * (jk + ri),     1.0f - 2.0f * (ii + jj)}
    }};

    return M;
}

Matrix3x3 buRandomRotationMatrix() {
    buQuaternion q = buRandomUnitQuaternion();  // Already normalised
    return buQuaternionToMatrix3x3(q);
}


void Matrix3x3ToAxisAngle(Matrix3x3 R, buVector3 *axis, buReal *angle) {
    // Compute the trace: sum of diagonal elements
    buReal trace = R.m[0][0] + R.m[1][1] + R.m[2][2];
    buReal cos_theta = (trace - 1.0f) * 0.5f;

    // Clamp to valid acosf input range to avoid NaNs
    if (cos_theta > 1.0f) cos_theta = 1.0f;
    if (cos_theta < -1.0f) cos_theta = -1.0f;

    *angle = buAcos(cos_theta);  // radians

    // If angle is very small, the rotation is close to identity
    if (buAbs(*angle) < 1e-6f) {
        *axis = (buVector3){1.0f, 0.0f, 0.0f};  // arbitrary axis
        return;
    }

    buReal sin_theta = buSin(*angle);
    if (buAbs(sin_theta) < 1e-6f) sin_theta = 1e-6f;

    // Axis from skew-symmetric part of R
    axis->x = (R.m[2][1] - R.m[1][2]) / (2.0f * sin_theta);
    axis->y = (R.m[0][2] - R.m[2][0]) / (2.0f * sin_theta);
    axis->z = (R.m[1][0] - R.m[0][1]) / (2.0f * sin_theta);

    *axis = buVector3Normalise(*axis);
}

void Matrix3x3Print(Matrix3x3 M) {
    printf("[\n");
    for (int i = 0; i < 3; ++i) {
        printf("  [ % .6f, % .6f, % .6f ]\n",
               M.m[i][0], M.m[i][1], M.m[i][2]);
    }
    printf("]\n");
}