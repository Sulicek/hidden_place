#include "Circle.h"

void Circle::Draw(Vec2 pos, Graphics & gfx) {
	gfx.DrawCircle(vec - pos, r, c);
}