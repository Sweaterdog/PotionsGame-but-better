/**
 * @file stringutils.cpp
 * @brief String Utilities Implementation
 */

#define _CRT_SECURE_NO_WARNINGS

#include "stringutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* String_SafeCopy(char* dest, const char* src, size_t destSize) {
    if (!dest || !src || destSize == 0) return dest;
    
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
    return dest;
}

char* String_SafeConcat(char* dest, const char* src, size_t destSize) {
    if (!dest || !src || destSize == 0) return dest;
    
    strncat(dest, src, destSize - strlen(dest) - 1);
    return dest;
}

void String_ToUpper(char* str) {
    if (!str) return;
    for (; *str; str++) {
        *str = (char)toupper((unsigned char)*str);
    }
}

void String_ToLower(char* str) {
    if (!str) return;
    for (; *str; str++) {
        *str = (char)tolower((unsigned char)*str);
    }
}

void String_Trim(char* str) {
    if (!str) return;
    
    /* Trim leading whitespace */
    char* start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    
    /* Trim trailing whitespace */
    char* end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    *(end + 1) = '\0';
    
    /* Move trimmed string to beginning if needed */
    if (start != str) {
        memmove(str, start, (size_t)(end - start + 2));
    }
}

int String_Contains(const char* haystack, const char* needle) {
    if (!haystack || !needle) return 0;
    return strstr(haystack, needle) != NULL ? 1 : 0;
}

int String_StartsWith(const char* str, const char* prefix) {
    if (!str || !prefix) return 0;
    size_t prefixLen = strlen(prefix);
    return strncmp(str, prefix, prefixLen) == 0 ? 1 : 0;
}

int String_EndsWith(const char* str, const char* suffix) {
    if (!str || !suffix) return 0;
    
    size_t strLen = strlen(str);
    size_t suffixLen = strlen(suffix);
    
    if (suffixLen > strLen) return 0;
    
    return strcmp(str + strLen - suffixLen, suffix) == 0 ? 1 : 0;
}

void String_Replace(char* str, char oldChar, char newChar) {
    if (!str) return;
    for (; *str; str++) {
        if (*str == oldChar) *str = newChar;
    }
}

int String_Split(const char* str, char delimiter, char** parts, int maxParts) {
    if (!str || !parts || maxParts <= 0) return 0;
    
    /* Make a mutable copy of the string */
    size_t len = strlen(str);
    char* mutableStr = (char*)malloc(len + 1);
    if (!mutableStr) return 0;
    strcpy(mutableStr, str);
    
    int count = 0;
    char* start = mutableStr;
    char* end = mutableStr;
    
    while (*end && count < maxParts - 1) {
        if (*end == delimiter) {
            parts[count] = start;
            *end = '\0';
            count++;
            start = end + 1;
        }
        end++;
    }
    
    /* Last part */
    if (count < maxParts - 1) {
        parts[count] = start;
        count++;
    }
    
    free(mutableStr);
    return count;
}

void String_FormatNumber(char* buffer, size_t bufferSize, long value) {
    if (!buffer || !bufferSize) return;
    snprintf(buffer, bufferSize, "%ld", value);
}

void String_FormatFloat(char* buffer, size_t bufferSize, float value, int decimals) {
    if (!buffer || !bufferSize) return;
    snprintf(buffer, bufferSize, "%.*f", decimals, value);
}

size_t String_Length(const char* str) {
    return str ? strlen(str) : 0;
}

int String_CompareNoCase(const char* a, const char* b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}
