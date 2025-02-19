#include "vec.h"
#include "string.h"
#include "stdbool.h"
#include "hashmap.h"

int HashMap_keyIndex(HashMap *me, char *key) {
    for (int i = 0; i < me->keys.size; i++) {
        if (strcmp(*(char**)IndexVec(&me->keys, i), key) == 0) {
            return i;
        }
    }
    return -1;
}

void *HashMap_index(HashMap *me, char *key) {
    int keyIndex = HashMap_keyIndex(me, key);
    if (keyIndex >= 0) {
        return IndexVec(&me->items, keyIndex);
    } else {
        return NULL;
    }
}


void HashMap_insert(HashMap *me, char *key, void *item) {
    int keyIndex = HashMap_keyIndex(me, key);
    if (keyIndex >= 0) {
        Vec_replace(&me->items, keyIndex, item);
    } else {
        PushVec(&me->keys, &key);
        PushVec(&me->items, item);    
    }
}

void HashMap_remove(HashMap *me, char *key) {
    int keyIndex = HashMap_keyIndex(me, key);
    if (keyIndex >= 0) {
        Vec_remove(&me->items, keyIndex);
        Vec_remove(&me->keys, keyIndex);
    }
}

HashMap NewHashmap(int maxKeyLength, int sizeOfContent, int capacity) {
    return (HashMap){
        .items = EmptyVec(capacity, sizeOfContent),
        .keys = EmptyVec(capacity, (maxKeyLength + 1)*sizeof(char)),
    };
}

void HashMap_clear(HashMap *me) {
    ClearVec(&me->keys);
    ClearVec(&me->items);
}