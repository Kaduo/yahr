#include "problem.h"
#include "rodsystem.h"
#include "vec.h"
#include "consts.h"
#include "raylib.h"

void MakeAndSaveProblem(int knownLength, int knownRod, int mysteryRod, int *nbRodsPerLengths, int problemId) {
    Problem prob = (Problem){
        .knownLength = knownLength,
        .knownRod = knownRod,
        .mysteryRod = mysteryRod,
    };

    Vec rods = EmptyVec(20, sizeof(Rod2));

    for (int i = 0; i < 10; i ++) {
        Rod2 rod = GetRod2ForN(i + 1);
        for (int j = 0; j < nbRodsPerLengths[i]; j++) {
            PushVec(&rods, &rod);
        }
    }
    char path[200] = {0};
    sprintf(path, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problems/problem_%d.rods", problemId);
    WriteRods2(&rods, path);
    char path2[200] = {0};
    sprintf(path2, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problems/problem_%d.statement", problemId);
    WriteProblem(prob, path2);

}

Vec MakePretestRods() {
    Vec rods = EmptyVec(14, sizeof(Rod2));
    for (int i = 0; i < 10; i++) {
        Rod2 rod = GetRod2ForN(i+1);
        PushVec(&rods, &rod);
    }

    for (int i = 0; i < 4; i++) {
        Rod2 rod = GetRod2ForN(GetRandomValue(1, 10));
        PushVec(&rods, &rod);
    }
    ShuffleRods2(&rods, 0, TABLET_WIDTH, 0, TABLET_HEIGHT/2.);
    return rods;
}

void GenerateExpeProblems() {
    for (int i = 0; i < 5; i ++) {
        Vec rods = MakePretestRods();
        char path[200] = {0};
        sprintf(path, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problemes/problem_%d.rods", i);
        WriteRods2(&rods, path);
    }

    for (int i = 5; i < 15; i ++) {
        Problem prob = MakeRandomInterestingProblem();
        Vec rods = GetRandomRodsForProblem(prob, 20);
        char path[200] = {0};
        sprintf(path, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problemes/problem_%d.rods", i);
        WriteRods2(&rods, path);
        char path2[200] = {0};
        sprintf(path2, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problemes/problem_%d.statement", i);
        WriteProblem(prob, path2);
    }
    for (int i = 15; i < 20; i ++) {
        Vec rods = MakePretestRods();
        char path[200] = {0};
        sprintf(path, "/home/balthazar/job/research/Cuisenaire_rods/Code/yahr/data/experiment/problemes/problem_%d.rods", i);
        WriteRods2(&rods, path);
    }
}

int main(void) {

    int nbRodsPerLengths1[10] = {
        3,
        4,
        3,
        2,
        1,
        2,
        1,
        2,
        1,
        1
    };
    MakeAndSaveProblem(
        3,
        2,
        8,
        nbRodsPerLengths1,
        5
    );

    int nbRodsPerLengths2[10] = {
        6,
        2,
        3,
        2,
        1,
        2,
        1,
        1,
        1,
        1
    };
    MakeAndSaveProblem(
        5,
        4,
        1,
        nbRodsPerLengths2,
        6
    );


    int nbRodsPerLengths3[10] = {
        2,
        3,
        3,
        3,
        1,
        2,
        1,
        2,
        2,
        1
    };
    MakeAndSaveProblem(
        12,
        9,
        3,
        nbRodsPerLengths3,
        7
    );


    int nbRodsPerLengths4[10] = {
        3,
        3,
        3,
        3,
        1,
        1,
        2,
        2,
        1,
        1
    };
    MakeAndSaveProblem(
        4,
        2,
        3,
        nbRodsPerLengths3,
        8
    );

    int nbRodsPerLengths5[10] = {
        4,
        2,
        3,
        3,
        1,
        2,
        1,
        1,
        1,
        1
    };
    MakeAndSaveProblem(
        4,
        3,
        2,
        nbRodsPerLengths5,
        9
    );

    int nbRodsPerLengths6[10] = {
        4,
        2,
        4,
        3,
        1,
        2,
        1,
        1,
        1,
        1
    };
    MakeAndSaveProblem(
        2,
        3,
        4,
        nbRodsPerLengths6,
        10
    );


    int nbRodsPerLengths7[10] = {
        3,
        4,
        2,
        3,
        1,
        2,
        1,
        2,
        1,
        1
    };
    MakeAndSaveProblem(
        6,
        8,
        5,
        nbRodsPerLengths7,
        11
    );


    int nbRodsPerLengths8[10] = {
        2,
        4,
        3,
        2,
        1,
        2,
        1,
        2,
        1,
        2
    };
    MakeAndSaveProblem(
        5,
        6,
        4,
        nbRodsPerLengths8,
        12
    );


    int nbRodsPerLengths9[10] = {
        4,
        2,
        5,
        1,
        3,
        1,
        1,
        1,
        1,
        1
    };
    MakeAndSaveProblem(
        2,
        3,
        5,
        nbRodsPerLengths9,
        13
    );


    int nbRodsPerLengths10[10] = {
        3,
        7,
        1,
        1,
        2,
        1,
        2,
        1,
        1,
        1
    };
    MakeAndSaveProblem(
        2,
        3,
        5,
        nbRodsPerLengths10,
        14
    );

    int nbRodsPerLengths11[10] = {
        3,
        7,
        1,
        1,
        2,
        1,
        2,
        1,
        1,
        1
    };
    MakeAndSaveProblem(
        2,
        3,
        5,
        nbRodsPerLengths11,
        15
    );


    return 0;
}