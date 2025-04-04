#pragma once
#include "raylib.h"
#include "vec.h"
#include "gameobject.h"
#include "stdlib.h"
#include "stdbool.h"
#include "raylib.h"
#include "coords.h"
#include "stdio.h"
#include "vec.h"



#define BOUNDS_MAX_COUNT 22


typedef enum CollisionType
{
  NO_STRICT_COLLISION,
  FROM_LEFT,
  FROM_RIGHT,
  FROM_ABOVE,
  FROM_BELOW,
} CollisionType;

typedef struct Bound
{
  float value;
  CollisionType collisionType;
} Bound;


typedef struct Bounds {
  int nbBounds;
  Bound bounds[BOUNDS_MAX_COUNT];
} Bounds;

typedef struct CollisionInfo {
  Bounds xBounds;
  Bounds yBounds;
  bool strictlyCollided;
  bool collided;
} CollisionInfo;

Rectangle CloneMove(Rectangle rect, Vector2 targetTopLeft);
CollisionInfo ComputeCollisionInfo(Iterator *world, Rectangle movingRectangle, Rectangle targetRectangle);
bool StrictlyCollide(Rectangle rect1, Rectangle rect2);
bool CollidedWithAnotherRod(const CollisionInfo collisionInfo);