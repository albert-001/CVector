#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CVector.h"

Vector* createVector(int capacity) {
    Vector* v = (Vector*)malloc(sizeof(Vector));
    v->capacity = capacity;
    v->data = (int*)malloc(capacity * sizeof(V_ELEM_TYPE)); // Initial capacity
    v->size = 0;
    v->maxUsedIndex = -1;
    v->bitmap = (char*)malloc((capacity / 8 + 1));
    memset(v->bitmap, 0, v->capacity / 8 + 1);
    return v;
}

void freeVector(Vector* v) {
    free(v->data);
    free(v->bitmap);
    free(v);
}

static bool isSlotFree(Vector* v, int index) {
    int byteIndex = index / 8;
    int bitIndex = index % 8;
    return (v->bitmap[byteIndex] & (1 << bitIndex)) == 0;
}

static void markSlotUsed(Vector* v, int index) {
    int byteIndex = index / 8;
    int bitIndex = index % 8;
    v->bitmap[byteIndex] |= (1 << bitIndex);
}

static void markSlotFree(Vector* v, int index) {
    int byteIndex = index / 8;
    int bitIndex = index % 8;
    v->bitmap[byteIndex] &= ~(1 << bitIndex);
}

static void pushBack(Vector* v, V_ELEM_TYPE value) {
    if (v->size == v->capacity) {
        // Double the capacity if full
        v->capacity *= 2;
        v->data = (int*)realloc(v->data, v->capacity * sizeof(V_ELEM_TYPE));
        if (v->data == NULL) {
            printf("Failed to allocate memory for data\n");
            v->capacity /= 2;
            return;
        }
        v->bitmap = (char*)realloc(v->bitmap, (v->capacity / 8 + 1));
        if (v->bitmap == NULL) {
            printf("Failed to allocate memory for bitmap\n");
            v->capacity /= 2;
            return;
        }
        memset(v->bitmap + (v->capacity / 2 + 7)/8, 0, ((v->capacity + 7)/8 - (v->capacity/2 + 7)/8));
    }
    memcpy(v->data + v->size, &value, sizeof(V_ELEM_TYPE));
    markSlotUsed(v,v->size);
    v->size++;
    v->maxUsedIndex = v->size - 1;
}

static void compact(Vector* v, bool force) {
    if (!force) {
        int freeSlots =  v->maxUsedIndex + 1 - v->size;
        if (freeSlots < 10) return;
        float ratio = (float)freeSlots / (v->maxUsedIndex + 1);
        if (ratio <= MEM_COMPACT_THREASHOLD) return;
    }

    int writeIndex = 0;
    int readIndex = 0;
    while (readIndex < v->capacity) {
        if (!isSlotFree(v, readIndex)) {
            if (writeIndex != readIndex) {
                memcpy(&v->data[writeIndex], &v->data[readIndex], sizeof(V_ELEM_TYPE));
                markSlotUsed(v, writeIndex);
            }
            writeIndex++;
        }
        readIndex++;
    }
    v->maxUsedIndex = writeIndex - 1;
}

void vector_add(Vector* v, V_ELEM_TYPE value) {
    for (int i = 0; i < v->size; i++) {
        if (isSlotFree(v, i)) {
            memcpy(v->data + i, &value, sizeof(V_ELEM_TYPE));
            markSlotUsed(v, i);
            v->size++;
            return;
        }
    }

    // If no free slot found, push the element to the end
    pushBack(v, value);
}

int size(Vector* v) {
    return v->size;
}

int capacity(Vector* v) {
    return v->capacity;
}

V_ELEM_TYPE* data(Vector* v) {
    return v->data;
}

int* getElementsByIndex(Vector* v, int* indices, int numIndices) {
    int* elements = (int*)malloc(numIndices * sizeof(V_ELEM_TYPE));
    int count = 0;
    int elementIndex = 0;
    for (int i = 0; i <= v->maxUsedIndex; i++) {
        if (!isSlotFree(v, i)) {
            if (count == indices[elementIndex]) {
                memcpy(elements + elementIndex, v->data + i, sizeof(V_ELEM_TYPE));
                elementIndex++;
                if (elementIndex == numIndices) return elements;
            }
            count++;
        }
    }
    return NULL; // not enough elements
}

void deleteElementsByIndex(Vector* v, int* indices, int numIndices) {
    assert(numIndices <= v->size);
    int actualIndex = 0;
    int elementIndex = 0;
    for (int i = 0; i <= v->maxUsedIndex; i++) {
        if (!isSlotFree(v, i)) {
            if (actualIndex == indices[elementIndex]) {
                markSlotFree(v, i);
                v->size--;

                // Update maxUsedIndex if necessary
                if (i == v->maxUsedIndex) {
                    int newMaxIndex = i - 1;
                    while (newMaxIndex >= 0 && isSlotFree(v, newMaxIndex)) {
                        newMaxIndex--;
                    }
                    v->maxUsedIndex = newMaxIndex;
                }

                elementIndex++;
                if (elementIndex == numIndices) {
                    compact(v, false);
                    return;
                }
            }
            actualIndex++;
        }
    }
}

void vector_remove(Vector* v, int index) {
    int indices[1] = {index};
    deleteElementsByIndex(v, indices, 1);
}

int vector_get(Vector* v, int index) {
    int indices[1] = {index};
    int* elements = getElementsByIndex(v, indices, 1);
    assert(elements != NULL);
    int r = elements[0];
    free(elements);
    return r;
}


// ---------------------- Below are test cases

void test_vector_init() {
    Vector* vector = createVector(10);
    assert(vector->size == 0);
    assert(vector->capacity == 10);
    for (int i = 0; i < vector->capacity; i++) {
        assert(isSlotFree(vector, i));
    }
    freeVector(vector);
    printf("test_vector_init passed\n");
}
void test_vector_add() {
    Vector* vector = createVector(10);
    vector_add(vector, 10);
    vector_add(vector, 20);
    vector_add(vector, 30);
    assert(vector->size == 3);
    assert(vector_get(vector, 0) == 10);
    assert(vector_get(vector, 1) == 20);
    assert(vector_get(vector, 2) == 30);
    freeVector(vector);
    printf("test_vector_add passed\n");
}

void test_vector_remove() {
    Vector* vector = createVector(10);
    vector_add(vector, 10);
    vector_add(vector, 20);
    vector_add(vector, 30);
    vector_remove(vector, 1);
    assert(vector->size == 2);
    assert(vector_get(vector, 0) == 10);
    assert(vector_get(vector, 1) == 30);
    // Try removing last remaining element
    vector_remove(vector, 0);
    assert(vector->size == 1);
    assert(vector_get(vector, 0) == 30);
    freeVector(vector);
    printf("test_vector_remove passed\n");
}

void test_vector_resize() {
    Vector* vector = createVector(10);
    for (int i = 0; i < 20; i++) {
        vector_add(vector, i);
    }
    assert(vector->size == 20);
    for (int i = 0; i < 20; i++) {
        assert(vector_get(vector, i) == i);
    }
    freeVector(vector);
    printf("test_vector_resize passed\n");
}

void test_vector_compact() {
    Vector* vector = createVector(10);
    vector_add(vector, 10);
    vector_add(vector, 20);
    vector_add(vector, 30);
    vector_add(vector, 40);
    vector_add(vector, 50);
    assert(vector->maxUsedIndex == 4);
    int list[2] = {1, 3};
    deleteElementsByIndex(vector, list, 2);
    assert(vector->size == 3);
    assert(vector->maxUsedIndex == 4);
    compact(vector, true);
    assert(vector->maxUsedIndex == 2);
    assert(vector->data[0] == 10);
    assert(vector->data[1] == 30);
    assert(vector->data[2] == 50);
    // Try removing last remaining element
    vector_remove(vector, 2);
    assert(vector->maxUsedIndex == 1);

    freeVector(vector);
    printf("test_vector_compact passed\n");
}

int main() {
    test_vector_init();
    test_vector_add();
    test_vector_remove();
    test_vector_resize();
    test_vector_compact();
    printf("All tests passed!\n");
    return 0;
}