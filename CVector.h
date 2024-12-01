#pragma once

#define MEM_COMPACT_THREASHOLD (0.4)

typedef int V_ELEM_TYPE;

typedef struct {
    V_ELEM_TYPE *data;
    int capacity;
    int size;
    char *bitmap; //mark used (1) and free (0) slots
    int maxUsedIndex; // size <= maxUsedIndex
} Vector;


Vector* createVector(int capacity);
void freeVector(Vector* v);
void vector_add(Vector* v, V_ELEM_TYPE value);
int vector_get(Vector* v, int index);
void vector_remove(Vector* v, int index);
int* getElementsByIndex(Vector* v, int* indices, int numIndices);
void deleteElementsByIndex(Vector* v, int* indices, int numIndices);
int size(Vector* v);
int capacity(Vector* v);
V_ELEM_TYPE* data(Vector* v);
