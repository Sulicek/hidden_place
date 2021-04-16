#include "Capsule.h"

void Capsule::Draw(Vec2 pos, Graphics & gfx)
{
	gfx.DrawCircle(vec1 + pos, r, c);
	gfx.DrawCircle(vec2 + pos, r, c);
	Vec2 n = Vec2::GetDeltaVecNorm(vec1, vec2).GetNormal();
	gfx.DrawVec(vec1 + (n*r) + pos, vec2 + (n*r) + pos, c);
	gfx.DrawVec(vec1 - (n*r) + pos, vec2 - (n*r) + pos, c);
	for (float i = 0; i < r * 2; i += 1) {
		gfx.DrawVec(vec1 + (n*(r - i)) + pos, vec2 + (n*(r - i)) + pos, c);
	}
}

Capsule::Capsule(Vec2 vec1_in, Vec2 vec2_in, float r_in, Color c_in) 
:
vec1(vec1_in),
vec2(vec2_in),
r(r_in),
c(c_in)
{
}