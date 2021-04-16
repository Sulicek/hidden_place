#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include "Colors.h"

class Circle {
private:
	int r;
	Vec2 vec;
	Color c;
public:
	Circle(Vec2 vec_in, int r_in, Color c_in) {
		r = r_in;
		vec = vec_in;
		c = c_in;
	}
	Circle(Vec2 vec_in, int r_in) {
		r = r_in;
		vec = vec_in;
		c = Colors::Magenta;
	}

	void Draw(Vec2 pos, Graphics &gfx);
};