#pragma once
#include "rodfactory.h"
#include "stdio.h"
#include "input.h"
#include "haptic.h"

typedef struct Tap {
    float frameTime;
    Vector2 mousePosition;
    bool flipped;
} Tap;



void WriteRods(FILE *file, const Rods *rods);
Rods *ReadRods(FILE *file);
void WriteTap(FILE *file, Tap tap);
Tap ReadTap(FILE *file);
