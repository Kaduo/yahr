#include "collision.h"

bool StrictlyCollide(Rectangle rect1, Rectangle rect2)
{
  return (RelativeXPosition(rect1, rect2) == X_ALIGNED) && (RelativeYPosition(rect1, rect2) == Y_ALIGNED);
}

bool SoftlyCollide(Rectangle rect1, Rectangle rect2)
{
  return (RelativeXPosition(rect1, rect2) & (X_ALIGNED | JUST_RIGHT | JUST_LEFT)) && (RelativeYPosition(rect1, rect2) & (Y_ALIGNED | JUST_ABOVE | JUST_BELOW));
}

CollisionType CheckStrictCollision(Rectangle rect_before, Rectangle rect_after, Rectangle other_rect)
{
  if (!StrictlyCollide(rect_after, other_rect))
  {
    return NO_STRICT_COLLISION;
  }

  switch (RelativeYPosition(rect_before, other_rect))
  {
  case JUST_ABOVE: case STRICTLY_ABOVE:
    return FROM_ABOVE;
  case JUST_BELOW: case STRICTLY_BELOW:
    return FROM_BELOW;
  default:
    break;
  }

  switch (RelativeXPosition(rect_before, other_rect))
  {
  case JUST_LEFT: case STRICTLY_LEFT:
    return FROM_LEFT;
  case JUST_RIGHT: case STRICTLY_RIGHT:
    return FROM_RIGHT;
  default:
    // fprintf(stderr, "THIS HSHOLDN4T AHPPEN!\n");
    return NO_STRICT_COLLISION;
  }
}


Bound newBound(Rectangle boundingRectangle, CollisionType collisionType)
{
  float value;
  switch (collisionType)
  {
  case FROM_ABOVE:
    value = GetTop(boundingRectangle);
    break;
  case FROM_BELOW:
    value = GetBottom(boundingRectangle);
    break;

  case FROM_RIGHT:
    value = GetRight(boundingRectangle);
    break;

  case FROM_LEFT:
    value = GetLeft(boundingRectangle);
    break;

  default:
    fprintf(stderr, "SHOULDN'T HAPPEN!!\n ONLY CALL THIS FUNCTION WHEN THERE IS A COLLISION !\n");
    abort();
  }
  return (Bound){value, collisionType};
}

Rectangle CloneMove(Rectangle rect, Vector2 targetTopLeft)
{
  Rectangle clone = rect;
  SetTopLeft(&clone, targetTopLeft);
  return clone;
}

void AddBound(Bounds *bounds, Bound bound) {
  bounds->bounds[bounds->nbBounds] = bound;
  bounds->nbBounds += 1;
}

void AddYBound(CollisionInfo *collisionInfo, Bound bound) {
  AddBound(&collisionInfo->yBounds, bound);
}

void AddXBound(CollisionInfo *collisionInfo, Bound bound) {
  AddBound(&collisionInfo->xBounds, bound);
}

bool CollidedWithAnotherRod(const CollisionInfo collisionInfo) {
  return collisionInfo.strictlyCollided;
}

CollisionInfo ComputeCollisionInfo(Iterator *world, Rectangle movingRectangle, Rectangle targetRectangle) {

  CollisionInfo collisionInfo = {.strictlyCollided = false, .collided = false};
  Rod **otherRod;

  world->reinit(world);
  while ((otherRod = (Rod**)world->next(world)) != NULL)
  {
    Rectangle otherRectangle = GetRodRectangle(*otherRod);
    // THIS CHECK SHOULD BE USELESS!
    // if (otherRectangle == movingRectangle) {
    //   continue;
    // }

    if (CheckCollisionRecs(targetRectangle, otherRectangle)) {
      collisionInfo.collided = true;
    }
    CollisionType collisionType = CheckStrictCollision(movingRectangle, targetRectangle, otherRectangle);
    if (collisionType != NO_STRICT_COLLISION)
    {
      if (collisionType == FROM_ABOVE || collisionType == FROM_BELOW)
      {
        AddYBound(&collisionInfo, newBound(otherRectangle, collisionType));
      }
      else
      {
        AddXBound(&collisionInfo, newBound(otherRectangle, collisionType));
      }

      collisionInfo.strictlyCollided = true;
    }
  }

  if (collisionInfo.strictlyCollided) { // Virtual bounds, corresponding to the rod being aligned with its original position or its target position.
    AddYBound(&collisionInfo, (Bound){.value =  GetBottom(targetRectangle), FROM_ABOVE});
    AddXBound(&collisionInfo, (Bound){.value =  GetRight(targetRectangle), FROM_LEFT});
    AddYBound(&collisionInfo, (Bound){.value =  GetBottom(movingRectangle), FROM_ABOVE});
    AddXBound(&collisionInfo, (Bound){.value =  GetRight(movingRectangle), FROM_LEFT});
  }

  collisionInfo.collided = collisionInfo.strictlyCollided || collisionInfo.collided;
  return collisionInfo;
}
