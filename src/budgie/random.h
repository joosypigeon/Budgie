#ifndef RANDOM_H
#define RANDOM_H

#include "core.h"
#include <stdlib.h>

long buRandCross();
void buSeed(unsigned seed);
buReal buRandCrossNormalised();
buReal buRandomReal(buReal min, buReal max);
int buRandomInt(unsigned max);
buReal buRandomBinomial(buReal scale);
buQuaternion buRandomUnitQuaternion();
buVector3 buRandomVectorByScale(buReal scale);
buVector3 buRandomXZVector(buReal scale);
buVector3 buRandomVectorByVector(const buVector3 *scale);
buVector3 buRandomVectorByRange(const buVector3 *min, const buVector3 *max);
buReal buRandomNormal(buReal mean, buReal stddev);
#endif // RANDOM_H
