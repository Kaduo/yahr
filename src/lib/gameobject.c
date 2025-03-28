#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "coords.h"
#include "vec.h"
#include "collision.h"
#include "gameobject.h"
#include "stdio.h"
#include "haptic.h"
#include "consts.h"

typedef struct SelectableObject SelectableObject;
typedef struct SelectionComponent SelectionComponent;
typedef struct ClickInputComponent ClickInputComponent;
typedef struct SignalComponent SignalComponent;
typedef struct UpdatableObject UpdatableObject;

void DoNothingInputComponent_update(InputComponent *me) {}

void InputComponent_Construct(InputComponent *me)
{
    me->update = DoNothingInputComponent_update;
}

InputComponent NewInputComponent(InputService *inputService)
{
    InputComponent inputComponent = {.inputService = inputService};
    InputComponent_Construct(&inputComponent);
    return inputComponent;
}

typedef struct ClickInputComponent ClickInputComponent;
struct ClickInputComponent
{
    InputComponent super;
    const Vector2 *position;
    float clickboxHeight;
    float clickboxWidth;
    void (*onClick)(ClickInputComponent *me, Vector2 mousePosition, bool inside);
    void (*onRelease)(ClickInputComponent *me, Vector2 mousePosition, bool inside);
    void (*onHoverDown)(ClickInputComponent *me, Vector2 mousePosition, bool inside);
    void (*onHoverUp)(ClickInputComponent *me, Vector2 mousePosition, bool inside);
};

void DoNothingClickInputComponent_onAnything(ClickInputComponent *me, Vector2 mousePosition, bool inside) {}

Rectangle GetClickbox(const ClickInputComponent *me)
{
    return (Rectangle){.x = me->position->x, .y = me->position->y, .width = me->clickboxWidth, .height = me->clickboxHeight};
}

void ClickInputComponent_update(InputComponent *_me)
{
    ClickInputComponent *me = (ClickInputComponent*)_me;
    Vector2 mousePosition = me->super.inputService->getMousePosition(me->super.inputService);
    ButtonStatus leftMouseButtonStatus = GetButtonStatus(me->super.inputService, MOUSE_BUTTON_LEFT);

    bool inside = CheckCollisionPointRec(mousePosition, GetClickbox(me));
    switch (leftMouseButtonStatus)
    {
    case PRESSED:
        me->onClick(me, mousePosition, inside);
        break;
    case RELEASED:
        me->onRelease(me, mousePosition, inside);
        break;
    case DOWN:
        me->onHoverDown(me, mousePosition, inside);
        break;
    case UP:
        me->onHoverUp(me, mousePosition, inside);
        break;
    }
}

void ClickInputComponent_Construct(ClickInputComponent *me)
{
    me->super.update = ClickInputComponent_update;
}

ClickInputComponent NewClickInputComponent(InputService *inputService, Vector2 *position, float clickboxWidth, float clickboxHeight)
{
    InputComponent super = NewInputComponent(inputService);
    ClickInputComponent clickInputComponent = {
        .super = super,
        .position = position,
        .clickboxHeight = clickboxHeight,
        .clickboxWidth = clickboxWidth,
        .onClick = DoNothingClickInputComponent_onAnything,
        .onHoverDown = DoNothingClickInputComponent_onAnything,
        .onHoverUp = DoNothingClickInputComponent_onAnything,
        .onRelease = DoNothingClickInputComponent_onAnything
        };
    ClickInputComponent_Construct(&clickInputComponent);
    return clickInputComponent;
}
typedef struct SelectionInputComponent SelectionInputComponent;

struct SelectionInputComponent
{
    ClickInputComponent super;
    void (*setTarget)(SelectionInputComponent *me, Vector2 mousePosition);
    Vec observers;
    Vector2 target;
    bool isSelected;
};

// void SelectionInputComponent()

void SelectionInputComponent_notifyTargetChanged(SelectionInputComponent *me)
{
    for (int i = 0; i < me->observers.size; i++)
    {
        SelectionObserver *observer = *(SelectionObserver **)IndexVec(&me->observers, i);
        if (observer->targetChangedCallback != NULL)
        {
            observer->targetChangedCallback(observer, me->target);
        }
    }
}

void SelectionInputComponent_notifySelectionStatusChanged(SelectionInputComponent *me)
{
    for (int i = 0; i < me->observers.size; i++)
    {
        SelectionObserver *observer = *(SelectionObserver **)IndexVec(&me->observers, i);
        if (observer->selectionStatusChangedCallback != NULL)
        {
            observer->selectionStatusChangedCallback(observer, me->isSelected);
        }
    }
}

void SelectionInputComponent_onClick(ClickInputComponent *_me, Vector2 mousePosition, bool inside)
{
    SelectionInputComponent *me = (SelectionInputComponent*)_me;
    if (inside)
    {
        me->setTarget(me, mousePosition);
        me->isSelected = true;
        SelectionInputComponent_notifySelectionStatusChanged(me);
        SelectionInputComponent_notifyTargetChanged(me);
    }
}

void SelectionInputComponent_onRelease(ClickInputComponent *_me, Vector2 mousePosition, bool inside)
{
    SelectionInputComponent *me = (SelectionInputComponent*)_me;
    if (me->isSelected) {
        me->isSelected = false;
        me->setTarget(me, *me->super.position);
        SelectionInputComponent_notifySelectionStatusChanged(me);
    }
}

void SelectionInputComponent_onHoverDown(ClickInputComponent *_me, Vector2 mousePosition, bool inside)
{
    SelectionInputComponent *me = (SelectionInputComponent*)_me;
    if (me->isSelected)
    {
        me->setTarget(me, mousePosition);
        SelectionInputComponent_notifyTargetChanged(me);
    }
}

void SelectionInputComponent_Construct(SelectionInputComponent *me)
{
    me->super.onRelease = SelectionInputComponent_onRelease;
    me->super.onClick = SelectionInputComponent_onClick;
    me->super.onHoverDown = SelectionInputComponent_onHoverDown;
}

SelectionInputComponent NewSelectionInputComponent(InputService *inputService, Vector2 *position, float clickboxHeight, float clickboxWidth)
{
    ClickInputComponent super = NewClickInputComponent(inputService, position, clickboxWidth, clickboxHeight);
    SelectionInputComponent selectionInputComponent = {
        .super = super,
        .target = *position,
        .observers = EmptyVec(2, sizeof(SelectionObserver*)),
        .isSelected = false};
    SelectionInputComponent_Construct(&selectionInputComponent);
    return selectionInputComponent;
}

void AbsoluteSelectionInputComponent_setTarget(SelectionInputComponent *me, Vector2 mousePosition)
{
    me->target = mousePosition;
}

void AbsoluteSelectionInputComponent_Construct(SelectionInputComponent *me)
{
    me->setTarget = AbsoluteSelectionInputComponent_setTarget;
}

typedef struct RelativeSelectionInputComponent
{
    SelectionInputComponent super;
    Vector2 offset;
} RelativeSelectionInputComponent;

void RelativeSelectionInputComponent_setTarget(SelectionInputComponent *_me, Vector2 mousePosition)
{
    RelativeSelectionInputComponent *me = (RelativeSelectionInputComponent*)_me;

    // If not yet marked as selected, we need to update the offset
    if (!me->super.isSelected)
    {
        me->offset = Vector2Subtract(mousePosition, *me->super.super.position);
    }
    me->super.target = Vector2Subtract(mousePosition, me->offset);
}

void RelativeSelectionInputComponent_Construct(RelativeSelectionInputComponent *me)
{
    me->super.setTarget = RelativeSelectionInputComponent_setTarget;
}

RelativeSelectionInputComponent NewRelativeSelectionInputComponent(InputService *inputService, Vector2 *position, float clickboxHeight, float clickboxWidth)
{
    SelectionInputComponent super = NewSelectionInputComponent(inputService, position, clickboxHeight, clickboxWidth);
    RelativeSelectionInputComponent relativeSelectionInputComponent = {
        .super = super,
        .offset = (Vector2){0, 0}};
    RelativeSelectionInputComponent_Construct(&relativeSelectionInputComponent);
    return relativeSelectionInputComponent;
}


Rectangle GetHitbox(const CollisionComponent *me)
{
    return (Rectangle){.x = me->position->x, .y = me->position->y, .width = me->hitboxWidth, .height = me->hitboxHeight};
}

CollisionComponent NewCollisionComponent(Iterator **world, Vector2 *position, float hitboxHeight, float hitboxWidth)
{
    return (CollisionComponent){
        .world = world,
        .position = position,
        .hitboxHeight = hitboxHeight,
        .hitboxWidth = hitboxWidth,
        .observers = EmptyVec(2, sizeof(CollisionObserver *))};
}

// TODO : break into pieces
void CollisionComponent_computeAndNotifyCorrectedTarget(CollisionComponent *me, Vector2 target)
{
    Rectangle targetRectangle = CloneMove(GetHitbox(me), target);
    Rectangle movingRectangle = GetHitbox(me);
    CollisionInfo collisionInfo = ComputeCollisionInfo(*me->world, movingRectangle, targetRectangle);

    if (!CollidedWithAnotherRod(collisionInfo)) // Pas de collision
    {
        for (int i = 0; i < me->observers.size; i++)
        {
            CollisionObserver *observer = *(CollisionObserver **)IndexVec(&me->observers, i);
            if (observer->collidedOrNotCallback != NULL)
            {
                observer->collidedOrNotCallback(observer, false);
            }
            if (observer->targetCorrectedCallback != NULL)
            {
                observer->targetCorrectedCallback(observer, target);
            }
        }
        return;
    }

    for (int i = 0; i < me->observers.size; i++)
    {
        CollisionObserver *observer = *(CollisionObserver**)IndexVec(&me->observers, i);
        if (observer->collidedOrNotCallback != NULL)
        {
            observer->collidedOrNotCallback(observer, true);
        }
    }

    Rectangle candidateRectangle = movingRectangle;
    Rectangle bestRectangle = candidateRectangle;
    float bestDist = Vector2DistanceSqr(GetTopLeft(targetRectangle), GetTopLeft(candidateRectangle));
    for (int ix = 0; ix < collisionInfo.xBounds.nbBounds; ix++)
    {
        for (int iy = 0; iy < collisionInfo.yBounds.nbBounds; iy++)
        {
            Bound yBound = collisionInfo.yBounds.bounds[iy];
            Bound xBound = collisionInfo.xBounds.bounds[ix];
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

                Rod **otherRod;
                (*me->world)->reinit(*me->world);
                while ((otherRod = (Rod**)(*me->world)->next(*me->world)) != NULL)
                {
                    Rectangle otherRectangle = GetRodRectangle(*otherRod);
                    if (StrictlyCollide(otherRectangle, candidateRectangle))
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
    for (int i = 0; i < me->observers.size; i++)
    {
        CollisionObserver *observer = *(CollisionObserver**)IndexVec(&me->observers, i);
        if (observer->targetCorrectedCallback != NULL)
        {
            observer->targetCorrectedCallback(observer, GetTopLeft(bestRectangle));
        }
    }
}

void RodCollisionComponent_targetChangedCallback(SelectionObserver *_me, Vector2 target)
{
    RodCollisionComponent *me = (RodCollisionComponent*)_me;
    CollisionComponent_computeAndNotifyCorrectedTarget(&me->inner, target);
}

void RodCollisionComponent_Construct(RodCollisionComponent *me)
{
    me->super.targetChangedCallback = RodCollisionComponent_targetChangedCallback;
}

RodCollisionComponent NewRodCollisionComponent(Iterator **world, Vector2 *position, float hitboxHeight, float hitboxWidth)
{
    SelectionObserver super = {NULL, NULL};
    CollisionComponent inner = NewCollisionComponent(world, position, hitboxHeight, hitboxWidth);
    RodCollisionComponent rodCollisionComponent = {
        .super = super,
        .inner = inner,
    };
    RodCollisionComponent_Construct(&rodCollisionComponent);
    return rodCollisionComponent;
}

typedef struct RodMoveComponent
{
    CollisionObserver super;
    Vector2 *position;
} RodMoveComponent;

void RodMoveComponent_targetCorrectedCallback(CollisionObserver *_me, Vector2 target)
{
    RodMoveComponent *me = (RodMoveComponent*)_me;
    *me->position = target;
}

void RodMoveComponent_Construct(RodMoveComponent *me)
{
    me->super.targetCorrectedCallback = RodMoveComponent_targetCorrectedCallback;
}

RodMoveComponent NewRodMoveComponent(Vector2 *position)
{
    CollisionObserver super = {NULL};
    RodMoveComponent rodMoveComponent = {
        .super = super,
        .position = position};
    RodMoveComponent_Construct(&rodMoveComponent);
    return rodMoveComponent;
}

Rectangle RectangleFromPosAndDims(Vector2 position, float height, float width) {
    return (Rectangle){.x = position.x, .y = position.y, .height = height, .width = width};
}


void RodGraphicsComponent_draw(GraphicsComponent *_me) {
    RodGraphicsComponent *me = (RodGraphicsComponent*)_me;
    Rectangle rect = RectangleFromPosAndDims(*me->position, me->height, me->width);
    DrawRectangleRec(rect, me->color);
    DrawRectangleLinesEx(rect, 1, BLACK);
}

void RodGraphicsComponent_construct(RodGraphicsComponent *me) {
    me->super.draw = RodGraphicsComponent_draw;
}

RodGraphicsComponent NewRodGraphicsComponent(const Vector2 *position, float height, float width, Color color) {
    GraphicsComponent super = {0};
    RodGraphicsComponent rodGraphicsComponent = {
        .super = super,
        .position = position,
        .height = height,
        .width = width,
        .color = color
    };

    RodGraphicsComponent_construct(&rodGraphicsComponent);
    return rodGraphicsComponent;
}


void RodHapticComponent_selectionStatusChangedCallback(SelectionObserver *_me, bool selected) {
    RodHapticComponent *me = (RodHapticComponent*)((uintptr_t)_me - sizeof(CollisionObserver));
    if (selected) {
        HapticService_playSignal(me->hapticService, me->signal);
    } else {
        HapticService_clearAndPauseQueue(me->hapticService);
    }
    me->colliding = false;
}

void RodHapticComponent_collidedOrNotCallback(CollisionObserver *_me, bool collided) {
    RodHapticComponent *me = (RodHapticComponent*)_me;

    if (me->signal.signalShape == SignalShape_NO_SIGNAL) {
        return;
    }

    if (!collided && me->colliding) {
        HapticService_playSignal(me->hapticService, me->signal);
        me->colliding = false;
    } else if (collided && !me->colliding)  {
        me->colliding = true;
        HapticService_clearAndPauseQueue(me->hapticService);
        HapticService_addSignalToQueue(me->hapticService, IMPULSE_SIGNAL);
        HapticService_addSignalToQueue(me->hapticService, COLLISION_SIGNAL);
        HapticService_playQueue(me->hapticService);
    }
}

void RodHapticComponent_Construct(RodHapticComponent *me) {
    me->collisionObserver.collidedOrNotCallback = RodHapticComponent_collidedOrNotCallback;
    me->selectionObserver.selectionStatusChangedCallback = RodHapticComponent_selectionStatusChangedCallback;
}

RodHapticComponent NewRodHapticComponent(HapticService *hapticService, Signal signal) {
    RodHapticComponent rodHapticComponent = {
        .signal = signal,
        .hapticService = hapticService,
        .colliding = false,
        .collisionObserver = {NULL},
        .selectionObserver = {NULL},
    };
    RodHapticComponent_Construct(&rodHapticComponent);
    return rodHapticComponent;
}

Rectangle GetRodRectangle(const Rod *rod) {
    return (Rectangle){.x = rod->position.x, .y = rod->position.y, .width =rod->width, .height =rod->height};
}

Rod *NewRod(Rectangle rect, Color color, InputService *inputService, Iterator *world, HapticService *hapticService, Signal signal)
{
    Rod *rod = malloc(sizeof(Rod));
    rod->height = rect.height;
    rod->width = rect.width;
    rod->position = GetTopLeft(rect);
    rod->world = world;


    RelativeSelectionInputComponent *rodInputComponent = malloc(sizeof(RelativeSelectionInputComponent));
    *rodInputComponent = NewRelativeSelectionInputComponent(inputService, &rod->position, rod->height, rod->width);
    rod->inputComponent = (InputComponent *)rodInputComponent;

    RodCollisionComponent *rodCollisionComponent = malloc(sizeof(RodCollisionComponent));
    *rodCollisionComponent = NewRodCollisionComponent(&rod->world, &rod->position, rod->height, rod->width);
    rod->collisionComponent = rodCollisionComponent;
    void *temp = &rod->collisionComponent->super;
    PushVec(&rodInputComponent->super.observers, &temp);


    RodMoveComponent *rodMoveComponent = malloc(sizeof(RodMoveComponent));
    *rodMoveComponent = NewRodMoveComponent(&rod->position);
    rod->rodMoveComponent = rodMoveComponent;
    void *temp2 = &rod->rodMoveComponent->super;
    PushVec(&rodCollisionComponent->inner.observers, &temp2);


    RodHapticComponent *rodHapticComponent = malloc(sizeof(RodHapticComponent));
    *rodHapticComponent = NewRodHapticComponent(hapticService, signal);

    void *temp3 = &rodHapticComponent->collisionObserver;

    PushVec(&rodCollisionComponent->inner.observers, &temp3);


    void *temp4 = &rodHapticComponent->selectionObserver;
    PushVec(&rodInputComponent->super.observers, &temp4);
    rod->rodHapticComponent = rodHapticComponent;


    RodGraphicsComponent *rodGraphicsComponent = malloc(sizeof(RodGraphicsComponent));
    *rodGraphicsComponent = NewRodGraphicsComponent(&rod->position, rod->height ,rod->width, color);
    rod->graphicsComponent = (GraphicsComponent*)rodGraphicsComponent;

    return rod;
}

void DeleteRod(Rod *rod) {
    free(rod->world);
    free(rod->inputComponent);
    free(rod->collisionComponent);
    free(rod->rodMoveComponent);
    free(rod->graphicsComponent);
    free(rod->rodHapticComponent);
    free(rod);
}

typedef struct ZoneGraphicsComponent {
    GraphicsComponent super;
    Rectangle *rect;
    Color color;
} ZoneGraphicsComponent;

void ZoneGraphicsComponent_draw(GraphicsComponent *_me) {
    ZoneGraphicsComponent *me = (ZoneGraphicsComponent*)_me;
    DrawRectangleRoundedLinesEx(*me->rect, 0.2, 0, 1, me->color);
}

void ZoneGraphicsComponent_Construct(ZoneGraphicsComponent *me) {
    me->super.draw = ZoneGraphicsComponent_draw;
}

ZoneGraphicsComponent NewZoneGraphicsComponent(Rectangle *rect, Color color) {
    GraphicsComponent super = {0};
    ZoneGraphicsComponent zoneGraphicsComponent = {
        .super = super,
        .rect = rect,
        .color = color
    };

    ZoneGraphicsComponent_Construct(&zoneGraphicsComponent);
    return zoneGraphicsComponent;
}

Zone *NewZone(Rectangle rect, Color color) {
    Zone *zone = malloc(sizeof(Zone));
    zone->rect = rect;

    ZoneGraphicsComponent *zoneGraphicsComponent = malloc(sizeof(ZoneGraphicsComponent));
    *zoneGraphicsComponent = NewZoneGraphicsComponent(&zone->rect, color);
    zone->graphicsComponent = (GraphicsComponent*)zoneGraphicsComponent;

    return zone;
}
