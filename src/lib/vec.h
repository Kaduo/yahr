#ifndef VEC_INCLUDE
#define VEC_INCLUDE
typedef struct Vec Vec;

struct Vec {
    int capacity;
    int size;
    int sizeOfContent;
    void *contents;
};

typedef struct Iterator Iterator;

struct Iterator {
    void* (*next)(Iterator *me);
    void (*reinit)(Iterator *me);
};

typedef struct CountingIterator {
    Iterator super;
    int i;
    void* (*_old_next)(Iterator *me);
    void (*_old_reinit)(Iterator *me);
} CountingIterator;

typedef struct VecIterator {
    Iterator super;
    int i;
    const Vec *data;
} VecIterator;

typedef struct VecIteratorSkip {
    Iterator super;
    int i;
    int skip;
    const Vec *data;
} VecIteratorSkip;

VecIteratorSkip NewVecIteratorSkip(const Vec *data, int skip);

Vec EmptyVec(int capacity, int sizeOfContent);
void PushVec(Vec *Vec, void *elem);
void *IndexVec(const Vec *Vec, int index);
void ClearVec(Vec *Vec);
void PopVec(Vec *vec);
void Vec_replace(Vec *vec, int index, void *item);
void Vec_remove(Vec *me, int index);

#endif