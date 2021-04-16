#pragma once
#include "Entity.h"
#include "Surface.h"
#include <string>
#include "Inventory.h"
#include <vector>
#include "Item.h"
#include <map>
#include "Animation.h"

class Chest : public Entity {
public:
	Inventory inv;
	Animation a;
	Animation aBase;
	Color coat = Colors::Red;
	float coatOpacity = 0.5;
public:
	/*Chest(Vec2 vec_in, std::string surf_in, std::string surfBase_in,
		std::string surfMask_in, std::string biome_in, std::vector<Item> items_in);
	void Open();
	void Close();
	void Update(class Player &hero);
	void Draw(Vec2 &pos_in, Font &font, Graphics &gfx);*/
};