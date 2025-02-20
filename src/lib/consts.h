#pragma once
#include "raylib.h"
#include "haptic.h"

#define UNIT_ROD_HEIGHT 30
#define TABLET_WIDTH 1000
#define TABLET_HEIGHT 600
#define NB_ROD_LENGTHS 10

extern const float ROD_WIDTH_FOR_LENGTH[NB_ROD_LENGTHS];
extern const Color ROD_COLOR_FOR_LENGTH[NB_ROD_LENGTHS];
Signal RodSignalForLength(int l);