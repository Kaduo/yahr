#include "raylib.h"
#include "gameobject.h"
#include "stdio.h"
#include "rodfactory.h"
#include "haptic.h"
#include "serialize.h"
#include "network.h"
#include "string.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "pretest.h"

const int TABLET_WIDTH = 1000;
const int TABLET_HEIGHT = 600;

const Color colors[10] = {WHITE, RED, GREEN, DARKPURPLE, YELLOW, DARKGREEN, BLACK, BROWN, BLUE, ORANGE};




int main(int argc, char *argv[]) {
    InputService inputService = NewPhysicalInputService();
    // static const HapticDriver MOCK_HAPTIC_DRIVER = {
    //     .clearSignal = MockHapticDriver_clearSignal,
    //     .setSignal = MockHapticDriver_setSignal
    // };
    // HapticDriver hapticDriver = {
    //     .clearSignal = MockHapticDriver_clearSignal,
    //     .setSignal = MockHapticDriver_setSignal
    // };

    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver();
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);
    StandardRodFactory rodFactory = {.inputService = (InputService*)&inputService, .hapticService = &hapticService, .rodUnitHeight = 30};

    Rods *rods = OnlyHapticStacked(&rodFactory);

    // if (argc > 1) {
    //     printf("%d\n", argc);
    //     if (strcmp(argv[1], "server") == 0) {
    //         printf("I'm the server !!!\n");
    //         Server server = NewServer();
    //         Server_initAndWaitForConnection(&server, "localhost", "55555");
    //         while (true) {
    //             Vec *message = ActiveSocket_readMessage(&server.super);
    //             if (message != NULL) {
    //                 for (int i =0; i < message->size; i++) {
    //                     // printf("%c", ((char*)(message->contents))[i]);
    //                     printf("%c", *((char*)IndexVec(message, i)));
    //                 }
    //                 printf("\n");

    //                 printf("");
    //             }
    //         }
    //     }
    // } else {
        // printf("I'm the client!!!\n");
        // ActiveSocket socket = NewActiveSocket();
        // ActiveSocket_connectToServer(&socket, "localhost", "55555");
        // // unsigned char messageLen[] = {4, 0};
        // uint16_t messageLen = htons(4);
        // write(socket.pfd, &messageLen, 2);
        // sleep(2);
        // write(socket.pfd, "ho", 2);
        // sleep(2);
        // write(socket.pfd, "la", 2);
        // return 0;
    // }
    // Rods rods = EmptyRods(14);
    // for (int i = 0; i < 14; i++) {
    //     AppendRod(&rods, CreateRod(&rodFactory, 3, 0, i*rodFactory.rodUnitHeight, WHITE, signal));
    // }

    // for (int i = 0; i < 10; i++) {
    //     AppendRod(&rods, CreateRod(&rodFactory, 3, 0, i*rodFactory.rodUnitHeight, colors[i], (Signal){.amplitude = 66 + i}));
    // }

    // FILE *file = fopen("my_third_save.rods", "r");
    // Rods *rods  = ReadRods(file);
    // SetRodServices(rods, &inputService, &hapticService);
    // fclose(file);


    float margin = 20;
    int nbZones = 4;
    float zoneWidth = (TABLET_WIDTH - margin*(nbZones + 1))/(nbZones);
    float zoneHeight = (TABLET_HEIGHT/2 - margin*2);


    Vec zones = EmptyVec(4, sizeof(Zone*));
    for (int i =0; i < nbZones; i ++) {
        Zone *zone = NewZone((Rectangle){.x = margin + i*(zoneWidth+margin), .y = TABLET_HEIGHT/2 + margin,  .height = zoneHeight, .width = zoneWidth}, BLACK);
        PushVec(&zones, &zone);
    }

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(TABLET_WIDTH, TABLET_HEIGHT, "Haptic Rods");

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

    // FILE *file = fopen("my_third_save.rods", "w");
    // WriteRods(file, &rods);
    CloseWindow();
    return 0;
}