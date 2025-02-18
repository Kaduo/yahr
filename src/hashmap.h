#pragma once
#include "vec.h"
typedef struct HashMap {
    Vec keys;
    Vec items;
} HashMap;

HashMap NewHashmap(int maxKeyLength, int sizeOfContent, int capacity);
void *HashMap_index(HashMap *me, char *key);
void HashMap_remove(HashMap *me, char *key);
void HashMap_insert(HashMap *me, char *key, void *item);
void HashMap_clear(HashMap *me);