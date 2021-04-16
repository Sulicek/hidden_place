#include "Item.h"
#include "Spell.h"
#include "Effect.h"
#include <random>
#include <time.h>
#include "Surface.h"
#include "Inventory.h"
#include <algorithm>
#include <string>
#include "Spell.h"
#include "Player.h"

using namespace std;

SoulItem::SoulItem(SpellName spell_in):
	spell(SpellName(spell_in))
{
}

SoulItem::SoulItem():
	spell(SpellName(0))
{
}

Spell & SoulItem::GetSpell()
{
	return spell;
}

MinorItem::MinorItem(std::map<SpellParameters, float> updates_in,
	std::map < SpellParameters, float > modulations_in)
{
	updates = updates_in;
	modulations = modulations_in;
	desc = "";
	for (auto i : updates) {
		desc += to_string(int(i.first));
		desc += ", ";
	}
}

MinorItem::MinorItem(ItemName itemName_in)
{
	switch (itemName_in) {
	case ItemName::item_unknown:
		*this = MinorItem(
			{},
			{}
		);
		name = "Unknown item";
		desc = "please report to the game master";
		break;
	case ItemName::item_godmode:
		*this = MinorItem(
			{ {SpellParameters::nProjectiles, 10} },
			{}
		);
		name = "Item maturitni komise";
		desc = "enjoy :>";
		break;
	case ItemName::item_dashEnable:
		*this = MinorItem(
			{ {SpellParameters::canPlayerDash, 1} },
			{}
		);
		name = "Friend's blessing";
		desc = "enables the dash ability on [SPACE]";
		break;
	case ItemName::item_explosiveDash:
		*this = MinorItem(
			{ {SpellParameters::spellCastOnDash, int(SpellName::stored_dash_explode)} },
			{}
		);
		name = "Water pendant";
		desc = "release splashes upon dashing";
		break;
	case ItemName::item_scatter:
		*this = MinorItem({
				{ SpellParameters::storedSpellExpire, SpellName::stored_scatter },
				{ SpellParameters::rangeCap, 100 },
			}, {
				{SpellParameters::castSpeed, 0.7},
				{SpellParameters::damage, 0} });
		name = "Scattering soul";
		desc = "collected essence";
		break;
	case ItemName::item_grenade:
		*this = MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_explode },
		{SpellParameters::notDeleteOnHit, 3}
			}, {
				{SpellParameters::castSpeed, 0.7},
				{SpellParameters::damage, 0}
			});
		name = "Explosive soul";
		desc = "collected essence";
		break;
	case ItemName::item_boomerang:
		*this = MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_return },
		{ SpellParameters::addAmmoOnHeroHit, 1 },
		{SpellParameters::notDeleteOnHit, 3}
			}, {
				{SpellParameters::castSpeed, 0.8}
			});
		break;
	case ItemName::item_claw:
		*this = MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_claw },
		{ SpellParameters::rangeCap, 0.1}
			}, { });
		break;
	}
	id = itemName_in;
}

float MinorItem::GetUpdate(SpellParameters spellP_in)
{
	float output = 0;
	if (updates.count(spellP_in)) {
		output = updates[spellP_in];
	}
	return output;
}

float MinorItem::GetModulation(SpellParameters spellP_in)
{
	float output = 1;
	if (modulations.count(spellP_in)) {
		output = modulations[spellP_in];
	}
	return output;
}

std::string MinorItem::GetName()
{
	return name;
}

std::string MinorItem::GetDesc()
{
	return desc;
}

Color MinorItem::GetParamterColor(ItemParameters itemP_in)
{
	Color out;
	switch (itemP_in) {
	default:
		out = Colors::White;
		break;
	case ItemParameters::vitality:
		out = Colors::Red;
		break;
	case ItemParameters::castSpeedItem:
		out = Colors::Yellow;
		break;
	case ItemParameters::agility:
		out = Color(0, 200, 10);
		break;
	case ItemParameters::moveSpeed:
		out = Color(255, 0, 255);
		break;
	}
	return out;
}

std::vector<std::string> MinorItem::GetParameterText(ItemParameters itemP_in)
{
	vector<string> out;
	switch (itemP_in) {
	default:
		out.push_back("unkown item, please report to the game master");
		break;
	case ItemParameters::vitality:
		out.push_back("Your health");
		break;
	case ItemParameters::castSpeedItem:
		out.push_back("Speed with which you prepare");
		out.push_back("your attacks");
		break;
	case ItemParameters::agility:
		out.push_back("Speed with which your dash");
		out.push_back("replenishes");
		break;
	case ItemParameters::moveSpeed:
		out.push_back("Speed with which you move");
		out.push_back("through the world");
		break;
	}
	return out;
}
