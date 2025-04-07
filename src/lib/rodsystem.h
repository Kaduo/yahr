#pragma once
#include "raylib.h"
#include "vec.h"
#include "haptic.h"

typedef struct Rod2 {
    int n;
    Rectangle rect;
    Color color;
    Signal signal;
} Rod2;

typedef struct RodSystem {
    InputService *inputService;
    HapticService *hapticService;
    Vec rods;
    int previousSelectedRodId;
    int selectedRodId;
    Vector2 selectionOffset;
    bool collided;
    bool previousCollided;
} RodSystem;

Rod2 *RodSystem_getSelectedRod(RodSystem *me);
RodSystem NewRodSystem(InputService *inputService, HapticService *hapticService);
void RodSystem_updateAndDrawRods(RodSystem *me);
void RodSystem_loadRods(RodSystem *me, char *filename);
void ShuffleRods2(Vec *rods, float xMin, float xMax, float yMin, float yMax);
void WriteRods2(Vec *rods, char *filename);
void WriteRod2(FILE *file, Rod2 rod);
Rod2 ReadRod2(FILE *file);
void SaveRodSystem(RodSystem *me, char *filename);