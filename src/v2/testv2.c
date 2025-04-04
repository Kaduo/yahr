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

// TODO :
// 

int main(void) {
    InputService inputService = NewPhysicalInputService();
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver(&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);

    // Vec zones = MakeZones(2, 2, 5, 10, 10, 10, 10, 400, 200);

    InitWindow(1000, 1000, "v2");

    RodSystem rodSystem = NewRodSystem(&inputService, &hapticService);
    RodSystem_loadRods(&rodSystem, "data/pre-test/sessions/colors+haptic+length/session_0.rods");
    // Rod2 rod1 = (Rod2){
    //     .color = RED,
    //     .rect = {10, 10, 90, 30},
    //     .signal = RodSignalForLength(2),
    // };

    // Rod2 rod2 = (Rod2){
    //     .color = BLUE,
    //     .rect = {100, 100, 30, 120},
    //     .signal = RodSignalForLength(3),
    // };
    // Rod2 rod3 = (Rod2){
    //     .color = GREEN,
    //     .rect = {400, 100, 20, 120},
    //     .signal = RodSignalForLength(5),
    // };
    // printf("hi !\n");

    // PushVec(&rodSystem.rods, &rod1);
    // PushVec(&rodSystem.rods, &rod2);
    // PushVec(&rodSystem.rods, &rod3);
    // printf("huh\n");

    while (!WindowShouldClose()) {
        inputService.poll(&inputService);
        HapticService_update(&hapticService, GetFrameTime());

        BeginDrawing();
        ClearBackground(WHITE);
        printf("hihishi\n");
        RodSystem_updateAndDrawRods(&rodSystem);
        EndDrawing();
    }
    return 0;
}