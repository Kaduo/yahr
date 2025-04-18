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
    sprintf(path, "data/experiment/problems/problem_%d.statement", problemId);
    return ReadProblem(path);
}

int main(int argc, char *argv[]) {

    // if (argc <= 1) {
    //     fprintf(stderr, "Please enter the port number of the server you are trying to connect to!\n");
    // }

    if (argc <= 1) {
        fprintf(stderr, "Please enter the name of the participant");
        exit(1);
    }

    ActiveSocket socket = NewActiveSocket();
    ActiveSocket_connectToServer(&socket, TABLET_IP, TABLET_PORT, SOCK_NONBLOCK);
    ActiveSocket_writeStr(&socket, argv[1]);

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Client");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    GuiSetStyle(DEFAULT, LINE_COLOR, 20);
    ToggleFullscreen();

    int problemId = 0;
    Problem problem;
    char *statement;

    char saveFolderPath[100] = {0};
    sprintf(saveFolderPath, "data/experiment/results/%s", argv[1]);
    MakeDirectory(saveFolderPath);
    printf("here??\n");
    char answersSaveName[200];
    sprintf(answersSaveName, "data/experiment/results/%s/%s.answers", argv[1], argv[1]);
    printf("save name : %s\n", answersSaveName);
    FILE *answers = fopen(answersSaveName, "w");

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
                statement = DisplayProblem(problem);

            }
        }

        // if (IsKeyPressed(KEY_ENTER)) {
        //     if (FractionsEqual(ParseFractionInput(fracInput), (Fraction){2, 3})) {
        //         printf("youpi:\n");
        //     } else {
        //         printf("non!\n");
        //     }
        // }

        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            if (problemId >= 5 && problemId < 15) {
                GuiLabel((Rectangle){200, 400, 5000, 50}, statement);
                float labelWidth = 1000;
                float labelHeight = 30;
                GuiTextBox((Rectangle){300, 500, 500, 50}, fracInput, 50, true);
    
            } else {
                float labelWidth = 1000;
                float labelHeight = 30;
                GuiLabel((Rectangle){ GetScreenWidth()/2 - labelWidth/2., GetScreenHeight()/2., labelWidth, labelHeight },
                "Triez les réglettes par similarité.\n\n\n Chaque réglette doit être placée dans une seule zone. \n\n\n Il y a au moins une réglette par zone. \n\n\n Appuyez sur la touche entrée quand vous êtes satisfait.");    
            }
            // GuiTextBox((Rectangle){20, 20, 50, 50}, numerator, 50, true);



        EndDrawing();
    }
    fclose(answers);
    // ActiveSocket_writeStr(&socket, "CLOSE");
    ActiveSocket_closeConnection(&socket);
    return 0;
}