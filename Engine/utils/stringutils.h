/**
 * @file stringutils.h
 * @brief String Utilities Header
 * 
 * Enhanced string manipulation functions for the engine.
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * STRING FUNCTIONS
 * ============================================================ */

/** Safe string copy with buffer size limit */
char* String_SafeCopy(char* dest, const char* src, size_t destSize);

/** Safe string concatenation with buffer size limit */
char* String_SafeConcat(char* dest, const char* src, size_t destSize);

/** Convert string to uppercase (modifies in place) */
void String_ToUpper(char* str);

/** Convert string to lowercase (modifies in place) */
void String_ToLower(char* str);

/** Trim leading and trailing whitespace */
void String_Trim(char* str);

/** Check if string contains substring */
int String_Contains(const char* haystack, const char* needle);

/** Check if string starts with prefix */
int String_StartsWith(const char* str, const char* prefix);

/** Check if string ends with suffix */
int String_EndsWith(const char* str, const char* suffix);

/** Replace all occurrences of oldChar with newChar */
void String_Replace(char* str, char oldChar, char newChar);

/** Split string by delimiter into array (returns count) */
int String_Split(const char* str, char delimiter, char** parts, int maxParts);

/* ============================================================
 * FORMATTING FUNCTIONS
 * ============================================================ */

/** Format a number with thousand separators */
void String_FormatNumber(char* buffer, size_t bufferSize, long value);

/** Format a float to string with specified decimal places */
void String_FormatFloat(char* buffer, size_t bufferSize, float value, int decimals);

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

/** Get string length (safe wrapper for strlen) */
size_t String_Length(const char* str);

/** Compare two strings case-insensitively */
int String_CompareNoCase(const char* a, const char* b);

#ifdef __cplusplus
}
#endif

#endif /* STRINGUTILS_H */
