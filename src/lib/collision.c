#include "raylib.h"
#include "coords.h"
#include "stdio.h"
#include "vec.h"
#include "collision.h"
#include "gameobject.h"
#include "stdlib.h"

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
    fprintf(stderr, "THIS HSHOLDN4T AHPPEN!\n");
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

void AddYBound(XYBounds *xyBounds, Bound bound) {
  AddBound(&xyBounds->yBounds, bound);
}

void AddXBound(XYBounds *xyBounds, Bound bound) {
  AddBound(&xyBounds->xBounds, bound);
}

XYBounds GetCollisionBounds(Iterator *world, Rectangle movingRectangle, Rectangle targetRectangle) {

  XYBounds xyBounds = {0};
  Rod **otherRod;

  world->reinit(world);
  while ((otherRod = (Rod**)world->next(world)) != NULL)
  {
    Rectangle otherRectangle = GetRodRectangle(*otherRod);
    // THIS CHECK SHOULD BE USELESS!
    // if (otherRectangle == movingRectangle) {
    //   continue;
    // }
    CollisionType collisionType = CheckStrictCollision(movingRectangle, targetRectangle, otherRectangle);
    if (collisionType != NO_STRICT_COLLISION)
    {
      if (collisionType == FROM_ABOVE || collisionType == FROM_BELOW)
      {
        AddYBound(&xyBounds, newBound(otherRectangle, collisionType));
      }
      else
      {
        AddXBound(&xyBounds, newBound(otherRectangle, collisionType));
      }
    }
  }
  AddYBound(&xyBounds, (Bound){.value =  GetBottom(targetRectangle), FROM_ABOVE});
  AddXBound(&xyBounds, (Bound){.value =  GetRight(targetRectangle), FROM_LEFT});
  AddYBound(&xyBounds,(Bound){.value =  GetBottom(movingRectangle), FROM_ABOVE});
  AddXBound(&xyBounds,(Bound){.value =  GetRight(movingRectangle), FROM_LEFT});

  return xyBounds;
}

// // Returns true if there was a collision
// bool Move(Iterator *world, Rectangle *movingRectangle, Vector2 targetTopLeft)
// {
//   Rectangle targetRectangle = CloneMove(*movingRectangle, targetTopLeft);

//   XYBounds xyBounds = GetCollisionBounds(world, movingRectangle, targetRectangle);

//   if (xyBounds.xBounds.nbBounds <= 2 && xyBounds.yBounds.nbBounds <= 2)
//   {
//     SetTopLeft(movingRectangle, targetTopLeft);
//     return false;
//   }

//   Rectangle candidateRectangle = *movingRectangle;
//   Rectangle bestRectangle = candidateRectangle;
//   float bestDist = Vector2DistanceSqr(GetTopLeft(targetRectangle), GetTopLeft(candidateRectangle));
//   for (int ix = 0; ix < xyBounds.xBounds.nbBounds; ix++)
//   {
//     for (int iy = 0; iy < xyBounds.yBounds.nbBounds; iy++)
//     {
//       Bound yBound = xyBounds.yBounds.bounds[iy];
//       Bound xBound = xyBounds.xBounds.bounds[ix];
//       if (yBound.collisionType == FROM_ABOVE)
//       {
//         SetBottom(&candidateRectangle, yBound.value);
//       }
//       else
//       {
//         SetTop(&candidateRectangle, yBound.value);
//       }

//       if (xBound.collisionType == FROM_LEFT)
//       {
//         SetRight(&candidateRectangle, xBound.value);
//       }
//       else
//       {
//         SetLeft(&candidateRectangle, xBound.value);
//       }

//       float candidateDist = Vector2DistanceSqr(GetTopLeft(targetRectangle), GetTopLeft(candidateRectangle));

//       if (candidateDist < bestDist)
//       {
//         bool noCollision = true;

//         Rectangle *otherRectangle;
//         world->reinit(world);
//         while (otherRectangle = world->next(world) != NULL)
//         {
//           if (StrictlyCollide(*otherRectangle, candidateRectangle))
//           {
//             noCollision = false;
//             break;
//           }
//         }
//         if (noCollision)
//         {
//           bestDist = candidateDist;
//           bestRectangle = candidateRectangle;
//         }
//       }
//     }
//   }
//   SetTopLeft(movingRectangle, GetTopLeft(bestRectangle));
//   return true;
// }