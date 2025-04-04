#pragma once
#include "raylib.h"

// #include "raylib.h"
#include "vec.h"
// #include "haptic.h"
#include "input.h"
// #include "raymath.h"
// #include "stdlib.h"
// #include "coords.h"
// #include "vec.h"
// #include "collision.h"
// #include "stdio.h"
#include "haptic.h"
// #include "consts.h"


typedef struct InputComponent InputComponent;
typedef struct CollisionComponent CollisionComponent;
typedef struct RodMoveComponent RodMoveComponent;
typedef struct RodCollisionComponent RodCollisionComponent;


struct InputComponent
{
    InputService *inputService;
    void (*update)(InputComponent *me);
};




typedef struct SelectionObserver SelectionObserver;

struct SelectionObserver
{
    void (*targetChangedCallback)(SelectionObserver *me, Vector2 target);
    void (*selectionStatusChangedCallback)(SelectionObserver *me, bool selected);
};

typedef struct CollisionComponent
{
    Iterator **world; // The world's other hitboxes, not including its own!!!
    const Vector2 *position;
    float hitboxWidth;
    float hitboxHeight;
    Vec observers;
} CollisionComponent;

Rectangle GetHitbox(const CollisionComponent *me);



struct RodCollisionComponent
{
    SelectionObserver super;
    CollisionComponent inner;
};

typedef struct GraphicsComponent GraphicsComponent;

struct GraphicsComponent {
    void (*draw)(GraphicsComponent *me);
};

typedef struct RodGraphicsComponent {
    GraphicsComponent super;
    const Vector2 *position;
    float height;
    float width;
    Color color;
} RodGraphicsComponent;


typedef struct CollisionObserver CollisionObserver;

struct CollisionObserver
{
    void (*collidedOrNotCallback)(CollisionObserver *me, bool collided);
    void (*targetCorrectedCallback)(CollisionObserver *me, Vector2 correctedTarget);
};


typedef struct RodHapticComponent {
    CollisionObserver collisionObserver;
    SelectionObserver selectionObserver;
    HapticService *hapticService;
    Signal signal;
    bool colliding;
} RodHapticComponent;


typedef struct Rod
{
    Vector2 position;
    float width;
    float height;
    Iterator *world;
    InputComponent *inputComponent;
    RodCollisionComponent *collisionComponent;
    RodMoveComponent *rodMoveComponent;
    RodHapticComponent *rodHapticComponent;
    GraphicsComponent *graphicsComponent;
} Rod;


Rod *NewRod(Rectangle rect, Color color, InputService *inputService, Iterator *world, HapticService *hapticService, Signal signal);
void DeleteRod(Rod *rod);
Rectangle GetRodRectangle(const Rod *rod);

typedef struct Zone {
    Rectangle rect;
    GraphicsComponent *graphicsComponent;
} Zone;

Zone *NewZone(Rectangle rect, Color color);