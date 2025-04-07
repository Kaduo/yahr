#pragma once
#include "rodsystem.h"

// Alors, un problème il faut qu'il ait : deux types de réglettes, une longueur donnée, et une longueur à trouver.
// Donc ça implique que les réglettes aient des longueurs ? Ouais, ça paraît plus malin, donc je fais ça. Ok...

typedef struct Fraction {
    int numerator;
    int denominator;
} Fraction;

typedef struct Problem {
    Rod2 knownRod;
    Rod2 mysteryRod;
    int knownLength;
} Problem;

Vec GetRandomRodsForProblem(Problem problem, int maxNbRods);
Problem MakeRandomInterestingProblem();
bool FractionsEqual(Fraction f1, Fraction f2);
void WriteProblem(Problem prob, char *filename);
Problem ReadProblem(char *filename);