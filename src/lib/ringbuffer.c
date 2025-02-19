#include "stdlib.h"
#include "stdbool.h"
#include "ringbuffer.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"

bool IsBufferFull(RingBuffer *ringBuffer) {
    return ringBuffer->size == ringBuffer->capacity;
}

void ReallocBufferIfFull(RingBuffer *me) {
    if (IsBufferFull(me)) {
        me->contents = realloc(me->contents, 2*me->capacity*sizeof(me->sizeOfContent));

        // "Straighten" the buffer
        int i;
        for (i = 0; i < me->start; i++) {
            memcpy((void*)((uintptr_t)me->contents + (me->capacity + i)*me->sizeOfContent), (void*)((uintptr_t)me->contents + i), me->sizeOfContent);
        }

        me->capacity *= 2;
    }
}

void AppendToBuffer(RingBuffer *me, void *content) {
    ReallocBufferIfFull(me);
    memcpy((void*)((uintptr_t)(me->contents) + ((me->start + me->size) % me->capacity)*me->sizeOfContent), content, me->sizeOfContent);
    me->size += 1;
}

bool IsIndexInBuffer(RingBuffer *ringBuffer, int index) {
    return 0 <= index && index < ringBuffer->size;
}

void *ReadBuffer(RingBuffer *me, int index) {
    if (IsIndexInBuffer(me, index)) {
        return (void*)((uintptr_t)me->contents + ((me->start + index) % me->capacity)*me->sizeOfContent);
    } else {
        return NULL;
    }
}

void *PopBuffer(RingBuffer *me) {
    if (me->size > 0) {
        void *firstElem = ReadBuffer(me, 0);
        me->start = (me->start + 1) % me->capacity;
        me->size -= 1;
        return firstElem;
    } else {
        return NULL;
    }
}

void ClearBuffer(RingBuffer *me) {
    me->size = 0;
    me->start = 0;
}

RingBuffer EmptyRingBuffer(int capacity, int sizeOfContent) {
    RingBuffer ringBuffer = {
        .contents = malloc(capacity*sizeOfContent),
        .capacity = capacity,
        .start = 0,
        .size = 0,
        .sizeOfContent = sizeOfContent,
    };
    return ringBuffer;
}