#pragma once
#include "Graphics.h"
#include "Vec2.h"
#include "Spell.h"
#include "Colors.h"

class Cone {
public:
	Vec2 vec, p1, p2, spike;
	Color c;
	Spell *spell;
public:
	bool isActive = true;
	bool isFading = false;
	int fadingTimer = 5;
	Cone(Vec2 vec_in, Vec2 edge_in, Spell &spell_in);
	int GetRadius();
	Vec2 GetVec();
	void Draw(Vec2 pos, Graphics &gfx);
	void Update(std::vector<class Enemy> &enemies, class Player &hero, class Room & room, bool isParry = false);
	void Update(Player &hero, Enemy & enem, Room & room);
};