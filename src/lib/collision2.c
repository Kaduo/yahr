#include "collision2.h"

bool StrictlyCollide2(Rectangle rect1, Rectangle rect2)
{
  return (RelativeXPosition(rect1, rect2) == X_ALIGNED) && (RelativeYPosition(rect1, rect2) == Y_ALIGNED);
}

bool SoftlyCollide2(Rectangle rect1, Rectangle rect2)
{
  return (RelativeXPosition(rect1, rect2) & (X_ALIGNED | JUST_RIGHT | JUST_LEFT)) && (RelativeYPosition(rect1, rect2) & (Y_ALIGNED | JUST_ABOVE | JUST_BELOW));
}

CollisionType2 CheckStrictCollision2(Rectangle rect_before, Rectangle rect_after, Rectangle other_rect)
{
  if (!StrictlyCollide2(rect_after, other_rect))
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


Bound2 newBound2(Rectangle boundingRectangle, CollisionType2 collisionType)
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
  return (Bound2){value, collisionType};
}

Rectangle CloneMove2(Rectangle rect, Vector2 targetTopLeft)
{
  Rectangle clone = rect;
  SetTopLeft(&clone, targetTopLeft);
  return clone;
}

void AddBound2(Bounds2 *bounds, Bound2 bound) {
  bounds->bounds[bounds->nbBounds] = bound;
  bounds->nbBounds += 1;
}

void AddYBound2(CollisionInfo2 *collisionInfo, Bound2 bound) {
  AddBound2(&collisionInfo->yBounds, bound);
}

void AddXBound2(CollisionInfo2 *collisionInfo, Bound2 bound) {
  AddBound2(&collisionInfo->xBounds, bound);
}

bool CollidedWithAnotherRod2(const CollisionInfo2 collisionInfo) {
  return collisionInfo.strictlyCollided;
}

CollisionInfo2 ComputeCollisionInfo2(RodSystem *rodSystem, Rectangle targetRectangle) {

  CollisionInfo2 collisionInfo = {.strictlyCollided = false, .collided = false};

  Rod2 *selectedRod = RodSystem_getSelectedRod(rodSystem);
  Rectangle selectedRectangle = selectedRod->rect;
  // while ((otherRod = (Rod**)world->next(world)) != NULL)
  for (int i = 0; i<rodSystem->rods.size; i++)
  {

    if (i == rodSystem->selectedRodId) {
      continue;
    }

    Rod2 *otherRod = (Rod2*)IndexVec(&rodSystem->rods, i);
    Rectangle otherRectangle = otherRod->rect;
    // THIS CHECK SHOULD BE USELESS!
    // if (otherRectangle == movingRectangle) {
    //   continue;
    // }

    if (CheckCollisionRecs(targetRectangle, otherRectangle)) {
      collisionInfo.collided = true;
    }
    CollisionType2 collisionType = CheckStrictCollision2(selectedRectangle, targetRectangle, otherRectangle);
    if (collisionType != NO_STRICT_COLLISION)
    {
      if (collisionType == FROM_ABOVE || collisionType == FROM_BELOW)
      {
        AddYBound2(&collisionInfo, newBound2(otherRectangle, collisionType));
      }
      else
      {
        AddXBound2(&collisionInfo, newBound2(otherRectangle, collisionType));
      }

      collisionInfo.strictlyCollided = true;
    }
  }

  if (collisionInfo.strictlyCollided) { // Virtual bounds, corresponding to the rod being aligned with its original position or its target position.
    AddYBound2(&collisionInfo, (Bound2){.value =  GetBottom(targetRectangle), FROM_ABOVE});
    AddXBound2(&collisionInfo, (Bound2){.value =  GetRight(targetRectangle), FROM_LEFT});
    AddYBound2(&collisionInfo, (Bound2){.value =  GetBottom(selectedRectangle), FROM_ABOVE});
    AddXBound2(&collisionInfo, (Bound2){.value =  GetRight(selectedRectangle), FROM_LEFT});
  }

  collisionInfo.collided = collisionInfo.strictlyCollided || collisionInfo.collided;
  return collisionInfo;
}
