#pragma once
#include "Vec2.h"
#include "Colors.h"
#include "Graphics.h"
#include <map>
#include "Surface.h"
#include "Animation.h"
#include <string>
#include <map>
#include "Effect.h"
#include "Spell.h"

using namespace std;

enum ItemParameters {
	equipSlot,
	vitality, castSpeedItem, agility, moveSpeed,
	luck,
	isAttracting,
	isRam, isFox, isRebound, isDash, isParry,
	intellect, strength, chaosLevel,
	spellTarget, effectTarget, effectTime, itemType, needsShrine,
	//scuffed skill tree
	st_projectilesSpeedMultiplier, st_reboundProjectiles,
	st_extraShadowBallProjectiles, st_isReaping, st_dormantUpgrade,
	maxEffectStacks,
	ss_speed, ss_damage, ss_nProjectiles, ss_main,
	ss_reboundCount, ss_accuracy, ss_timeActive,
	ss_castSpeed
};

enum StatMod {
	//modifiers
	//general
	castTime_mod, cooldown_mod,
	//magical
	manaDrain_mod, projectileSpeed_mod, damageMagical_mod,
	//physical
	staminaDrain_mod, coneAngle_mod, coneRadius_mod
};

enum ItemName {
	item_dashEnable, item_grenade, item_scatter, item_boomerang, item_claw,
	item_unknown, item_godmode, item_explosiveDash
};

enum ItemTypes {
	unknownType, wheaponType, armorType, usableType, perkType
};

enum EquipSlot {
	hand, ringSlot, neckSlot, essenceSlot, 
	head, chest, legs, hands, feet, use,
	ss_equipSlot
};

class Item {
public:
	
	//static Item GetRandom(int level, std::vector<ItemParameters> itemStats);
	//static Item GetSpellItem(enum SpellName spellN_in);
	//static Item GetSoulItem(enum SpellName spellN_in, Inventory * inv_in = nullptr, class Player * player_in = nullptr, int level = 1);
	//static Item GetEffectItem(enum EffectType effectT_in, float timeLeft_in);
};       
class SoulItem {
private:
	Spell spell; //TODO: fix memory leaks
	std::map<SpellParameters, float> data;
public:
	SoulItem(SpellName spell_in);
	SoulItem();
	Spell& GetSpell();
};


class MinorItem {
private:
	std::string name = "--name_missing--";
	std::string desc = "--desc_missing--";
public:
	ItemName id = ItemName::item_unknown;
	std::map < SpellParameters, float > updates;
	std::map < SpellParameters, float > modulations;
public:
	MinorItem(std::map < SpellParameters, float > updates_in,
		std::map < SpellParameters, float > modulations_in);
	MinorItem(ItemName itemName_in);
	float GetUpdate(SpellParameters spellP_in);
	float GetModulation(SpellParameters spellP_in);
	std::string GetName();
	std::string GetDesc();
	static Color GetParamterColor(ItemParameters itemP_in);
	static vector<string> GetParameterText(ItemParameters itemP_in);
};