#pragma once
#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

typedef struct Tap {
    float frameTime;
    Vector2 mousePosition;
    bool flipped;
} Tap;

void WriteTap(FILE *file, Tap tap);
Tap ReadTap(FILE *file);
