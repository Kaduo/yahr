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

int main(int argc, char const *argv[]) {
    // InputService innerInputService = NewPhysicalInputService();
    // WriteTapInputService inputService = NewWriteTapInputService(innerInputService, "test_save.tap");
    if (argc <= 2) {
        fprintf(stderr, "Please enter the name of the files you wish to play.\n");
        exit(1);
    }
    char *tapSaveName = argv[1];
    ReadTapInputService inputService = NewReadTapInputService(tapSaveName);
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver((InputService*)&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);

    InitWindow(TABLET_WIDTH, TABLET_HEIGHT, "v2");
    SetTargetFPS(60);

    Problem prob = MakeRandomInterestingProblem();
    RodSystem rodSystem = NewRodSystem(&inputService, &hapticService);
    // RodSystem_loadRods(&rodSystem, "data/pre-test/sessions/colors+haptic+length/session_0.rods");
    // rodSystem.rods = GetRandomRodsForProblem(prob, 15);

    char *rodStartSaveName = argv[2];
    RodSystem_loadRods(&rodSystem, rodStartSaveName);

    while (!WindowShouldClose()) {
        ((InputService*)(&inputService))->poll((InputService*)&inputService);
        HapticService_update(&hapticService, GetFrameTime());

        BeginDrawing();
        ClearBackground(WHITE);
        RodSystem_updateAndDrawRods(&rodSystem);
        if (inputService.super.isMouseButtonDown(&inputService.super, MOUSE_BUTTON_LEFT)) {
            DrawCircle(inputService.super.getMousePosition(&inputService.super).x, inputService.super.getMousePosition(&inputService.super).y, 3, RED);
        }
        EndDrawing();
    }
    // WriteTapInputServiceCloseSave(&inputService);
    return 0;
}