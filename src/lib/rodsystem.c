#include "rodsystem.h"
#include "raylib.h"
#include "raymath.h"
#include "coords.h"
#include "collision2.h"
#include "consts.h"
#include "serialize.h"

Rod2 *RodSystem_getSelectedRod(RodSystem *me) {
    if (me->selectedRodId >= 0) {
        return (Rod2*)IndexVec(&me->rods, me->selectedRodId);
    } else {
        return NULL;
    }
}

void RodSystem_updateSelection(RodSystem *me) {
    me->previousSelectedRodId = me->selectedRodId;
    Vector2 mousePosition = me->inputService->getMousePosition(me->inputService);
    if (me->inputService->isMouseButtonPressed(me->inputService, MOUSE_BUTTON_LEFT)) {
        int closestRodId = -1;
        float smallestDist = 10000;
        me->selectedRodId = -1;
        for (int i = 0; i<me->rods.size; i++) {
            Rod2 rod = *(Rod2*)IndexVec(&me->rods, i);
            if (CheckCollisionPointRec(mousePosition, rod.rect)) {
                me->selectedRodId = i;
                break;
            }

            float dist = Vector2Distance(mousePosition, (Vector2){rod.rect.x + rod.rect.width/2, rod.rect.y + rod.rect.height/2});
            if (dist < smallestDist) {
                smallestDist = dist;
                closestRodId = i;
            }
        }
        if (me->selectedRodId < 0) {
            me->selectedRodId = closestRodId;
        }


        Rod2 *selectedRod = RodSystem_getSelectedRod(me);
        me->selectionOffset = Vector2Subtract(mousePosition, GetTopLeft(selectedRod->rect));
    } else if (me->inputService->isMouseButtonReleased(me->inputService, MOUSE_BUTTON_LEFT)) {
        me->selectedRodId = -1;
    }
}

void RodSystem_updatePosition(RodSystem *me) {
    Rod2 *selectedRod = RodSystem_getSelectedRod(me);
    if (selectedRod != NULL) {
        Vector2 mousePosition = me->inputService->getMousePosition(me->inputService);
        Vector2 targetPosition = Vector2Subtract(mousePosition, me->selectionOffset);
        Rectangle targetRectangle = (Rectangle){.x = targetPosition.x, .y = targetPosition.y,
                                                .width = selectedRod->rect.width, .height = selectedRod->rect.height};
        
        
        
        CollisionInfo2 collisionInfo = ComputeCollisionInfo2(&me->rods, me->selectedRodId, targetRectangle);

        me->previousCollided = me->collided;
        if (!collisionInfo.collided) {
            me->collided = false;
            selectedRod->rect = targetRectangle;
        } else {
            me->collided = true;
            Rectangle candidateRectangle = selectedRod->rect;
            Rectangle bestRectangle = candidateRectangle;
            float bestDist = Vector2DistanceSqr(GetTopLeft(targetRectangle), GetTopLeft(candidateRectangle));
            for (int ix = 0; ix < collisionInfo.xBounds.nbBounds; ix++)
            {
                for (int iy = 0; iy < collisionInfo.yBounds.nbBounds; iy++)
                {
                    Bound2 yBound = collisionInfo.yBounds.bounds[iy];
                    Bound2 xBound = collisionInfo.xBounds.bounds[ix];
                    if (yBound.collisionType == FROM_ABOVE)
                    {
                        SetBottom(&candidateRectangle, yBound.value);
                    }
                    else
                    {
                        SetTop(&candidateRectangle, yBound.value);
                    }
        
                    if (xBound.collisionType == FROM_LEFT)
                    {
                        SetRight(&candidateRectangle, xBound.value);
                    }
                    else
                    {
                        SetLeft(&candidateRectangle, xBound.value);
                    }
        
                    float candidateDist = Vector2DistanceSqr(GetTopLeft(targetRectangle), GetTopLeft(candidateRectangle));
        
                    if (candidateDist < bestDist)
                    {
                        bool noCollision = true;
    
                        for (int i = 0; i < me->rods.size; i++)
                        {
                            if (i == me->selectedRodId) {
                                continue;
                            }
                            Rod2 *otherRod = (Rod2*)IndexVec(&me->rods, i);
                            Rectangle otherRectangle = otherRod->rect;
                            if (StrictlyCollide2(otherRectangle, candidateRectangle))
                            {
                                noCollision = false;
                                break;
                            }
                        }
                        if (noCollision)
                        {
                            bestDist = candidateDist;
                            bestRectangle = candidateRectangle;
                        }
                    }
                }
            }
    
            selectedRod->rect = bestRectangle;
        }

    }
}

void RodSystem_updateHaptic(RodSystem *me) {
    if (me->selectedRodId < 0) {
        HapticService_clearAndPauseQueue(me->hapticService);
        return;
    }

    Rod2 *selectedRod = RodSystem_getSelectedRod(me);

    if (!me->previousCollided && me->collided) {
        HapticService_clearAndPauseQueue(me->hapticService);
        HapticService_addSignalToQueue(me->hapticService, IMPULSE_SIGNAL);
        HapticService_addSignalToQueue(me->hapticService, COLLISION_SIGNAL);
        HapticService_playQueue(me->hapticService);
    } else if ((!me->collided && me->previousCollided) || me->previousSelectedRodId < 0) {
        HapticService_clearAndPauseQueue(me->hapticService);
        HapticService_playSignal(me->hapticService, selectedRod->signal);
    }
}

void DrawRod2(Rod2 rod) {
    DrawRectangleRec(rod.rect, rod.color);
    DrawRectangleLinesEx(rod.rect, 1, BLACK);
}

void RodSystem_drawRods(RodSystem *me) {
    for (int i =0; i<me->rods.size; i++) {
        Rod2 rod = *(Rod2*)IndexVec(&me->rods, i);
        DrawRod2(rod);
    }
}

void RodSystem_updateAndDrawRods(RodSystem *me) {
    RodSystem_updateSelection(me);
    RodSystem_updatePosition(me);
    RodSystem_updateHaptic(me);
    RodSystem_drawRods(me);

}

void ShuffleRods2(Vec *rods, float xMin, float xMax, float yMin, float yMax) {
    for (int i =0; i < rods->size; i ++) {
        Rod2 *rod = (Rod2*)IndexVec(rods, i);
        CollisionInfo2 collisionInfo;
        int myYMax = yMax - rod->rect.height;
        int myXMax = xMax - rod->rect.width;
        float x;
        float y;

        Rectangle targetRectangle;
        Rectangle rodRectangle = rod->rect;

        do {
            x = xMin + (float)rand()/(float)(RAND_MAX/(myXMax - xMin));
            y = yMin + (float)rand()/(float)(RAND_MAX/(myYMax - yMin));
            targetRectangle = CloneMove2(rodRectangle, (Vector2){x, y});
            collisionInfo = ComputeCollisionInfo2(rods, i, targetRectangle);
        } while (collisionInfo.collided);
        rod->rect.x = x;
        rod->rect.y = y;
    }
}


void RodSystem_shuffleRods(RodSystem *me, float xMin, float xMax, float yMin, float yMax) {
    ShuffleRods2(&me->rods, xMin, xMax, yMin, yMax);
}



void RodSystem_addRod(RodSystem *me, Rod2 rod) {
    PushVec(&me->rods, &rod);
}

void WriteRod2(FILE *file, Rod2 rod) {
    fprintf(file, "rod\n");
    fprintf(file, "n %d endn\n", rod.n);
    WriteRect(file, rod.rect);
    WriteColor(file, (rod.color));
    WriteSignal(file, rod.signal);
    fprintf(file, "endrod\n");
}

Rod2 ReadRod2(FILE *file) {
    fscanf(file, "rod\n");
    int n;
    fscanf(file, "n %d endn\n", &n);
    Rectangle rect = ReadRect(file);
    Color color = ReadColor(file);
    Signal signal = ReadSignal(file);
    fscanf(file, "endrod\n");
    return (Rod2){
        .n = n,
        .color = color,
        .signal = signal, 
        .rect = rect,
    };
}



void SaveRodSystem(RodSystem *me, char *filename) {
    FILE *save = fopen(filename, "w");
    if (save != NULL) {
        fprintf(save, "nbRods %d\n", me->rods.size);
        for (int i = 0; i<me->rods.size; i++) {
            WriteRod2(save, *(Rod2*)IndexVec(&me->rods, i));
        }
    } else {
        perror(0);
        exit(1);
    }
}

void RodSystem_clear(RodSystem *me) {
    me->collided= false;
    me->previousCollided = false;
    me->selectedRodId = -1;
    me->previousSelectedRodId = -1;
    me->selectionOffset = (Vector2){0, 0};
    ClearVec(&me->rods);
}

void RodSystem_loadRods(RodSystem *me, char *filename) {
    RodSystem_clear(me);
    FILE *save=  fopen(filename, "r");
    if (save != NULL) {
        int nbRods;
        fscanf(save, "nbRods %d\n", &nbRods);
        for (int i = 0; i <nbRods; i++) {
            Rod2 rod = ReadRod2(save);
            PushVec(&me->rods, &rod);
        }
    } else {
        perror(0);
        exit(1);
    }
}

RodSystem NewRodSystem(InputService *inputService, HapticService *hapticService) {
    return (RodSystem){
        .collided = false,
        .previousCollided = false,
        .hapticService = hapticService,
        .inputService = inputService,
        .rods = EmptyVec(20, sizeof(Rod2)),
        .selectedRodId = -1,
        .previousSelectedRodId = -1,
        .selectionOffset = {0, 0},
    };
}

void WriteRods2(Vec *rods, char *filename) {
    printf("%s\n", filename);
    FILE *save = fopen(filename, "w");
    if (save != NULL) {
        fprintf(save, "nbRods %d\n", rods->size);
        for (int i = 0; i<rods->size; i++) {
            WriteRod2(save, *(Rod2*)IndexVec(rods, i));
        }

        fclose(save);

    } else {
        perror(0);
        exit(1);
    }
}