#pragma once
#include "raylib.h"
#include "vec.h"
#include "haptic.h"

typedef struct Rod2 {
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