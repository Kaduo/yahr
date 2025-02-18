#include "raylib.h"
#include "rodfactory.h"
#include "haptic.h"
#include "pretest.h"
#include "stdlib.h"

const uint16_t PERIODS[] = {10, 30, 90, 180};

Rods *OnlyHapticStacked(const StandardRodFactory *rodFactory) {
    Rods *rods = malloc(sizeof(Rods));
    *rods = EmptyRods(14);
    for (int i = 0; i < 14; i++) {
        AppendRod(rods, CreateRod(rodFactory, 3, 0, i*rodFactory->rodUnitHeight, WHITE, (Signal){
            .amplitude = 255,
            .period = PERIODS[i%4],
            .signalShape = SignalShape_SINE,
            0,
        }));
    }
    return rods;
}
