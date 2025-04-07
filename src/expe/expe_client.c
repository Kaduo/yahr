#include "raylib.h"
#include "raygui.h"
#include "network.h"
#include "consts.h"
#include "unistd.h"
#include "libinetsocket.h"
#include "arpa/inet.h"
#include "problem.h"
#include "stdlib.h"

char *DisplayProblem(Problem prob) {
    char *res = calloc(200, sizeof(char));
    sprintf(res, "Si la réglette de couleur %s mesure %d unités,\n \n combien d'unités mesure la réglette de couleur %s ?",
        GetColorName(prob.knownRod.color), prob.knownLength, GetColorName(prob.mysteryRod.color));

    return res;
}

Fraction ParseFractionInput(char *input) {
    char numeratorString[10] = {0};
    int numi = 0;
    char denominatorString[10] = {0};
    int denomi = 0;
    bool readingNumerator = true;
    for (int i = 0; input[i] != 0; i++) {
        if (readingNumerator)  {
            if (input[i] == '/') {
                readingNumerator = false;
            } else {
                numeratorString[numi] = input[i];
                numi ++;
            }
        } else {
            denominatorString[denomi] = input[i];
            denomi++;
        }
        
    }

    Fraction frac = {0};
    sscanf(numeratorString, "%d", &frac.numerator);
    sscanf(denominatorString, "%d", &frac.denominator);
    return frac;
}


Problem GetProblemById(int problemId) {
    char path[200];
    sprintf(path, "data/experiment/problemes/problem_%d.statement", problemId);
    return ReadProblem(path);
}

int main(int argc, char *argv[]) {

    // if (argc <= 1) {
    //     fprintf(stderr, "Please enter the port number of the server you are trying to connect to!\n");
    // }


    ActiveSocket socket = NewActiveSocket();
    ActiveSocket_connectToServer(&socket, TABLET_IP, TABLET_PORT, SOCK_NONBLOCK); // TODO : UNCOMMENT ME !!!!

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(1000, 1000, "Client");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    GuiSetStyle(DEFAULT, LINE_COLOR, 20);
    ToggleFullscreen();

    int problemId = 0;
    Problem problem;
    char *statement;

    FILE *answers = fopen("hugo.answers", "w");

    if (answers == NULL) {
        perror(0);
        exit(1);
    }


    char fracInput[50] = {0};
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ENTER)) {
            ActiveSocket_writeStr(&socket, "NEXT");
            if (problemId >= 5 && problemId < 15) {
                fprintf(answers, "problem %d : %s\n", problemId, fracInput);
                fflush(answers);
                *fracInput = 0;
            }
            if (problemId < 20) {
                problemId +=1;
            } else {
                break;
            }
            if (problemId < 15 && problemId >= 5) {
                problem = GetProblemById(problemId);
                char *statement = DisplayProblem(problem);

            }
        }

        // if (IsKeyPressed(KEY_ENTER)) {
        //     if (FractionsEqual(ParseFractionInput(fracInput), (Fraction){2, 3})) {
        //         printf("youpi:\n");
        //     } else {
        //         printf("non!\n");
        //     }
        // }

        printf("ok so far\n");
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            if (problemId >= 5 && problemId < 15) {
                GuiLabel((Rectangle){0, 0, 10000, 1000}, statement);
                float labelWidth = 1000;
                float labelHeight = 30;
                GuiTextBox((Rectangle){130, 20, 500, 50}, fracInput, 50, true);
    
            } else {
                float labelWidth = 1000;
                float labelHeight = 30;
                GuiLabel((Rectangle){ GetScreenWidth()/2 - labelWidth/2., GetScreenHeight()/2., labelWidth, labelHeight },
                "Triez les réglettes par similarité.\n\n\n Chaque réglette doit être placée dans une seule zone. \n\n\n Il y a au moins une réglette par zone. \n\n\n Appuyez sur la touche entrée quand vous êtes satisfait.");    
            }
            // GuiTextBox((Rectangle){20, 20, 50, 50}, numerator, 50, true);



        EndDrawing();
        printf("HERE?\n");
    }
    fclose(answers);
    // ActiveSocket_writeStr(&socket, "CLOSE");
    ActiveSocket_closeConnection(&socket);
    return 0;
}