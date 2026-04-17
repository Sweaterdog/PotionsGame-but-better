/**
 * @file mathutils.cpp
 * @brief Math Utilities Implementation
 */

#define _CRT_SECURE_NO_WARNINGS

#include "mathutils.h"
#include <stdlib.h>
#include <time.h>

static unsigned int g_randomSeed = 0;

void Math_RandomSeed(unsigned int seed) {
    g_randomSeed = seed;
}

int Math_RandomInt(int minVal, int maxVal) {
    if (g_randomSeed == 0) {
        g_randomSeed = (unsigned int)time(NULL);
    }
    
    /* Simple LCG random number generator */
    g_randomSeed = g_randomSeed * 1103515245 + 12345;
    
    unsigned int range = (unsigned int)(maxVal - minVal + 1);
    return minVal + (int)((g_randomSeed >> 16) % range);
}

float Math_RandomFloat(void) {
    return (float)Math_RandomInt(0, 10000) / 10000.0f;
}

float Math_RandomRange(float minVal, float maxVal) {
    return minVal + Math_RandomFloat() * (maxVal - minVal);
}
