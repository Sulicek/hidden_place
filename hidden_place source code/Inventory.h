#pragma once
#include "Vec2.h"
#include <vector>
#include "Item.h"
#include "Graphics.h"
#include <map>
#include <string>
#include "MainWindow.h"
#include "Font.h"
#include <set>

class Inventory {
public:
	enum InventoryType {
		it_player = 0,
		it_chest = 1,
		it_craft = 2
	};
	struct InventoryItem {
		int invId;
		Vec2 vec;
		int width, height;
	};
	//vector of items contained by the inventory
	std::map <int, MinorItem> items;
	int selected = 0;
	class Player *player;
	public:
	//constructor, basic one
	Inventory();
	//draw with specific parameters
	void Draw(Vec2 vec, int scale, int spacing, Font &font, Graphics &gfx);
	float GetData(float output_in, SpellParameters spellP_in);
	void AddItem(MinorItem  item_in);
};