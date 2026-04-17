/**
 * @file mathutils.h
 * @brief Math Utilities Header
 * 
 * Provides 2D vector operations, random number generation,
 * and common mathematical helper functions.
 */

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * CONSTANTS
 * ============================================================ */

#define PI            3.14159265358979323846f
#define PI_HALF       1.57079632679489661923f
#define PI_DOUBLE     6.28318530717958647692f
#define DEG_TO_RAD    0.01745329251f
#define RAD_TO_DEG    57.29577951308f

/* ============================================================
 * VECTOR2D STRUCTURE
 * ============================================================ */

typedef struct {
    float x;
    float y;
} Vec2;

/* ============================================================
 * VECTOR2D FUNCTIONS
 * ============================================================ */

/** Create a 2D vector */
static inline Vec2 Vec2_Create(float x, float y) {
    Vec2 v; v.x = x; v.y = y; return v;
}

/** Add two vectors */
static inline Vec2 Vec2_Add(Vec2 a, Vec2 b) {
    return Vec2_Create(a.x + b.x, a.y + b.y);
}

/** Subtract two vectors */
static inline Vec2 Vec2_Sub(Vec2 a, Vec2 b) {
    return Vec2_Create(a.x - b.x, a.y - b.y);
}

/** Multiply vector by scalar */
static inline Vec2 Vec2_Mul(Vec2 v, float s) {
    return Vec2_Create(v.x * s, v.y * s);
}

/** Get vector length (magnitude) */
static inline float Vec2_Length(Vec2 v) {
    return (float)sqrt(v.x * v.x + v.y * v.y);
}

/** Get vector length squared (faster than Length) */
static inline float Vec2_LengthSq(Vec2 v) {
    return v.x * v.x + v.y * v.y;
}

/** Normalize a vector */
static inline Vec2 Vec2_Normalize(Vec2 v) {
    float len = Vec2_Length(v);
    if (len > 0.0f) {
        return Vec2_Create(v.x / len, v.y / len);
    }
    return Vec2_Create(0.0f, 0.0f);
}

/** Dot product of two vectors */
static inline float Vec2_Dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

/** Clamp vector components to range */
static inline Vec2 Vec2_Clamp(Vec2 v, float minVal, float maxVal) {
    v.x = (v.x < minVal) ? minVal : ((v.x > maxVal) ? maxVal : v.x);
    v.y = (v.y < minVal) ? minVal : ((v.y > maxVal) ? maxVal : v.y);
    return v;
}

/** Distance between two points */
static inline float Vec2_Distance(Vec2 a, Vec2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return (float)sqrt(dx * dx + dy * dy);
}

/* ============================================================
 * RANDOM NUMBER GENERATION
 * ============================================================ */

/** Initialize random number generator with seed */
void Math_RandomSeed(unsigned int seed);

/** Get a random integer between min and max (inclusive) */
int Math_RandomInt(int minVal, int maxVal);

/** Get a random float between 0.0 and 1.0 */
float Math_RandomFloat(void);

/** Get a random float between min and max */
float Math_RandomRange(float minVal, float maxVal);

/* ============================================================
 * MATH HELPER FUNCTIONS
 * ============================================================ */

/** Clamp a value to range [min, max] */
static inline float Math_Clamp(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

/** Clamp an integer to range [min, max] */
static inline int Math_ClampInt(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

/** Linear interpolation between a and b by t (0-1) */
static inline float Math_Lerp(float a, float b, float t) {
    return a + (b - a) * Math_Clamp(t, 0.0f, 1.0f);
}

/** Convert degrees to radians */
static inline float Math_DegToRad(float degrees) {
    return degrees * DEG_TO_RAD;
}

/** Convert radians to degrees */
static inline float Math_RadToDeg(float radians) {
    return radians * RAD_TO_DEG;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHUTILS_H */
