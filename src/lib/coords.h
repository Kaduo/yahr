#include "raylib.h"


enum RelativeYPosition
{
  STRICTLY_ABOVE = 0x01,
  STRICTLY_BELOW = 0x02,
  JUST_ABOVE = 0x04,
  JUST_BELOW = 0x08,
  Y_ALIGNED = 0x10
};

enum RelativeXPosition
{
  STRICTLY_LEFT = 0x01,
  STRICTLY_RIGHT = 0x02,
  JUST_LEFT = 0x04,
  JUST_RIGHT = 0x08,
  X_ALIGNED = 0x10
};

Vector2 GetTopLeft(Rectangle rect);
Vector2 GetBottomRight(Rectangle rect);

Vector2 GetBottomLeft(Rectangle rect);
Vector2 GetTopRight(Rectangle rect);
float GetTop(Rectangle rect);

void SetTop(Rectangle *rect, float top);

float GetBottom(Rectangle rect);

void SetBottom(Rectangle *rect, float bottom);
float GetLeft(Rectangle rect);

void SetLeft(Rectangle *rect, float left);

float GetRight(Rectangle rect);

void SetRight(Rectangle *rect, float right);

void SetTopLeft(Rectangle *rect, Vector2 newPos);


enum RelativeYPosition RelativeYPosition(Rectangle rect1, Rectangle rect2);
enum RelativeXPosition RelativeXPosition(Rectangle rect1, Rectangle rect2);

