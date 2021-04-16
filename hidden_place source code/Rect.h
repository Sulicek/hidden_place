#pragma once
#include "Vec2.h"

class Rect {
public:
	Vec2 offsetVec;
	int width, height;
public:
	Rect(Vec2 offsetVec_in, int width_in, int height_in) {
		offsetVec = offsetVec_in;
		width = width_in;
		height = height_in;
	};

	Rect operator+(Vec2 &rhs);
};