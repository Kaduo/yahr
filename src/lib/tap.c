
#include "tap.h"
#include "stdbool.h"
#include "serialize.h"
#include "string.h"
#include "stdlib.h"

const int MAXCNTAP = 40;

void WriteMousePosition(FILE *file, Vector2 mousePosition) {
    fprintf(file, "mousePosition %f %f endmousePosition\n", mousePosition.x, mousePosition.y);
}

Vector2 ReadMousePosition(FILE *file) {
    Vector2 mousePosition = {0};
    fscanf(file, "%f %f endmousePosition\n", &mousePosition.x, &mousePosition.y);
    return mousePosition;
}

void WriteTime(FILE *file, float time) {
    fprintf(file, "time %f endtime\n", time);
}

float ReadTime(FILE *file) {
    float time;
    fscanf(file, "%f endtime\n", &time);
    return time;
}

void WriteLeftMouseButttonStateFlipped(FILE *file) {
    fprintf(file, "leftMouseButtonFlipped\n");
}

void WriteTap(FILE *file, Tap tap) {
    // printf("hola\n");
    WriteTime(file, tap.time);
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
            tap.time = ReadTime(file);
        } else if (strcmp(string, "leftMouseButtonFlipped") == 0) {
            tap.flipped = true;
        } else if (strcmp(string, "mousePosition") == 0) {
            tap.mousePosition = ReadMousePosition(file);
            break;
        } else {
            printf("unknown stuff in tap : %s\n", string);
            abort();
        }
        ReadStringTillSpace(file, string);
    }
    return tap;
}
