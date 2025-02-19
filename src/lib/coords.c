#include "raylib.h"
#include "coords.h"



Vector2 GetTopLeft(Rectangle rect)
{
  return (Vector2){rect.x, rect.y};
}

Vector2 GetBottomRight(Rectangle rect)
{
  return (Vector2){rect.x + rect.width, rect.y + rect.height};
}

Vector2 GetBottomLeft(Rectangle rect)
{
  return (Vector2){rect.x, rect.y + rect.height};
}

Vector2 GetTopRight(Rectangle rect)
{
  return (Vector2){rect.x + rect.width, rect.y};
}

float GetTop(Rectangle rect)
{
  return rect.y;
}

void SetTop(Rectangle *rect, float top)
{
  rect->y = top;
}

float GetBottom(Rectangle rect)
{
  return rect.y + rect.height;
}

void SetBottom(Rectangle *rect, float bottom)
{
  rect->y = bottom - rect->height;
}

float GetLeft(Rectangle rect)
{
  return rect.x;
}

void SetLeft(Rectangle *rect, float left)
{
  rect->x = left;
}

float GetRight(Rectangle rect)
{
  return rect.x + rect.width;
}

void SetRight(Rectangle *rect, float right)
{
  rect->x = right - rect->width;
}

void SetTopLeft(Rectangle *rect, Vector2 newPos)
{
  rect->x = newPos.x;
  rect->y = newPos.y;
}


enum RelativeYPosition RelativeYPosition(Rectangle rect1, Rectangle rect2)
{
  if (GetBottom(rect1) < GetTop(rect2))
  {
    return STRICTLY_ABOVE;
  }
  else if (GetBottom(rect1) == GetTop(rect2))
  {
    return JUST_ABOVE;
  }
  else if (GetTop(rect1) > GetBottom(rect2))
  {
    return STRICTLY_BELOW;
  }
  else if (GetTop(rect1) == GetBottom(rect2))
  {
    return JUST_BELOW;
  }
  else
  {
    return Y_ALIGNED;
  }
}

enum RelativeXPosition RelativeXPosition(Rectangle rect1, Rectangle rect2)
{
  if (GetRight(rect1) < GetLeft(rect2))
  {
    return STRICTLY_LEFT;
  }
  else if (GetRight(rect1) == GetLeft(rect2))
  {
    return JUST_LEFT;
  }
  else if (GetLeft(rect1) > GetRight(rect2))
  {
    return STRICTLY_RIGHT;
  }
  else if (GetLeft(rect1) == GetRight(rect2))
  {
    return JUST_RIGHT;
  }
  else
  {
    return X_ALIGNED;
  }
}