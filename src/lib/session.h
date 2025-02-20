#pragma once
#include "input.h"
#include "haptic.h"
#include "rodfactory.h"

typedef struct Session {
    InputService *inputService;
    HapticService *hapticService;
    Rods *rods;
} Session;

void Session_loadFromFile(Session *me, char *fileName);
void Session_saveToFile(Session *me, char *fileName);
Session NewSession(InputService *inputService, HapticService *hapticService);