
#include "tap.h"
#include "stdbool.h"
#include "serialize.h"
#include "string.h"
#include "stdlib.h"

const int MAXCNTAP = 20;

void WriteMousePosition(FILE *file, Vector2 mousePosition) {
    fprintf(file, "mousePosition %f %f endmousePosition\n", mousePosition.x, mousePosition.y);
}

Vector2 ReadMousePosition(FILE *file) {
    Vector2 mousePosition = {0};
    fscanf(file, "%f %f endmousePosition\n", &mousePosition.x, &mousePosition.y);
    return mousePosition;
}

void WriteFrameTime(FILE *file, float frameTime) {
    fprintf(file, "time %f endtime\n", frameTime);
}

float ReadFrameTime(FILE *file) {
    float frameTime;
    fscanf(file, "%f endtime\n", &frameTime);
    return frameTime;
}

void WriteLeftMouseButttonStateFlipped(FILE *file) {
    fprintf(file, "leftMouseButtonFlipped\n");
}

void WriteTap(FILE *file, Tap tap) {
    // printf("hola\n");
    WriteFrameTime(file, tap.frameTime);
    if (tap.flipped) {
        WriteLeftMouseButttonStateFlipped(file);
    }
    WriteMousePosition(file, tap.mousePosition);
}

Tap ReadTap(FILE *file) {
    Tap tap = {0};
    char string[MAXCNTAP];
    ReadStringTillSpace(file, string);
    while (strlen(string) > 0) {

        if (strcmp(string, "time") == 0) {
            tap.frameTime = ReadFrameTime(file);
        } else if (strcmp(string, "leftMouseButtonFlipped")) {
            tap.flipped = true;
        } else if (strcmp(string, "mousePosition")) {
            tap.mousePosition = ReadMousePosition(file);
            break;
        } else {
            printf("SHOULDN'T HAPPEN !!! UNKNOWN stuff while reading tap : %s\n", string);
            abort();
        }
        ReadStringTillSpace(file, string);
    }
    return tap;
}
