#include "raylib.h"
#include "network.h"
#include "stdlib.h"
#include "string.h"
#include "vec.h"
#include "stdio.h"
#include "session.h"
#include "gameobject.h"
#include "consts.h"
#include "stdbool.h"
#include "haptic.h"
#include "vec.h"
#include "rodsystem.h"
#include "problem.h"

int main(void) {
    InputService innerInputService = NewPhysicalInputService();
    WriteTapInputService inputService = NewWriteTapInputService(innerInputService, "test_save.tap");
    // ReadTapInputService inputService = NewReadTapInputService("test_save.tap");
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver((InputService*)&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);

    InitWindow(TABLET_WIDTH, TABLET_HEIGHT, "v2");
    SetTargetFPS(60);

    Problem prob = MakeRandomInterestingProblem();
    RodSystem rodSystem = NewRodSystem(&inputService, &hapticService);
    // RodSystem_loadRods(&rodSystem, "data/pre-test/sessions/colors+haptic+length/session_0.rods");
    printf("so far so good ?\n");
    rodSystem.rods = GetRandomRodsForProblem(prob, 15);

    printf("hehehe\n");
    while (!WindowShouldClose()) {
        ((InputService*)(&inputService))->poll((InputService*)&inputService);
        HapticService_update(&hapticService, GetFrameTime());

        BeginDrawing();
        ClearBackground(WHITE);
        RodSystem_updateAndDrawRods(&rodSystem);
        EndDrawing();
    }
    WriteTapInputServiceCloseSave(&inputService);
    return 0;
}