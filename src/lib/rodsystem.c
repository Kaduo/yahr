#include "rodsystem.h"
#include "raylib.h"
#include "raymath.h"
#include "coords.h"
#include "collision2.h"
#include "consts.h"

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
        printf("wait !\n");
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
            printf("still there?\n");
            if (dist < smallestDist) {
                smallestDist = dist;
                closestRodId = i;
            }
        }
        printf("whooo\n");
        if (me->selectedRodId < 0) {
            me->selectedRodId = closestRodId;
        }

        printf("heree???\n");

        Rod2 *selectedRod = RodSystem_getSelectedRod(me);
        printf("heh\n");
        me->selectionOffset = Vector2Subtract(mousePosition, GetTopLeft(selectedRod->rect));
        printf("herehre\n");
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
        
        
        
        CollisionInfo2 collisionInfo = ComputeCollisionInfo2(me, targetRectangle);

        me->previousCollided = me->collided;
        if (!collisionInfo.collided) {
            me->collided = false;
            selectedRod->rect = targetRectangle;
        } else {
            printf("so far \n");
            me->collided = true;
            Rectangle candidateRectangle = selectedRod->rect;
            Rectangle bestRectangle = candidateRectangle;
            float bestDist = Vector2DistanceSqr(GetTopLeft(targetRectangle), GetTopLeft(candidateRectangle));
            for (int ix = 0; ix < collisionInfo.xBounds.nbBounds; ix++)
            {
                printf("so good?\n");
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

void RodSystem_drawRods(RodSystem *me) {
    for (int i =0; i<me->rods.size; i++) {
        Rod2 rod = *(Rod2*)IndexVec(&me->rods, i);
        DrawRectangleRec(rod.rect, rod.color);
    }
}

void RodSystem_updateAndDrawRods(RodSystem *me) {
    printf("huh!\n");
    RodSystem_updateSelection(me);
    printf("selection update\n");
    RodSystem_updatePosition(me);
    printf("position update\n");
    RodSystem_updateHaptic(me);
    printf("haptic update\n");

    RodSystem_drawRods(me);
    printf("draw update\n");

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