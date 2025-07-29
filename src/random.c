#include "budgie/random.h"  // Cross-platform random number support
#include <math.h>
#include "budgie/math_constants.h"
#include "budgie/core.h"

// Detect platform
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#else
    #define PLATFORM_POSIX
#endif

// Cross-platform random number generator
long buRandCross() {
#ifdef PLATFORM_POSIX
    return random();  // Better RNG (usually 0 to 2^31-1)
#else
    return rand();    // Fallback (typically 0 to 32767)
#endif
}

// Optional: seed function
void buSeed(unsigned seed) {
#ifdef PLATFORM_POSIX
    srandom(seed);
#else
    srand(seed);
#endif
}

// Define RAND_MAX_CROSS for the maximum possible value
#ifdef PLATFORM_POSIX
    #define RAND_MAX_CROSS 2147483647L  // POSIX random() uses 2^31 - 1
#else
    #define RAND_MAX_CROSS RAND_MAX     // system-defined, typically 32767
#endif

buReal buRandCrossNormalised() {
    return (buReal)buRandCross() / ((buReal)RAND_MAX_CROSS + 1.0);
}

buReal buRandomReal(buReal min, buReal max) {
    return min + buRandCrossNormalised() * (max - min);
}

int buRandomInt(unsigned max) {
    return (int)buRandCross() % max;
}

buReal buRandomBinomial(buReal scale) {
    return (buRandCrossNormalised()-buRandCrossNormalised())*scale;
}

buQuaternion buRandomUnitQuaternion() {
    buQuaternion q;
    do {
        q = (buQuaternion) {
            buRandCrossNormalised(),
            buRandCrossNormalised(),
            buRandCrossNormalised(),
            buRandCrossNormalised()
        };
    } while (q.r == 0.0f && q.i == 0.0f && q.j == 0.0f && q.k == 0.0f);

    buNormaliseQuaternion(&q);
    return q;
}

buVector3 buRandomVectorByScale(buReal scale) {
    return (buVector3) {
        buRandomBinomial(scale),
        buRandomBinomial(scale),
        buRandomBinomial(scale)};
}

buVector3 buRandomXZVector(buReal scale) {
    return (buVector3) {
        buRandomBinomial(scale),
        (buReal)0.0,
        buRandomBinomial(scale)};
}

buVector3 buRandomVectorByVector(const buVector3 *scale)
{
    return (buVector3) {
        buRandomBinomial(scale->x),
        buRandomBinomial(scale->y),
        buRandomBinomial(scale->z)};
}

buVector3 buRandomVectorByRange(const buVector3 *min, const buVector3 *max)
{
    return (buVector3) {
        buRandomReal(min->x, max->x),
        buRandomReal(min->y, max->y),
        buRandomReal(min->z, max->z)};
}


/**
 * Generate a normally distributed (Gaussian) random float
 * using the Box-Muller transform.
 *
 * @param mean   The mean (μ) of the distribution
 * @param stddev The standard deviation (σ)
 * @return A random float drawn from N(mean, stddev^2)
 */
buReal buRandomNormal(buReal mean, buReal stddev) {
    // Generate two independent uniform random floats in (0, 1)
    // Add 1.0 and divide by RAND_MAX + 2.0 to avoid 0.0 or 1.0 edge cases
    buReal u1 = ((buReal)buRandCross() + 1.0) / ((buReal)RAND_MAX_CROSS + 2.0);
    buReal u2 = ((buReal)buRandCross() + 1.0) / ((buReal)RAND_MAX_CROSS + 2.0);

    // Perform Box-Muller transform to get a standard normal deviate
    buReal z0 = buSqrt(-2.0 * buLog(u1)) * buCos(2.0 * M_PI * u2);

    // Scale and shift to desired mean and standard deviation
    return z0 * stddev + mean;
}