#pragma once
#include "rodfactory.h"

void ReadStringTillSpace(FILE *file, char *string);

void WriteRods(FILE *file, const Rods *rods);
Rods *ReadRods(FILE *file);
