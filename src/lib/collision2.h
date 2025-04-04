#pragma once
#include "raylib.h"
#include "vec.h"
#include "stdlib.h"
#include "stdbool.h"
#include "coords.h"
#include "stdio.h"
#include "vec.h"
#include "rodsystem.h"


#define BOUNDS_MAX_COUNT2 22


typedef enum CollisionType2
{
  NO_STRICT_COLLISION,
  FROM_LEFT,
  FROM_RIGHT,
  FROM_ABOVE,
  FROM_BELOW,
} CollisionType2;

typedef struct Bound2
{
  float value;
  CollisionType2 collisionType;
} Bound2;


typedef struct Bounds2 {
  int nbBounds;
  Bound2 bounds[BOUNDS_MAX_COUNT2];
} Bounds2;

typedef struct CollisionInfo2 {
  Bounds2 xBounds;
  Bounds2 yBounds;
  bool strictlyCollided;
  bool collided;
} CollisionInfo2;

Rectangle CloneMove2(Rectangle rect, Vector2 targetTopLeft);
CollisionInfo2 ComputeCollisionInfo2(RodSystem *rodSystem, Rectangle targetRectangle);
bool StrictlyCollide2(Rectangle rect1, Rectangle rect2);
bool CollidedWithAnotherRod2(const CollisionInfo2 collisionInfo);