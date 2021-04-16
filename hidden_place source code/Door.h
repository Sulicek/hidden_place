#pragma once
#include "Entity.h"
#include "Graphics.h"
#include "Colors.h"
#include "Vec2.h"
#include <string>
#include "Animation.h"
#include <map>
#include "Surface.h"
#include "Light.h"

enum DoorType {
	top, bottom, left, right
};

using namespace std;

class Door :public Entity {
public:
	int rnChange = -1;
	int type;
	int size;
	std::string nextLoad;
	Animation a;
	Color coat;
	Light light;
	float coatOpacity;
	Door(int type_in, int rnChange_in, 
		class Room & room, std::string nextLoad_in = "none");
	int Update(class Player &hero);
	void Draw(class Font &font, Graphics & gfx, Vec2 pos);
};