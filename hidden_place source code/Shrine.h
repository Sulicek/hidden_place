#pragma once
#include "Entity.h"
#include "Vec2.h"
#include <map>
#include <string>

class Shrine : public Entity {
private:
	std::string surf;
	std::string maskSurf;
	Color innerC = Colors::Red;
public:
	Shrine(Vec2 vec_in);
	void Update(class Player &  player_in, std::vector<class Enemy> &enemies);
	void Draw(Vec2 & pos, std::map<std::string, Surface*> *sm, Graphics & gfx);
};