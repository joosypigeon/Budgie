#ifndef CORE_H
#define CORE_H

#include "precision.h"

typedef union {
    struct { buReal x, y, z; };
    buReal v[3];
} buVector3;

void buCopyFromTo(buVector3 from, buVector3 *to);

typedef struct {
    buVector3 X;
    buVector3 Y;
    buVector3 Z;
} buCoordinateFrame;

typedef union {
    struct {
        buReal r;
        buReal i;
        buReal j;
        buReal k;
    };
    buReal data[4];
} buQuaternion;

buReal buVector3Norm(buVector3 v);
buReal buVector3SquareNorm(buVector3 v);
buVector3 buVector3Normalise(buVector3 v);
buVector3 buVector3Scalar(buVector3 v, buReal scalar);
buVector3 buVector3Add(buVector3 a, buVector3 b);
buVector3 buVector3Difference(buVector3 a, buVector3 b);
buVector3 buVector3ComponentProduct(buVector3 a, buVector3 b);
buReal buVector3Dot(buVector3 a, buVector3 b);
buVector3 buVector3Cross(buVector3 a, buVector3 b);
buCoordinateFrame buMakeVector3OrthonormalBasis(buVector3 u, buVector3 v, buVector3 w);
void buNormaliseQuaternion(buQuaternion *q);
buReal buQuaternionLength(buQuaternion q);
buVector3 buDivideVectorComponents(buVector3 a, buVector3 b);


#endif // CORE_H