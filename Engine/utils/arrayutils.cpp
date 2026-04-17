/**
 * @file arrayutils.cpp
 * @brief Dynamic Array Utilities Implementation
 */

#define _CRT_SECURE_NO_WARNINGS

#include "arrayutils.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * INTERNAL HELPERS
 * ============================================================ */

static int Array_EnsureCapacity(DynamicArray* arr, size_t minCapacity) {
    if (arr->capacity >= minCapacity) return 1;
    
    size_t newCapacity = arr->capacity == 0 ? 16 : arr->capacity * 2;
    if (newCapacity < minCapacity) newCapacity = minCapacity;
    
    void** newData = (void**)realloc(arr->data, newCapacity * sizeof(void*));
    if (!newData) return 0;
    
    arr->data = newData;
    arr->capacity = newCapacity;
    return 1;
}

/* ============================================================
 * ARRAY FUNCTIONS
 * ============================================================ */

int Array_Init(DynamicArray* arr, size_t elementSize, size_t initialCapacity) {
    if (!arr) return 0;
    
    arr->elementSize = elementSize;
    arr->size = 0;
    arr->capacity = initialCapacity;
    
    arr->data = (void**)calloc(initialCapacity, sizeof(void*));
    if (!arr->data && initialCapacity > 0) return 0;
    
    return 1;
}

void Array_Free(DynamicArray* arr) {
    if (!arr) return;
    
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

int Array_Add(DynamicArray* arr, const void* element) {
    if (!arr || !element) return 0;
    
    if (!Array_EnsureCapacity(arr, arr->size + 1)) return 0;
    
    size_t pos = arr->size++;
    arr->data[pos] = malloc(arr->elementSize);
    if (!arr->data[pos]) {
        arr->size--;
        return 0;
    }
    memcpy(arr->data[pos], element, arr->elementSize);
    
    return 1;
}

int Array_Insert(DynamicArray* arr, size_t index, const void* element) {
    if (!arr || !element || index > arr->size) return 0;
    
    if (!Array_EnsureCapacity(arr, arr->size + 1)) return 0;
    
    /* Shift elements to make room */
    for (size_t i = arr->size; i > index; i--) {
        arr->data[i] = arr->data[i - 1];
    }
    
    arr->data[index] = malloc(arr->elementSize);
    if (!arr->data[index]) return 0;
    
    memcpy(arr->data[index], element, arr->elementSize);
    arr->size++;
    
    return 1;
}

int Array_RemoveLast(DynamicArray* arr, void* output) {
    if (!arr || arr->size == 0) return 0;
    
    size_t pos = arr->size - 1;
    
    if (output) {
        memcpy(output, arr->data[pos], arr->elementSize);
    }
    
    free(arr->data[pos]);
    arr->data[pos] = NULL;
    arr->size--;
    
    return 1;
}

int Array_RemoveAt(DynamicArray* arr, size_t index, void* output) {
    if (!arr || index >= arr->size) return 0;
    
    if (output) {
        memcpy(output, arr->data[index], arr->elementSize);
    }
    
    free(arr->data[index]);
    
    /* Shift elements */
    for (size_t i = index; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    
    arr->data[arr->size - 1] = NULL;
    arr->size--;
    
    return 1;
}

size_t Array_Find(DynamicArray* arr, const void* target, 
                  int (*compare)(const void*, const void*)) {
    if (!arr || !compare) return (size_t)-1;
    
    for (size_t i = 0; i < arr->size; i++) {
        if (compare(arr->data[i], target) == 0) {
            return i;
        }
    }
    
    return (size_t)-1;
}

void* Array_Get(DynamicArray* arr, size_t index) {
    if (!arr || index >= arr->size) return NULL;
    return arr->data[index];
}

int Array_Set(DynamicArray* arr, size_t index, const void* element) {
    if (!arr || !element || index >= arr->size) return 0;
    
    memcpy(arr->data[index], element, arr->elementSize);
    return 1;
}

void Array_Clear(DynamicArray* arr) {
    if (!arr) return;
    
    for (size_t i = 0; i < arr->size; i++) {
        free(arr->data[i]);
        arr->data[i] = NULL;
    }
    
    arr->size = 0;
}

size_t Array_Size(DynamicArray* arr) {
    return arr ? arr->size : 0;
}

int Array_IsEmpty(DynamicArray* arr) {
    return arr && arr->size == 0;
}

/* ============================================================
 * SORTING & SEARCHING
 * ============================================================ */

static int CompareInts(const void* a, const void* b) {
    int va = *(const int*)a;
    int vb = *(const int*)b;
    return (va > vb) - (va < vb);
}

void Array_Sort(DynamicArray* arr, int (*compare)(const void*, const void*)) {
    if (!arr || !compare || arr->size <= 1) return;
    
    /* Simple insertion sort for small arrays */
    for (size_t i = 1; i < arr->size; i++) {
        void* key = malloc(arr->elementSize);
        if (!key) break;
        memcpy(key, arr->data[i], arr->elementSize);
        
        size_t j = i;
        while (j > 0 && compare(arr->data[j - 1], key) > 0) {
            arr->data[j] = arr->data[j - 1];
            j--;
        }
        
        arr->data[j] = key;
    }
}

size_t Array_BinarySearch(DynamicArray* arr, const void* target,
                          int (*compare)(const void*, const void*)) {
    if (!arr || !compare) return (size_t)-1;
    
    size_t low = 0, high = arr->size - 1;
    
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        int cmp = compare(arr->data[mid], target);
        
        if (cmp == 0) return mid;
        else if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    
    return (size_t)-1;
}
