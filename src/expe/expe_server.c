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
#include "problem.h"
#include "rodsystem.h"

void DrawZones() {
    DrawLine(0, TABLET_HEIGHT/2, TABLET_WIDTH, TABLET_HEIGHT/2, BLACK);
    DrawLine(0, 3*TABLET_HEIGHT/4, TABLET_WIDTH, 3*TABLET_HEIGHT/4, BLACK);
    DrawLine(TABLET_WIDTH/2, TABLET_HEIGHT/2, TABLET_WIDTH/2, TABLET_HEIGHT, BLACK);
}

// typedef struct App {
//     RodSystem rodSystem;
//     int tableauId;
// } App;

int main() {
    InputService innerInputService = NewPhysicalInputService();
    WriteTapInputService inputService = NewWriteTapInputService(innerInputService, "blank_save.tap");
    // ReadTapInputService inputService = NewReadTapInputService("test_save.tap");
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver((InputService*)&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);


    RodSystem rodSystem = NewRodSystem(&inputService, &hapticService);
    Server server = NewServer();

    Server_initAndWaitForConnection(&server, TABLET_IP, TABLET_PORT);

    char userName[100];

    while (true) {
        Vec *message = ActiveSocket_readMessage((ActiveSocket*)&server);
        if (message != NULL) {
            strcpy(userName, (char*)message->contents);
            break;
        }
    }

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "layout_name");
    SetTargetFPS(60);
    ToggleFullscreen();

    int problemId = 0;
    // int userId = MakeNewUser();
    char saveFolderPath[100] = {0};
    sprintf(saveFolderPath, "data/experiment/results/%s", userName);
    MakeDirectory(saveFolderPath);
    char problemSavePath[100] = {0};
    sprintf(problemSavePath, "data/experiment/results/%s/problem_%d.tap", userName, problemId);

    WriteTapInputServiceOpenSave(&inputService, problemSavePath);

    char path2[50] = {0};

    sprintf(path2, "data/experiment/problems/problem_%d.rods", problemId);

    RodSystem_loadRods(&rodSystem, path2);

    while (!WindowShouldClose()) {

        Vec *message = ActiveSocket_readMessage((ActiveSocket*)&server);
        if (message != NULL) {
            char *messageStr = (char*)message->contents;

            if (strcmp(messageStr, "NEXT") == 0) {
                printf("current username : %s\n",userName);
                char path1[100];
                sprintf(path1, "data/experiment/results/%s/problem_%d_end.rods", userName, problemId);
                SaveRodSystem(&rodSystem, path1);
                WriteTapInputServiceCloseSave(&inputService);
            
                if (problemId < 19) {
                    problemId += 1;
                    char path[100] = {0};
                    sprintf(path, "data/experiment/results/%s/problem_%d.tap", userName, problemId);
                    WriteTapInputServiceOpenSave(&inputService, path);
                    char path2[100] = {0};

                    sprintf(path2, "data/experiment/problems/problem_%d.rods", problemId);
                    RodSystem_loadRods(&rodSystem, path2);

                    char path3[50] = {0};
                    sprintf(path3, "data/experiment/results/%s/problem_%d_start.rods", userName, problemId);
                    SaveRodSystem(&rodSystem, path3);
                } else {
                    break;
                }
            }

        }
        ((InputService*)(&inputService))->poll((InputService*)&inputService);
        HapticService_update(&hapticService, GetFrameTime());

        BeginDrawing();
        ClearBackground(WHITE);

        if (problemId < 5 || problemId >= 15) {
            DrawZones();
        }
        RodSystem_updateAndDrawRods(&rodSystem);
        EndDrawing();
    }

    return 0;
}
