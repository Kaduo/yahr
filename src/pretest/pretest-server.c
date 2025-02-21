#include "raylib.h"
#include "network.h"
#include "stdlib.h"
#include "string.h"
#include "vec.h"
#include "stdio.h"
#include "session.h"
#include "gameobject.h"
#include "consts.h"

typedef enum ServerState {
    SERVER_WAITING_FOR_NEXT,
    SERVER_WAITING_FOR_RODS,
    SERVER_WAITING_FOR_CONNECTION,
    SERVER_WAITING_FOR_INSTRUCTION,
} ServerState;

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


void DrawZones(Vec zones) {
    for (int i = 0; i < zones.size; i++) {
        Zone *zone = *(Zone**)IndexVec(&zones,i);
        zone->graphicsComponent->draw(zone->graphicsComponent);
    }
}

int main(void) {
    Server server = NewServer();

    ServerState serverState = SERVER_WAITING_FOR_CONNECTION;
    Server_initAndWaitForConnection(&server, "localhost", TABLET_PORT);
    serverState = SERVER_WAITING_FOR_INSTRUCTION;
    
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "layout_name");

    InputService inputService = NewPhysicalInputService();
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver(&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);
    SessionList sessionList = NewSessionList("data/pre-test/sessions/colors+haptic+length/", "data/pre-test/results/colors+haptic", 0, 10, &inputService, &hapticService);
    SessionList_loadNextSession(&sessionList);
    
    // ToggleFullscreen(); // TODO : uncomment me ?

    Vec zones = MakeZones(4, 10, 10, 10, 10);

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
            if (strcmp(messageStr, "CLOSE") == 0) {
                printf("bye!\n");
                CloseWindow();
            }
            
            else if (strcmp(messageStr, "NEXT?") == 0) {
                printf("hey !\n");
                SessionList_saveCurrentSession(&sessionList);
                if(!SessionList_loadNextSession(&sessionList)) {
                    printf("bye!!\n");
                    CloseWindow();
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
        BeginDrawing();
            ClearBackground(WHITE); 
            DrawZones(zones);
            if (sessionList.session.rods != NULL) {
                DrawRods(sessionList.session.rods);
            }

        EndDrawing();
    }
    Server_closeConnection(&server);
    return 0;
}