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

int GetCurrentUserId() {
    int id;
    FILE *latestUserIdFile = fopen("latest_user_id_expe", "r");
    if (latestUserIdFile != NULL) {
        fscanf(latestUserIdFile, "%d", &id);
        fclose(latestUserIdFile);
    } else {
        perror(0);
        exit(1);
    }
    return id;
}


char *GetUserDirPath(int userId) {
    char *path = calloc(50, sizeof(char));
    sprintf(path, "expe_results_user_%d", userId);
    return path;
}

char *GetCurrentUserDirPath() {
    return GetUserDirPath(GetCurrentUserId());
}

int MakeNewUser() {
    int id = GetCurrentUserId();
    FILE *latestUserIdFile = fopen("latest_user_id_expe", "w");
    if (latestUserIdFile != NULL) {
        fprintf(latestUserIdFile, "%d", (id+1));
        fclose(latestUserIdFile);
    } else {
        perror(0);
        exit(1);
    }

    char *nextUserDirPath = GetUserDirPath(id+1);
    MakeDirectory(nextUserDirPath);
    free(nextUserDirPath);
    return id + 1;
}


// TODO MOVE TO client
// void SaveCurrentUserSolution(int problemId, char *solution) {
//     char path[50] = {0};
//     sprintf(path, "%s/%s%d%s", GetCurrentUserId(), "solution_", problemId, ".sol");
//     FILE *save = fopen(path, "w");
//     if (save != NULL) {
//         fprintf(save, solution);
//         fclose(save);
//     } else {
//         perror(0);
//         exit(1);
//     }
// }

void DrawZones() {
    DrawLine(0, TABLET_HEIGHT/2, TABLET_WIDTH, TABLET_HEIGHT/2, BLACK);
    DrawLine(0, 3*TABLET_HEIGHT/4, TABLET_WIDTH, 3*TABLET_HEIGHT/4, BLACK);
    DrawLine(TABLET_WIDTH/2, TABLET_HEIGHT/2, TABLET_WIDTH/2, TABLET_HEIGHT, BLACK);
}

void SaveCurrentUser(int problemId, RodSystem *rodSystem, WriteTapInputService *inputService) {
    char path1[50] = {0};
    sprintf(path1, "%s/%s%d%s", GetCurrentUserDirPath(), "problem_", problemId, ".rods");
    SaveRodSystem(rodSystem, path1);
    WriteTapInputServiceCloseSave(inputService);
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
    
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "layout_name");
    SetTargetFPS(60);
    ToggleFullscreen();

    int problemId = 0;
    int userId = MakeNewUser();

    char path[50] = {0};
    sprintf(path, "%s/%s%d%s", GetCurrentUserDirPath(), "problem_", problemId, ".tap");

    WriteTapInputServiceOpenSave(&inputService, path);

    char path2[50] = {0};

    sprintf(path2, "data/experiment/problemes/problem_%d.rods", problemId);

    RodSystem_loadRods(&rodSystem, path2);

    while (!WindowShouldClose()) {

        Vec *message = ActiveSocket_readMessage((ActiveSocket*)&server);
        if (message != NULL) {
            char *messageStr = (char*)message->contents;

            if (strcmp(messageStr, "NEXT") == 0) {
                SaveCurrentUser(problemId, &rodSystem, &inputService);
                if (problemId < 20) {
                    problemId += 1;
                    char path[50] = {0};
                    sprintf(path, "%s/%s%d%s", GetCurrentUserDirPath(), "problem_", problemId, ".tap");
                    WriteTapInputServiceOpenSave(&inputService, path);
                    char path2[50] = {0};

                    sprintf(path2, "data/experiment/problemes/problem_%d.rods", problemId);

                    RodSystem_loadRods(&rodSystem, path2);
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
    WriteTapInputServiceCloseSave(&inputService);
    return 0;
}
