#include "raylib.h"
#include "lib/rodfactory.h"
#include "lib/haptic.h"
#include "stdlib.h"
#include "lib/consts.h"
#include "time.h"

const uint16_t PERIODS[] = {10, 30, 90, 180};

Rods *MakeFullRods(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, (l+1), 0, 0, ROD_COLOR_FOR_LENGTH[l], RodSignalForLength(l)));
        }
    }

    return rods;
}

Rods *OnlyHaptics(const StandardRodFactory *rodFactory, float zoneMargin) {
    Rods *rods = malloc(sizeof(Rods));
    *rods = EmptyRods(14);
    for (int i = 0; i < 14; i++) {
        AppendRod(rods, CreateRod(rodFactory, 3, 0, i*rodFactory->rodUnitHeight, WHITE, (Signal){
            .amplitude = 255,
            .period = PERIODS[i%4],
            .signalShape = SignalShape_SINE,
            .pulses = -1,
            .angle = -1,
        }));
    }

    
    return rods;
}


// Ok, il me faut : une marge sur les côtés, une marge en bas, et une marge en haut. Il faut : que je dis aux zones jusqu'où elles vont en haut,
// et au réglettes jusqu'où elles ont le droit d'aller en bas.

Vec MakeZones(int nbZones, float intraZoneXMargin, float rodZoneYMargin, float borderZoneXMargin, float borderZoneYMargin) {
    float zoneWidth = (TABLET_WIDTH - intraZoneXMargin*nbZones + borderZoneXMargin)/(nbZones);
    float zoneHeight = (TABLET_HEIGHT/2 - borderZoneYMargin - rodZoneYMargin);

    Vec zones = EmptyVec(nbZones, sizeof(Zone*));
    for (int i =0; i < nbZones; i ++) {
        Zone *zone = NewZone((Rectangle){.x = borderZoneXMargin + i*(zoneWidth+intraZoneXMargin), .y = TABLET_HEIGHT/2. + rodZoneYMargin,  .height = zoneHeight, .width = zoneWidth}, BLACK);
        PushVec(&zones, &zone);
    }

    return zones;
}



int main(void) {
    srand((unsigned int)time(NULL));
    InputService inputService = NewPhysicalInputService();
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver(&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);
    StandardRodFactory rodFactory = {.inputService = (InputService*)&inputService, .hapticService = &hapticService, .rodUnitHeight = 30};

    Rods *rods = MakeFullRods(&rodFactory, (int[10]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
    ShuffleRods(rods, 0, TABLET_WIDTH - 10*rodFactory.rodUnitHeight, 0, TABLET_HEIGHT/2. - rodFactory.rodUnitHeight);
    Vec zones = MakeZones(4, 10, 10 + rodFactory.rodUnitHeight, 10, 10);

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(TABLET_WIDTH, TABLET_HEIGHT, "Haptic Rods");

    ToggleFullscreen();

    while (!WindowShouldClose()) {
        HapticService_update(rodFactory.hapticService, GetFrameTime());
        UpdateRods(rods);
        BeginDrawing();
        ClearBackground(WHITE);
        for (int i = 0; i < zones.size; i++) {
            Zone *zone = *(Zone**)IndexVec(&zones,i);
            zone->graphicsComponent->draw(zone->graphicsComponent);
        }

        DrawRods(rods);
        EndDrawing();
    }
    CloseWindow();
    return 0;

}