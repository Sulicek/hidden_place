#include "Rect.h"

Rect Rect::operator+(Vec2 & rhs)
{
	return Rect(offsetVec + rhs, width, height);
}
