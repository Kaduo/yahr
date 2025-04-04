#pragma once
#include "gameobject.h"
#include "raylib.h"
#include "input.h"
#include "signal.h"
#include "haptic.h"

typedef struct Rods {
    Vec rods;
} Rods;

typedef struct StandardRodFactory {
    InputService *inputService;
    HapticService *hapticService;
    float rodUnitHeight;
} StandardRodFactory;


Rods EmptyRods(int capacity);
void AppendRod(Rods *me, Rod *rod);
void DrawRods(Rods *me);
void UpdateRods(Rods *me);
Rod *CreateRod(const StandardRodFactory *me, int length, float x, float y, Color color, Signal signal);
void SetRodServices(Rods *rods, InputService *inputService, HapticService *hapticService);
void ClearRods(Rods *me);
void FreeRods(Rods *me);
void ShuffleRods(Rods *rods, float xMin, float xMax, float yMin, float yMax);