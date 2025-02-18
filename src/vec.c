#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "assert.h"
#include "vec.h"
#include "stdio.h"

Vec EmptyVec(int capacity, int sizeOfContent) {
    Vec Vec = {
        .capacity = capacity,
        .size = 0,
        .sizeOfContent = sizeOfContent,
        .contents = malloc(capacity*sizeOfContent)
    };
    return Vec;
}

void ReallocVecIfFull(Vec *Vec) {
    if (Vec->size == Vec->capacity) {
        Vec->contents = realloc(Vec->contents, 2*Vec->capacity*Vec->sizeOfContent);
        assert(Vec->contents != NULL);
        Vec->capacity *= 2;
    }
}

void PushVec(Vec *Vec, void *elem) {
    ReallocVecIfFull(Vec);
    memcpy((void*)((uintptr_t)(Vec->contents) + Vec->size*Vec->sizeOfContent), elem, Vec->sizeOfContent);
    Vec->size += 1;
}

void PopVec(Vec *vec) {
    if (vec->size > 0) {
        vec->size -= 1;
    }
}

void Vec_remove(Vec *me, int index) {
    if (0 <= index && index < me->size) {
        for (int i = index+1; i < me->size; i++) {
            memcpy((void*)((uintptr_t)(me->contents) + i*me->sizeOfContent), (void*)((uintptr_t)(me->contents) + (i-1)*me->sizeOfContent), me->sizeOfContent);
        }
        me->size -= 1;
    }
}

// TODO return a pointer to the data itself, instead of a pointer that needs to be de-referenced
void *IndexVec(const Vec *vec, int index) {
    if ((0 <= index) && (index < vec->size)) {
        return (void*)((uintptr_t)(vec->contents) + index*vec->sizeOfContent);
    } else {
        return NULL;
    }
}

void Vec_replace(Vec *vec, int index, void *item) {
    void *ptr = IndexVec(vec, index);
    if (ptr != NULL) {
        memcpy(ptr, item, vec->sizeOfContent);
    }
}

void ClearVec(Vec *Vec) {
    Vec->size = 0;
}

void *CountingIterator_next(Iterator *_me) {
    CountingIterator *me = (CountingIterator*)_me;
    void *res = me->_old_next(&me->super);
    me->i += 1;
    return res;
}

void CountingIterator_reinit(Iterator *_me) {
    CountingIterator *me = (CountingIterator*)_me;
    me->_old_reinit(&me->super);
    me->i = 0;
}

void CountingIterator_Construct(CountingIterator *me) {
    me->_old_next = me->super.next;
    me->_old_reinit = me->super.reinit;
    me->super.next = CountingIterator_next;
    me->super.reinit = CountingIterator_reinit;
}

void *VecIterator_next(Iterator *_me) {
    VecIterator *me =(VecIterator*)_me;
    if (me->i < me->data->size) {
        me->i++;
        return IndexVec(me->data, me->i - 1);
    }
    return NULL;
}

void VecIterator_reinit(Iterator *_me) {
    VecIterator *me = (VecIterator*)_me;
    me->i = 0;
}

void VecIterator_Construct(VecIterator *me) {
    me->super.next = VecIterator_next;
    me->super.reinit = VecIterator_reinit;
}

VecIterator NewVecIterator(const Vec *data) {
    Iterator super = {NULL};
    VecIterator vecIterator = {
        .super = super,
        .i = 0,
        .data = data
    };
    VecIterator_Construct(&vecIterator);
    return vecIterator;
}


void *VecIteratorSkip_next(Iterator *_me) {
    VecIteratorSkip *me = (VecIteratorSkip*)_me;
    if (me->i < me->data->size) {
        me->i++;
        if (me->i - 1 == me->skip) {
            if (me->i < me->data->size) {
                me->i++;
            } else {
                return NULL;
            }
        }
        return IndexVec(me->data, me->i - 1);
    }
    return NULL;
}

void VecIteratorSkip_reinit(Iterator *_me) {
    VecIteratorSkip *me = (VecIteratorSkip*)_me;
    me->i = 0;
}

void VecIteratorSkip_Construct(VecIteratorSkip *me) {
    me->super.next = VecIteratorSkip_next;
    me->super.reinit = VecIteratorSkip_reinit;
}

VecIteratorSkip NewVecIteratorSkip(const Vec *data, int skip) {
    Iterator super = {NULL};
    VecIteratorSkip vecIteratorSkip = {
        .super = super,
        .i = 0,
        .skip = skip,
        .data = data
    };
    VecIteratorSkip_Construct(&vecIteratorSkip);
    return vecIteratorSkip;
}