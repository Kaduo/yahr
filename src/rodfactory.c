#include "gameobject.h"
#include "vec.h"
#include "stdlib.h"
#include "rodfactory.h"
#include "haptic.h"

Rods EmptyRods(int capacity)
{
    return (Rods){
        .rods = EmptyVec(capacity, sizeof(Rod *))};
}

void AppendRod(Rods *me, Rod *rod)
{
    VecIteratorSkip *world = malloc(sizeof(VecIteratorSkip));
    *world = NewVecIteratorSkip(&me->rods, me->rods.size);
    rod->world = (Iterator *)world;
    PushVec(&me->rods, &rod);
}

void DrawRods(Rods *me)
{
    for (int i = 0; i < me->rods.size; i++)
    {
        Rod *rod = *(Rod **)IndexVec(&me->rods, i);
        rod->graphicsComponent->draw(rod->graphicsComponent);
    }
}

void UpdateRods(Rods *me)
{
    for (int i = 0; i < me->rods.size; i++)
    {
        Rod *rod = *(Rod **)IndexVec(&me->rods, i);
        rod->inputComponent->update(rod->inputComponent);
    }
}

void ClearRods(Rods *me) {
    for (int i = 0; i < me->rods.size; i ++)
    {
        DeleteRod(*(Rod**)IndexVec(&me->rods, i));
    }
    ClearVec(&me->rods);
}

void FreeRods(Rods *me) {
    ClearRods(me);
    free(me);
}

Rod *CreateRod(const StandardRodFactory *me, int length, float x, float y, Color color, Signal signal)
{
    return NewRod((Rectangle){
                      .x = x,
                      .y = y,
                      .height = me->rodUnitHeight,
                      .width = length * me->rodUnitHeight},
                color,
                me->inputService,
                NULL,
                me->hapticService,
                signal);
}

void SetRodServices(Rods *rods, InputService *inputService, HapticService *hapticService) {
    for (int i = 0; i < rods->rods.size; i++) {
        Rod *rod = *(Rod**)IndexVec(&rods->rods, i);
        if (inputService != NULL) {
            rod->inputComponent->inputService = inputService;
        }
        if (hapticService != NULL) {
            rod->rodHapticComponent->hapticService = hapticService;
        }
    }
}