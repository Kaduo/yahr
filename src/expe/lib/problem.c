#include "problem.h"
#include "math.h"
#include "vec.h"
#include "rodsystem.h"
#include "stdlib.h"
#include "consts.h"
#include "time.h"


Fraction ComputeUnknownLengthProblem(Problem problem) {
    return (Fraction){
        .numerator = problem.knownLength * problem.mysteryRod.n,
        .denominator = problem.knownRod.n
    };
}

int gcd(int a, int b) {
  int c;
  while ( a != 0 ) {
     c = a;
     a = b%a;
     b = c;
  }
  return b;
}

int lcm(int a, int b) {
    return (a * b)/gcd(a, b);
}

Fraction ReduceFraction(Fraction fraction) {
    int g = gcd(fraction.numerator, fraction.denominator);
    return (Fraction){
        .numerator = fraction.numerator/g,
        .denominator = fraction.denominator/g,
    };
}

bool FractionsEqual(Fraction f1, Fraction f2) {
    return f1.numerator * f2.denominator == f2.numerator * f1.denominator;
}

int mydiv(int a, int b) {
    int q =0;
    while (b*q <= a) {
        q++;
    }
    return q - 1;
}

int max(int a, int b) {
    if (a >= b) {
        return a;
    } else {
        return b;
    }
}

int min(int a, int b) {
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

int *GetNecessaryRodsForProblem(Problem problem) {
    int n1 = problem.knownRod.n;
    int n2 = problem.mysteryRod.n;
    int l = lcm(n1, n2);
    int maxn = max(n1, n2);
    int minn = min(n1, n2);
    int q = mydiv(maxn, minn);
    int r = maxn - minn*q;
    int *res = calloc(10, sizeof(int));
    res[n1 - 1] = l / n1;
    res[n2 - 1] = l / n2;
    res[maxn - 1] = max(1, res[maxn- 1]);
    res[minn - 1] = max(res[minn- 1], q);
    if (r != 0) {
        res[r - 1] = 1;

    }
    return res;
}

bool TestSolutionToProblem(Problem problem, Fraction solution) {
    return FractionsEqual(ComputeUnknownLengthProblem(problem), solution);
}

Vec GetRandomRodsForProblem(Problem problem, int maxNbRods) {
    Vec res = EmptyVec(maxNbRods, sizeof(Rod2));
    int *necessaryRods = GetNecessaryRodsForProblem(problem);
    int nbRods = 0;
    for (int i = 0; i < 10; i ++) {
        for (int j = 0; j < necessaryRods[i]; j++) {
            Rod2 rod = GetRod2ForN(i + 1);
            PushVec(&res, &rod);
            nbRods++;
        }
    }

    for (int k = 0; k < maxNbRods - nbRods; k ++) {
        Rod2 rod = GetRod2ForN(GetRandomValue(1, 10));
        PushVec(&res, &rod);
    }

    ShuffleRods2(&res, 0, TABLET_WIDTH, 0, TABLET_HEIGHT);
    free(necessaryRods);
    return res;
}

bool IsProblemInteresting(Problem problem) {
    if (problem.knownRod.n == problem.mysteryRod.n) {
        return false;
    }
    if (abs(problem.knownRod.n - problem.mysteryRod.n) == 1) {
        return false;
    }
    if (problem.knownLength == 1) {
        return false;
    }

    if (problem.knownRod.n == 1 || problem.mysteryRod.n == 1) {
        return false;
    }
    if (problem.knownRod.n % problem.mysteryRod.n == 0 || problem.mysteryRod.n %problem.knownRod.n == 0) {
        return false;
    }

    return true;
}

Problem MakeRandomInterestingProblem() {
    Problem prob;
    do {
        prob.knownRod = GetRod2ForN(GetRandomValue(1, 10));
        prob.mysteryRod = GetRod2ForN(GetRandomValue(1, 10));
        int length = GetRandomValue(1, 10);
        prob.knownLength = length;
    } while (!IsProblemInteresting(prob));

    return prob;
}

void WriteProblem(Problem prob, char *filename) {
    FILE *save = fopen(filename, "w");
    if (save != NULL) {
        fprintf(save, "begin known rod\n");
        WriteRod2(save, prob.knownRod);
        fprintf(save, "end known rod\n");
        fprintf(save, "begin mystery rod\n");
        WriteRod2(save, prob.mysteryRod);
        fprintf(save, "end mystery rod\n");
        fprintf(save,"known length : %d", prob.knownLength);
        fclose(save);

    } else {
        perror(0);
        exit(1);
    }
}

Problem ReadProblem(char *filename) {
    Problem prob = {0};
    FILE *save = fopen(filename, "r");
    if (save != NULL) {
        fscanf(save, "begin known rod\n");
        prob.knownRod = ReadRod2(save);
        fscanf(save, "end known rod\n");
        fscanf(save, "begin mystery rod\n");
        prob.mysteryRod = ReadRod2(save);
        fscanf(save, "end mystery rod\n");
        fscanf(save, "known length : %d", &prob.knownLength);
        fclose(save);

    } else {
        perror(0);
        exit(1);
    }
    return prob;
}