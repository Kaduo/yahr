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
    // InputService innerInputService = NewPhysicalInputService();
    // WriteTapInputService inputService = NewWriteTapInputService(innerInputService, "test_save.tap");
    ReadTapInputService inputService = NewReadTapInputService("/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/expe_results_user_41/problem_2.tap");
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver((InputService*)&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);

    InitWindow(TABLET_WIDTH, TABLET_HEIGHT, "v2");
    SetTargetFPS(60);

    Problem prob = MakeRandomInterestingProblem();
    RodSystem rodSystem = NewRodSystem(&inputService, &hapticService);
    // RodSystem_loadRods(&rodSystem, "data/pre-test/sessions/colors+haptic+length/session_0.rods");
    // rodSystem.rods = GetRandomRodsForProblem(prob, 15);
    RodSystem_loadRods(&rodSystem, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problemes/problem_2.rods");

    while (!WindowShouldClose()) {
        ((InputService*)(&inputService))->poll((InputService*)&inputService);
        HapticService_update(&hapticService, GetFrameTime());

        BeginDrawing();
        ClearBackground(WHITE);
        RodSystem_updateAndDrawRods(&rodSystem);
        DrawCircle(inputService.super.getMousePosition(&inputService.super).x, inputService.super.getMousePosition(&inputService.super).y, 3, RED);
        EndDrawing();
    }
    // WriteTapInputServiceCloseSave(&inputService);
    return 0;
}