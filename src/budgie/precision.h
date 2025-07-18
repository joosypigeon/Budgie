#ifndef PRECISION_H
#define PRECISION_H

#include <float.h>
#include <math.h>

#define INT_FMT "%d"
#define FLOAT_FMT "%f"
#define DOUBLE_FMT "%f"  // changed from %lf because printf promotes float to double

#ifdef USE_FLOAT
    typedef float buReal;
    #define buSqrt sqrtf
    #define buPow powf
    #define buAbs fabsf
    #define buLog logf
    #define buExp expf
    #define buSin sinf
    #define buCos cosf
    #define REAL_FMT         "%f"
    #define REAL_MAX         FLT_MAX
    #define REAL_MIN         FLT_MIN
    #define REAL_EPSILON     FLT_EPSILON
    #define REAL_NEG_MIN     (-FLT_MAX)
#else
    typedef double buReal;
    #define REAL_FMT         "%f"   // was %lf — fixed for printf
    #define buSqrt sqrt
    #define buPow pow
    #define buAbs fabs
    #define buLog log
    #define buExp exp
    #define buSin sin
    #define buCos cos
    #define REAL_MAX         DBL_MAX
    #define REAL_MIN         DBL_MIN
    #define REAL_EPSILON     DBL_EPSILON
    #define REAL_NEG_MIN     (-DBL_MAX)
#endif

#endif // PRECISION_H