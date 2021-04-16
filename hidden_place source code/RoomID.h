#pragma once
#include "Entity.h"
#include "Vec2.h"
#include "Graphics.h"
#include "Colors.h"
#include <random>
#include "Surface.h"

class RoomID{
public:
	Color c;
	Vec2 vec;
	Surface surf;
	int width = 100, height = 100;
	bool isCreated = false;
public:
	void Load(int root, class Room & room);
	void Draw(Graphics &gfx, Vec2 roomPos);
	RoomID();
};