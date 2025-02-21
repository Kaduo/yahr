#include "raylib.h"
#include "raygui.h"
#include "network.h"
#include "consts.h"
#include "unistd.h"
#include "libinetsocket.h"
#include "arpa/inet.h"

int main(int argc, char *argv[]) {

    // if (argc <= 1) {
    //     fprintf(stderr, "Please enter the port number of the server you are trying to connect to!\n");
    // }


    ActiveSocket socket = NewActiveSocket();
    ActiveSocket_connectToServer(&socket, TABLET_IP, TABLET_PORT, SOCK_NONBLOCK);


    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "layout_name");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    // ToggleFullscreen();


    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ENTER)) {
            ActiveSocket_writeStr(&socket, "NEXT?");
            printf("huh!\n");
        }

        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            GuiLabel((Rectangle){ GetScreenHeight()/2, GetScreenWidth()/2, 1000000, 100 },
            "Triez les réglettes par similarité.");
        EndDrawing();
    }

    ActiveSocket_closeConnection(&socket);
    return 0;
}