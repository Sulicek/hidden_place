#pragma once
#include "Graphics.h"
#include "Vec2.h"
#include "Colors.h"
#include <map>
#include <string>

enum EffectType {
	canChangeSpells,
	ignite, burning, empowerPyroblast, soul, reaping, demon, manaShield, dash, invulnerable,
	healing, attackBoost, root
};

enum EffectParameters {
	timeLeft, damageP, maxStacks, progress, 
	maxProgress, velX, velY, defaultMaxTime, 
	healingOverTime, targetX, targetY, targetDist
};
class Effect {
public:
	//string having the name and maybe even a short description
	std::string desc = "*Missing desc*";
	//ammount of stacks
	int stacks = 1;
	//wether we can remove it at will or not
	bool isRemovable;
	//wether it stacks or not
	bool isStackable;
	//does it run out of time or not
	bool isPermanent;
	//effect type or name if you will
	EffectType type;
	//data for the effects
	std::map < EffectParameters, float > data;
	//for drawing purposes only
	int size = 10;
	//for drawing purposes only
	Color c = Colors::LightOrange;
	//constructor
	Effect(EffectType eType_in,
		int timeLeft_in, float damage_in);
	//here we draw the effect indication somewhere on the screen (just a square lol)
	void Draw(Vec2 vec_in, Graphics &gfx);
	float* GetDataDirect(EffectParameters effectP_in);
	float GetData(EffectParameters effectP_in);
	Effect() = default;
};