#include "Vec2.h"
#include "Graphics.h"
#include "Colors.h"

class Capsule {
public:
	Vec2 vec1, vec2;
	float r;
	Color c;
public:
	void Draw(Vec2 pos, Graphics &gfx);
	Capsule(Vec2 vec1_in, Vec2 vec2_in, float r_in, Color c_in);
};