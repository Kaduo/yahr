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
    GuiStyle(DEFAULT, LINE_COLOR, BLACK);
    // GuiSetFont(LoadFont("src/pretest/arial.ttf"));
    // ToggleFullscreen();


    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ENTER)) {
            ActiveSocket_writeStr(&socket, "NEXT?");
        }

        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            float labelWidth = 1000;
            float labelHeight = 30;
            GuiLabel((Rectangle){ GetScreenWidth()/2 - labelWidth/2., GetScreenHeight()/2., labelWidth, labelHeight },
            "Triez les réglettes par similarité.\n\n\n Chaque réglette doit être placée dans une seule zone. \n\n\n Il y a au moins une réglette par zone. \n\n\n Appuyez sur la touche entrée quand vous êtes satisfait.");
        EndDrawing();
    }

    ActiveSocket_closeConnection(&socket);
    return 0;
}