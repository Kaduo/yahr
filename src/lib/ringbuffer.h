#pragma once

typedef struct RingBuffer {
    void *contents;
    int capacity;
    int start;
    int size;
    int sizeOfContent;
} RingBuffer;

void AppendToBuffer(RingBuffer *ringBuffer, void *content);
void *ReadBuffer(RingBuffer *ringBuffer, int index);
void *PopBuffer(RingBuffer *ringBuffer);
void ClearBuffer(RingBuffer *ringBuffer);
RingBuffer EmptyRingBuffer(int capacity, int sizeOfContent);