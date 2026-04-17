/**
 * @file arrayutils.h
 * @brief Dynamic Array Utilities Header
 * 
 * Enhanced dynamic array implementation with generic-style operations.
 * Builds on the existing DynamicArray concept from the original codebase.
 */

#ifndef ARRAYUTILS_H
#define ARRAYUTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * DYNAMIC ARRAY STRUCTURE
 * ============================================================ */

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
    size_t elementSize;
} DynamicArray;

/* ============================================================
 * ARRAY FUNCTIONS
 * ============================================================ */

/** Initialize a dynamic array with given element size and initial capacity */
int Array_Init(DynamicArray* arr, size_t elementSize, size_t initialCapacity);

/** Free all memory associated with the array */
void Array_Free(DynamicArray* arr);

/** Add an element to the end of the array */
int Array_Add(DynamicArray* arr, const void* element);

/** Insert an element at a specific index */
int Array_Insert(DynamicArray* arr, size_t index, const void* element);

/** Remove and return the last element */
int Array_RemoveLast(DynamicArray* arr, void* output);

/** Remove an element at a specific index */
int Array_RemoveAt(DynamicArray* arr, size_t index, void* output);

/** Find the index of an element using a comparison function */
size_t Array_Find(DynamicArray* arr, const void* target, 
                  int (*compare)(const void*, const void*));

/** Get element at index (returns NULL if invalid) */
void* Array_Get(DynamicArray* arr, size_t index);

/** Set element at index */
int Array_Set(DynamicArray* arr, size_t index, const void* element);

/** Clear all elements from the array */
void Array_Clear(DynamicArray* arr);

/** Get current number of elements */
size_t Array_Size(DynamicArray* arr);

/** Check if array is empty */
int Array_IsEmpty(DynamicArray* arr);

/* ============================================================
 * SORTING & SEARCHING
 * ============================================================ */

/** Sort the array using quicksort */
void Array_Sort(DynamicArray* arr, int (*compare)(const void*, const void*));

/** Binary search (array must be sorted) */
size_t Array_BinarySearch(DynamicArray* arr, const void* target,
                          int (*compare)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif /* ARRAYUTILS_H */
