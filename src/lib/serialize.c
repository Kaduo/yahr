#include "serialize.h"
#include "inttypes.h"
#include "string.h"
#include "stdlib.h"

const int MAXCN = 20;

void ReadStringTillSpace(FILE *file, char *string) {
    int currChar = 0;
    int i = 0;

    while (i + 1 < MAXCN && ((currChar = fgetc(file)) != ' ') && currChar != EOF && currChar != '\n') {
        string[i++] = currChar;
    }
    string[i] = 0;
}

void WriteSignal(FILE *file, Signal signal) {
    fprintf(file, "sig\n");
    fprintf(file, "amplitude %" SCNu8 "\n", signal.amplitude);
    fprintf(file, "angle %" SCNu8 "\n", signal.angle);
    fprintf(file, "duration %f\n", signal.duration);
    fprintf(file, "duty %" SCNu8 "\n", signal.duty);
    fprintf(file, "offset %" SCNu8 "\n", signal.offset);
    fprintf(file, "period %" SCNu16 "\n", signal.period);
    fprintf(file, "phase %" SCNu16 "\n", signal.phase);
    fprintf(file, "pulses %" SCNu8 "\n", signal.pulses);
    fprintf(file, "signalShape %" SCNu8 "\n", signal.signalShape);
    fprintf(file, "endsig\n");
}

Signal ReadSignal(FILE *file) {
    Signal signal = {0};
    char string[MAXCN];
    ReadStringTillSpace(file, string);
    while (strlen(string) > 0) {

        if (strcmp(string, "sig") == 0) {
        } else if (strcmp(string, "amplitude") == 0) {
            fscanf(file, "%" SCNu8"\n", &signal.amplitude);
        } else if (strcmp(string, "angle") == 0) {
            fscanf(file, "%" SCNu8"\n", &signal.angle);
        } else if (strcmp(string, "duration") == 0) {
            fscanf(file, "%f\n", &signal.duration);
        } else if (strcmp(string, "duty") == 0) {
            fscanf(file, "%" SCNu8"\n", &signal.duty);
        } else if (strcmp(string, "offset") == 0) {
            fscanf(file, "%" SCNu8"\n", &signal.offset);
        } else if (strcmp(string, "period") == 0) {
            fscanf(file, "%" SCNu16"\n", &signal.period);
        } else if (strcmp(string, "phase") == 0) {
            fscanf(file, "%" SCNu16"\n", &signal.phase);
        } else if (strcmp(string, "pulses") == 0) {
            fscanf(file, "%" SCNu8"\n", &signal.pulses);
        } else if (strcmp(string, "signalShape") == 0) {
            fscanf(file, "%" SCNu8"\n", (uint8_t*)&signal.signalShape);
        } else if (strcmp(string, "endsig") == 0) {
            break;
        } else {
            printf("SHOULDNH4T HAPPEN!!!\n");
            abort();
        }
        ReadStringTillSpace(file, string);
    }
    return signal;
}


void WriteColor(FILE *file, Color color) {
    fprintf(file, "color %" SCNu8 " %" SCNu8 " %" SCNu8 " %" SCNu8 " endcolor\n", 
        color.r,
        color.g,
        color.b,
        color.a);
}

Color ReadColor(FILE *file) {
    Color color = {0};
    fscanf(file, "color %" SCNu8 " %" SCNu8 " %" SCNu8 " %" SCNu8 " endcolor\n", 
        &color.r,
        &color.g,
        &color.b,
        &color.a);
    return color;
}

void WriteRect(FILE *file, Rectangle rect) {
    fprintf(file, "rect %f %f %f %f endrect\n",
    rect.x,
    rect.y,
    rect.height,
    rect.width);
}

Rectangle ReadRect(FILE *file) {
    Rectangle rect = {0};
    fscanf(file, "rect %f %f %f %f endrect\n",
        &rect.x,
        &rect.y,
        &rect.height,
        &rect.width);
    return rect;
}

void WriteRod(FILE *file, const Rod *rod) {
    fprintf(file, "rod\n");
    WriteRect(file, GetRodRectangle(rod));
    WriteColor(file, ((RodGraphicsComponent*)(rod->graphicsComponent))->color);
    WriteSignal(file, rod->rodHapticComponent->signal);
    fprintf(file, "endrod\n");
}

Rod *ReadRod(FILE *file) {
    fscanf(file, "rod\n");
    Rectangle rect = ReadRect(file);
    Color color = ReadColor(file);
    Signal signal = ReadSignal(file);
    fscanf(file, "endrod\n");
    return NewRod(rect, color, NULL, NULL, NULL, signal);
}

void WriteRods(FILE *file, const Rods *rods) {
    fprintf(file, "nbRods %d\n", rods->rods.size);
    for (int i = 0; i < rods->rods.size; i++) {
        WriteRod(file, *(Rod**)IndexVec(&rods->rods, i));
    }
}

Rods *ReadRods(FILE *file) {
    int nbRods;
    fscanf(file, "nbRods %d\n", &nbRods);
    Rods *rods = malloc(sizeof(Rods));
    rods->rods = EmptyVec(nbRods, sizeof(Rod*));
    for (int i = 0; i<nbRods; i++) {
        Rod *rod = ReadRod(file);
        AppendRod(rods, rod);
    }
    return rods;
}

// void WriteHashMap(FILE *file, const HashMap *map) {
//     fprintf(file, "map\n");
//     for (int i = 0; i < map->keys.size; i++) {
//         fprintf(file, "key ");
//         fprintf(file, *(char**)IndexVec(&map->keys,i));
//         fprintf(file, "value ");
//         fwrite(IndexVec(&map->items, i), 1, );
//         fprintf(file, "\n");
//     }
// }