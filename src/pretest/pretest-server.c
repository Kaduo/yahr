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

typedef enum ServerState {
    SERVER_WAITING_FOR_NEXT,
    SERVER_WAITING_FOR_RODS,
    SERVER_WAITING_FOR_CONNECTION,
    SERVER_WAITING_FOR_INSTRUCTION,
} ServerState;

Vec MakeZones(
                int nbZonesPerRow,
                int nbRows,
                float intraRowsYMargin,
                float intraZoneXMargin,
                float rodZoneYMargin,
                float borderZoneXMargin,
                float borderZoneYMargin,
                float zoneWidth,
                float zoneHeight
            ) {
    Vec zones = EmptyVec(nbZonesPerRow, sizeof(Zone*));
    for (int j = 0; j < nbRows; j ++) {
        for (int i =0; i < nbZonesPerRow; i ++) {
            Zone *zone = NewZone((Rectangle){.x = borderZoneXMargin + i*(zoneWidth+intraZoneXMargin) + j*intraRowsYMargin, .y = (nbRows - j)*(zoneHeight + intraRowsYMargin) + rodZoneYMargin,  .height = zoneHeight, .width = zoneWidth}, BLACK);
            PushVec(&zones, &zone);
        }
    }

    return zones;
}


void DrawZones(Vec zones) {
    for (int i = 0; i < zones.size; i++) {
        Zone *zone = *(Zone**)IndexVec(&zones,i);
        zone->graphicsComponent->draw(zone->graphicsComponent);
    }
}

int main(void) {
    Server server = NewServer();

    ServerState serverState = SERVER_WAITING_FOR_CONNECTION;
    Server_initAndWaitForConnection(&server, TABLET_IP, TABLET_PORT); // TODO !!! UNCOMMENT ME !!
    // Server_initAndWaitForConnection(&server, "localhost", TABLET_PORT); // TODO : COMMENT ME, ONLY FOR DEBUG
    serverState = SERVER_WAITING_FOR_INSTRUCTION;
    
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "layout_name");

    bool waitingForNextUser = false;

    double startTime = 0;

    InputService inputService = NewPhysicalInputService();
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver(&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);
    // SessionList sessionList = NewSessionList("data/pre-test/sessions/colors+haptic+length/", "data/pre-test/results/colors+haptic/", 0, 10, &inputService, &hapticService);
    int sessionListId = 0;
    char *sessionFolderPaths[] = {
                                    "data/pre-test/sessions/haptic/",
                                    "data/pre-test/sessions/colors+length/",
                                    // "data/pre-test/sessions/colors+haptic+length/",
                                };
    char *saveFolderPaths[] = {
                                    "data/pre-test/results/haptic/",
                                    "data/pre-test/results/colors+length/",
                                    // "data/pre-test/results/colors+haptic+length/",
                                };
    int nbSessionLists = 2;

    int userId;

    int flipped = 0;
    FILE *latestUserFile = fopen("data/pre-test/latest_user_id", "r");
    if (latestUserFile != NULL) {
        fscanf(latestUserFile, "%d", &userId);
        fclose(latestUserFile);
    } else {
        perror(0);
        exit(1);
    }

    char durationFileName[100];
    sprintf(durationFileName, "pre-test-durations-from-user-%d", userId);

    FILE *durations = fopen(durationFileName, "w");

    if (durations == NULL) {
        perror(0);
        exit(1);
    } else {
        fprintf(durations, "user_%d\n", userId);
    }

    int nbSessionPerList = 10;
    SessionList sessionList = NewSessionList(sessionFolderPaths[sessionListId], saveFolderPaths[sessionListId], userId, nbSessionPerList, &inputService, &hapticService);

    SessionList_loadNextSession(&sessionList);
    
    ToggleFullscreen();

    // Vec zones = MakeZones(2, 2, 5, 10, 10, 10, 10, 400, 200);

    while (!WindowShouldClose()) {
        Vec *message = ActiveSocket_readMessage((ActiveSocket*)&server);
        if (message != NULL) {
            char *messageStr = (char*)message->contents;

            // switch (serverState) {
            //     case SERVER_WAITING_FOR_INSTRUCTION:
            //         if (strcmp(messageStr, "CLOSE") == 0) {
            //             printf("bye!\n");
            //             CloseWindow();
            //         } else if (strcmp(messageStr, "GET READY FOR RODS") == 0) {
            //             serverState = SERVER_WAITING_FOR_RODS;
            //             break;
            //         }

            //         break;
            //     case SERVER_WAITING_FOR_RODS:
            //         size_t bufferSize = 0;
            //         FILE *rodStream = open_memstream(&messageStr, &bufferSize);
            //         fclose(rodStream);
            //     default:
            //         break;
            // };

            if (waitingForNextUser) {
                if (strcmp(messageStr, "GO_NEXT_USER") == 0) {
                    waitingForNextUser = false;
                    startTime = GetTime();
                } 
                else if (strcmp(messageStr, "CLOSE") == 0) {
                    printf("bye!\n");
                    break;
                }
                else {
                    continue;
                }
            }
            else if (strcmp(messageStr, "CLOSE") == 0) {
                printf("bye!\n");
                CloseWindow();
            }
            
            else if (strcmp(messageStr, "NEXT?") == 0) {
                printf("next received!\n");
                SessionList_saveCurrentSession(&sessionList);
                double duration = GetTime() - startTime;
                startTime = GetTime();
                fprintf(durations, "%f\n", duration);
                if(!SessionList_loadNextSession(&sessionList)) {
                    printf("next user or session \n");
                    if (sessionListId < nbSessionLists - 1) {
                        printf("next session !\n");
                        sessionListId += 1;
                        FreeRods(sessionList.session.rods);
                        sessionList = NewSessionList(sessionFolderPaths[(sessionListId + flipped) % 2], saveFolderPaths[(sessionListId + flipped) % 2], userId, nbSessionPerList, &inputService, &hapticService);
                    }
                    else {
                        printf("next user !\n");
                        flipped = 1 - flipped;
                        waitingForNextUser = true;
                        sessionListId = 0;
                        userId += 1;
                        fprintf(durations, "\n\nuser_%d\n", userId);
                        FILE *latestUserFile = fopen("data/pre-test/latest_user_id", "w");
                        if (latestUserFile != NULL) {
                            fprintf(latestUserFile, "%d", userId);
                            fclose(latestUserFile);
                        } else {
                            printf("HEREEE ?!");
                            perror(0);
                            exit(1);
                        }
                        FreeRods(sessionList.session.rods);
                        sessionList = NewSessionList(sessionFolderPaths[(sessionListId + flipped) % 2], saveFolderPaths[(sessionListId + flipped) % 2], userId, nbSessionPerList, &inputService, &hapticService);
                    }
                    SessionList_loadNextSession(&sessionList);
                }
            }

            // else if (strcmp(messageStr, ""))
            
            else {
                fprintf(stderr, "Error, received unknown message : %s\n", messageStr);
                exit(1);
            }
        }

        if (sessionList.session.rods != NULL) {
            UpdateRods(sessionList.session.rods);
        }
        HapticService_update(&hapticService, GetFrameTime());
        BeginDrawing();
            ClearBackground(WHITE); 
            if (waitingForNextUser) {
                DrawText("Merci d'avoir participé !", TABLET_WIDTH/2, TABLET_HEIGHT/2, 20, BLACK);
            } else {
                DrawLine(0, TABLET_HEIGHT/2, TABLET_WIDTH, TABLET_HEIGHT/2, BLACK);
                DrawLine(0, 3*TABLET_HEIGHT/4, TABLET_WIDTH, 3*TABLET_HEIGHT/4, BLACK);
                DrawLine(TABLET_WIDTH/2, TABLET_HEIGHT/2, TABLET_WIDTH/2, TABLET_HEIGHT, BLACK);
                // DrawZones(zones);
                if (sessionList.session.rods != NULL) {
                    DrawRods(sessionList.session.rods);
                }
    
            }

        EndDrawing();
    }
    fclose(durations);
    Server_closeConnection(&server);
    CloseWindow();
    return 0;
}