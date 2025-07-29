#include <math.h>
#include <assert.h>
#include <stdio.h>
#include "budgie/core.h"

void buCopyFromTo(buVector3 from, buVector3 *to) {
    to->x = from.x;
    to->y = from.y;
    to->z = from.z;
}

buReal buVector3Norm(buVector3 v) {
    return buSqrt(v.x*v.x + v.y*v.y +v.z*v.z);
}

buReal buVector3SquareNorm(buVector3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

buVector3 buVector3Normalise(buVector3 v) {
    buReal length = buVector3Norm(v);
    #ifndef NDEBUG
        assert(length != 0.0);
    #endif
    buVector3 unit;
    unit.x = v.x/length;
    unit.y = v.y/length;
    unit.z = v.z/length;
    return unit;
}

buVector3 buVector3Scalar(buVector3 v, buReal scalar) {
    buVector3 product;
    product.x = v.x*scalar;
    product.y = v.y*scalar;
    product.z = v.z*scalar;
    return product;
}

buVector3 buVector3Add(buVector3 a, buVector3 b) {
    buVector3 sum;
    sum.x = a.x+b.x;
    sum.y = a.y+b.y;
    sum.z = a.z+b.z;
    return sum;
}

buVector3 buVector3Difference(buVector3 a, buVector3 b) {
    buVector3 difference;
    difference.x = a.x-b.x;
    difference.y = a.y-b.y;
    difference.z = a.z-b.z;
    return difference;
}

buVector3 buVector3ComponentProduct(buVector3 a, buVector3 b) {
    buVector3 product;
    product.x = a.x*b.x;
    product.y = a.y*b.y;
    product.z = a.z*b.z;
    return product;
}

buReal buVector3Dot(buVector3 a, buVector3 b) {
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

buVector3 buVector3Cross(buVector3 a, buVector3 b) {
    buVector3 product;
    product.x = a.y*b.z-a.z*b.y;
    product.y = a.z*b.x-a.x*b.z;
    product.z = a.x*b.y-a.y*b.x;
    return product;
}


buCoordinateFrame buMakeVector3OrthonormalBasis(buVector3 u, buVector3 v, buVector3 w) {
    buCoordinateFrame frame = {0};

    // Ensure input vectors are not zero
    assert(buVector3SquareNorm(u) != 0.0);
    assert(buVector3SquareNorm(v) != 0.0);
    // X: Normalised u
    frame.X = buVector3Normalise(u);
    assert(buVector3SquareNorm(frame.X) != 0.0);
    // Y: Remove component of v along X
    buVector3 projection = buVector3Scalar(frame.X, buVector3Dot(frame.X, v));
    frame.Y = buVector3Difference(v, projection);
    assert(buVector3SquareNorm(frame.Y) != 0.0);
    frame.Y = buVector3Normalise(frame.Y);
    // Z: Cross product of X and Y
    frame.Z = buVector3Cross(frame.X, frame.Y);
    assert(buVector3Dot(frame.Z, w) > 0.0);
    return frame;
}

void buNormaliseQuaternion(buQuaternion *q) {
    buReal r = q->r, i = q->i, j = q->j, k = q->k;
    buReal d = r*r+i*i+j*j+k*k;
    // Check for zero length quaternion, and use the no-rotation
    // quaternion in that case.
    if (d < REAL_EPSILON) {
        q->r = (buReal)1.0;
        q->r = (buReal)0.0;
        q->i = (buReal)0.0;
        q->j = (buReal)0.0;
        q->k = (buReal)0.0;
    } else {
        d = ((buReal)1.0)/buSqrt(d);
        q->r = r*d;
        q->i = i*d;
        q->j = j*d;
        q->k = k*d;
    }
}

buReal buQuaternionLength(buQuaternion q) {
    buReal r = q.r, i = q.i, j = q.j, k = q.k;
    buReal d = r*r+i*i+j*j+k*k;
    return buSqrt(d);
}

buVector3 buDivideVectorComponents(buVector3 a, buVector3 b) {
    assert(b.x != 0.0 && b.y != 0.0 && b.z != 0.0);
    return (buVector3){a.x/b.x, a.y/b.y, a.z/b.z};
}
