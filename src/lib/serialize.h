#pragma once
#include "rodfactory.h"

void ReadStringTillSpace(FILE *file, char *string);


void WriteColor(FILE *file, Color color);
Color ReadColor(FILE *file);

void WriteRect(FILE *file, Rectangle rect);
void WriteRods(FILE *file, const Rods *rods);
Rods *ReadRods(FILE *file);
Rectangle ReadRect(FILE *file);
Signal ReadSignal(FILE *file);
void WriteSignal(FILE *file, Signal signal);
void WriteRod(FILE *file, const Rod *rod);
Rod *ReadRod(FILE *file);