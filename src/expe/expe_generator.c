#include "problem.h"
#include "rodsystem.h"
#include "vec.h"
#include "consts.h"
#include "raylib.h"

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
    GenerateExpeProblems();
    return 0;
}