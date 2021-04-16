#pragma once
#include "Spell.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Player.h"
#include "Cone.h"
#include <string>
#include "Inventory.h"
#include <windows.h>
#include <assert.h>
#include "Item.h"
#include "Parameters.h"
#include <typeindex>

//ignore this
#define M_PI 3.14159265358979323846

std::map<SpellName, Spell*> Spell::storedSpells = {};

//projectiles around player
Projectile* Spell::Cast(Vec2 direction_in, std::vector<Projectile>& projectiles, Vec2 heroVec_in, int nShots_in,
	bool isDormant_in, int distance) {
	if (nShots_in > 1) {
		float desiredAngle = float(2 * M_PI) / float(nShots_in);
		for (int i = 0; i < nShots_in; i++) {
			Vec2 direction = Vec2::GetVecByAngle(
				direction_in.GetAngle() +
				desiredAngle * i);
			projectiles.push_back(Projectile(*this, heroVec_in - direction * distance, direction, Parameters(), isDormant_in));
		}
	}
	else {
		projectiles.push_back(Projectile(*(new Spell(*this)), heroVec_in, direction_in, Parameters(), isDormant_in));
	}
	return &projectiles[projectiles.size() - 1];
}

//projectiles in a cone around the player
void Spell::Cast(Vec2 direction_in, std::vector<Projectile>& projectiles, Vec2 heroVec_in,
	int nShots_in, int angle_in, Parameters param, bool isDormant_in) {
	SoulItem* item = nullptr;
	if (nShots_in > 1) {
		float angle = (float(angle_in) * float(2) * M_PI) / float(360);//converting deg to rad
		float desiredAngle;
		if (angle_in == 360) {
			desiredAngle = angle /  float(nShots_in);
		}
		else {
			desiredAngle = angle / float(nShots_in - 1); //should be angle between individual shots
		}
		for (int i = 0; i < nShots_in; i++) {
			Vec2 direction = Vec2::GetVecByAngle(
				direction_in.GetAngle() +
				desiredAngle * i -
				angle / 2
			);
			projectiles.push_back(Projectile(*this, heroVec_in + direction * GetData(SpellParameters::startingOffset),
				direction, param, isDormant_in));
		}
	}
	else {
		projectiles.push_back(Projectile(*(new Spell(*this)),
			heroVec_in + direction_in * GetData(SpellParameters::startingOffset),
			direction_in, param, isDormant_in));
	}
}

//projectiles in a cone around the player soul item spell variant
void Spell::Cast(Vec2 direction_in, std::vector<Projectile>& projectiles, Vec2 heroVec_in,
	int nShots_in, int angle_in, SoulItem &item_in, bool isDormant_in) {
	float angle = (float(angle_in) * float(2) * M_PI) / float(360);//converting deg to rad
	float desiredAngle = angle / float(nShots_in); //should be angle between individual shots
	for (int i = 0; i < nShots_in; i++) {
		projectiles.push_back(Projectile(*this, heroVec_in, Vec2::GetVecByAngle(
			direction_in.GetAngle() +
			desiredAngle * i
		), Parameters().Set<SoulItem*>(&item_in), isDormant_in));
	}
}


/*Vec2 edge =
		Vec2(
			Vec2::GetVecByAngle(Vec2::GetDeltaVec(direction_in, heroVec_in).GetAngle() -
				(data[SpellParameters::coneAngle] * M_PI / 180) / float(2)).GetNormalized().x * data[SpellParameters::coneR],
			Vec2::GetVecByAngle(Vec2::GetDeltaVec(direction_in, heroVec_in).GetAngle() -
				(data[SpellParameters::coneAngle] * M_PI / 180) / float(2)).GetNormalized().y * data[SpellParameters::coneR]
		)
		+ heroVec_in;*/
void Spell::Cast(Vec2 & heroVec_in, Vec2 direction_in, std::vector<class Cone>& cones)
{
	Vec2 edge =
		Vec2(
			Vec2::GetVecByAngle(direction_in.GetAngle() -
			(GetData(SpellParameters::coneAngle) * M_PI / 180) / float(2)).GetNormalized().x * GetData(SpellParameters::coneR),
			Vec2::GetVecByAngle(direction_in.GetAngle() -
			(GetData(SpellParameters::coneAngle) * M_PI / 180) / float(2)).GetNormalized().y * GetData(SpellParameters::coneR)
		)
		+ heroVec_in;

	cones.push_back(Cone(heroVec_in, edge, *this));
}

Spell::Spell(SpellName type_in) :
	surf("Resources\\Sprites\\kule.bmp"),
	castBar(Vec2(0, 0), c, Colors::BackgroundGrey, size, size),
	cdBar(Vec2(0, 0), c, Colors::BackgroundGrey, size, size)
{


	type = type_in;
	data[SpellParameters::speed] = 1;
	switch (type_in) {
	case 0://gcd
		c = Colors::Gray;
		data[SpellParameters::cooldown] = 30;
		break;
	case SpellName::debug:
		c = Colors::Magenta;
		data[SpellParameters::castTime] = 10;
		desc = "If you somehow got access to this, ignore it";
		castType = SpellCastType::Aim;
		break;
		//Enemy abilities
	case SpellName::dashEnemy:
		c = Color(196, 171, 59);
		castType = SpellCastType::Aim;
		data[SpellParameters::cooldown] = 200;
		data[SpellParameters::castTime] = 10;
		data[SpellParameters::dashDistance] = 100;
		data[SpellParameters::dashLength] = 10;
		data[SpellParameters::range] = 400;
		break;
	case SpellName::leapEnemy:
		c = Color(196, 171, 59);
		castType = SpellCastType::Aim;
		data[SpellParameters::cooldown] = 200;
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::dashDistance] = 100;
		data[SpellParameters::dashLength] = 10;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		break;
	case SpellName::retreatEnemy:
		c = Color(196, 171, 59);
		castType = SpellCastType::Aim;
		data[SpellParameters::cooldown] = 200;
		data[SpellParameters::castTime] = 10;
		data[SpellParameters::dashDistance] = 100;
		data[SpellParameters::dashLength] = 10;
		data[SpellParameters::range] = 400;
		break;
	case SpellName::meleeBasicAttack:
		c = Color(2, 217, 255);
		data[SpellParameters::castTime] = 50;
		data[SpellParameters::cooldown] = 100;
		size = 10;
		data[SpellParameters::damage] = 65;
		data[SpellParameters::coneR] = 150;
		data[SpellParameters::range] = 100;
		data[SpellParameters::coneAngle] = 30;
		castType = SpellCastType::Angle;
		data[SpellParameters::enemyCastType] = EnemyCastType::notTracking;
		break;
	case SpellName::meleeBossAttack:
		c = Color(2, 217, 255);
		data[SpellParameters::castTime] = 50;
		data[SpellParameters::cooldown] = 0;
		size = 10;
		data[SpellParameters::damage] = 65;
		data[SpellParameters::coneR] = 300;
		data[SpellParameters::range] = 200;
		data[SpellParameters::coneAngle] = 30;
		castType = SpellCastType::Angle;
		data[SpellParameters::enemyCastType] = EnemyCastType::notTracking;
		break;
	case SpellName::meleeWolfEnemy:
		c = Color(2, 217, 255);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::cooldown] = 450;
		size = 10;
		data[SpellParameters::damage] = 65;
		data[SpellParameters::coneR] = 350;
		data[SpellParameters::range] = 300;
		data[SpellParameters::coneAngle] = 10;
		castType = SpellCastType::Angle;
		data[SpellParameters::enemyCastType] = EnemyCastType::notTracking;
		break;
	case SpellName::rangedWolfEnemy:
		c = Color(211, 0, 200);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::cooldown] = 450;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 5;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 270;
		data[SpellParameters::reboundCount] = 2;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_chort_ritual:
		c = Color(211, 0, 255);
		data[SpellParameters::castTime] = 15;
		data[SpellParameters::cooldown] = 450;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 10;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 360;

		isChannel = true;
		data[SpellParameters::channelLength] = 15;
		data[SpellParameters::channelTicks] = 3;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_blast:
		c = Color(255, 140, 0);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		isChannel = true;
		data[SpellParameters::channelLength] = 60;
		data[SpellParameters::channelTicks] = 3;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_chort_blast:
		c = Color(211, 0, 255);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		isChannel = true;
		data[SpellParameters::channelLength] = 60;
		data[SpellParameters::channelTicks] = 3;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_ranged:
		c = Colors::Red;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::coneAngle] = 60;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1500;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_snipe:
		c = Colors::Red;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::castTime] = 120;
		data[SpellParameters::rangeCap] = 600;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1500;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_root:
		c = Colors::Green;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::castTime] = 120;
		data[SpellParameters::rangeCap] = 600;
		data[SpellParameters::speed] = 10;
		size = 30;
		data[SpellParameters::damage] = 0;
		data[SpellParameters::range] = 1500;
		data[SpellParameters::isRoot] = 1;
		data[SpellParameters::rootTime] = 60;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_rangedChannel:
		c = Colors::Red;
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1500;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_rebound:
		c = Colors::LightOrange;
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 2;
		data[SpellParameters::nProjectiles] = 1;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 4000;
		data[SpellParameters::reboundCount] = 3;
		data[SpellParameters::coneAngle] = 60;
		data[SpellParameters::oppositeDirection] = 1;
		data[SpellParameters::randomDirectionAngle] = 60;

		isChannel = true;
		data[SpellParameters::channelLength] = 30;
		data[SpellParameters::channelTicks] = 1;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_vomit:
		c = Colors::Yellow;
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::speed] = 3;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::rangeCap] = 300;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 4000;
		data[SpellParameters::randomDirectionAngle] = 90;

		isChannel = true;
		data[SpellParameters::channelLength] = 30;
		data[SpellParameters::channelTicks] = 5;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_grenade:
		c = Colors::DarkOrange;
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 70;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1400;

		data[SpellParameters::storedSpellExpire] = (int)SpellName::en_stored_explode;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_stored_explode:
		isSoulSpell = false;
		c = Color(255, 0, 0);
		data[SpellParameters::nProjectiles] = 5;
		data[SpellParameters::coneAngle] = 360;
		data[SpellParameters::speed] = 3;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 100;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored hate rebound spell";
		surf = "projectile";
		break;
	case SpellName::en_tree_s:
		c = Colors::Red;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::cooldown] = 80;
		data[SpellParameters::rangeCap] = 600;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1500;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectsile";
		break;
	case SpellName::en_tree_v:
		c = Colors::Red;
		data[SpellParameters::nProjectiles] = 2;
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::cooldown] = 80;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::coneAngle] = 50;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1500;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_tree_rebound:
		c = Colors::LightOrange;
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 2;
		data[SpellParameters::nProjectiles] = 1;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 4000;
		data[SpellParameters::reboundCount] = 2;
		data[SpellParameters::coneAngle] = 60;
		data[SpellParameters::oppositeDirection] = 1;
		data[SpellParameters::randomDirectionAngle] = 60;

		isChannel = true;
		data[SpellParameters::channelLength] = 30;
		data[SpellParameters::channelTicks] = 10;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_fountain_grenade:
		c = Color(0, 213, 255);
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 1400;
		data[SpellParameters::coneAngle] = 40;
		data[SpellParameters::nProjectiles] = 2;

		data[SpellParameters::storedSpellExpire] = (int)SpellName::en_fountain_stored_explode;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_fountain_stored_explode:
		c = Color(0, 213, 255);
		data[SpellParameters::nProjectiles] = 5;
		data[SpellParameters::coneAngle] = 360;
		data[SpellParameters::speed] = 3;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 100;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored hate rebound spell";
		surf = "projectile";
		break;
	case SpellName::en_fountain_vomit:
		c = Color(0, 213, 255);
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::speed] = 3;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::rangeCap] = 400;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 4000;
		data[SpellParameters::randomDirectionAngle] = 90;

		isChannel = true;
		data[SpellParameters::channelLength] = 60;
		data[SpellParameters::channelTicks] = 10;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_fountain_ritual:
		c = Color(211, 0, 255);
		data[SpellParameters::castTime] = 40;
		//data[SpellParameters::cooldown] = 160;
		data[SpellParameters::nProjectiles] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::speed] = 4;
		data[SpellParameters::range] = 700;
		size = 30;

		isChannel = true;
		data[SpellParameters::channelLength] = 60;
		data[SpellParameters::channelTicks] = 3;

		castType = SpellCastType::Aim;
		surf = "projectile";
		desc = "Embrace Darkness\ndarken your body,\ngenerating dark\npower";
		break;
	case SpellName::en_hammers_blast:
		c = Color(255, 140, 0);
		data[SpellParameters::castTime] = 10;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 2;
		data[SpellParameters::rangeCap] = 1;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		data[SpellParameters::storedSpellExpire] = (int)SpellName::en_hammers_stored_blast;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_hammers_spray:
		c = Color(255, 140, 0);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		isChannel = true;
		data[SpellParameters::channelLength] = 60;
		data[SpellParameters::channelTicks] = 6;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_hammers_stored_blast:
		c = Color(255, 140, 0);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 60;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_hammers_void:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 120;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::rangeCap] = 500;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		data[SpellParameters::isGhost] = 1;
		data[SpellParameters::storedSpellExpire] = (int)SpellName::en_hammers_stored_void;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_hammers_stored_void:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 16;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::rangeCap] = 350;
		data[SpellParameters::oppositeDirection] = 1;
		data[SpellParameters::isGhost] = 1;
		data[SpellParameters::storedSpellExpire] = (int)SpellName::en_hammers_stored_void;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_sprayLock:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::cooldown] = 0;
		data[SpellParameters::speed] = 6;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 2;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 40;
		isChannel = true;
		data[SpellParameters::channelLength] = 180;
		data[SpellParameters::channelTicks] = 36;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_sprayLockShort:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::cooldown] = 0;
		data[SpellParameters::speed] = 10;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 2;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		isChannel = true;
		data[SpellParameters::channelLength] = 90;
		data[SpellParameters::channelTicks] = 18;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_spraySnipe:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::cooldown] = 0;
		data[SpellParameters::speed] = 5;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		isChannel = true;
		data[SpellParameters::channelLength] = 180;
		data[SpellParameters::channelTicks] = 18;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_explode:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::speed] = 4;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 9;
		data[SpellParameters::rangeCap] = 700;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 180;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_jail:
		c = Color(100, 100, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::speed] = 0;
		size = 30;
		data[SpellParameters::damage] = 0;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::storedSpellHit] = int(SpellName::en_heart_stored_jail);
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_stored_jail:
		c = Color(127, 0, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 5;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		data[SpellParameters::distanceFromCaster] = 300;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::isGhost] = 1;
		data[SpellParameters::directionShift] = 1;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_voidTrigger:
		c = Color(100, 100, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::speed] = 0;
		size = 30;
		data[SpellParameters::damage] = 0;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::storedSpellHit] = int(SpellName::en_heart_stored_void);
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_stored_void:
		c = Color(127, 0, 255);
		data[SpellParameters::castTime] = 1;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 5;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::coneAngle] = 30;
		data[SpellParameters::distanceFromCaster] = 300;
		data[SpellParameters::rangeCap] = 300;
		data[SpellParameters::isGhost] = 1;
		data[SpellParameters::directionShift] = 1;
		data[SpellParameters::storedSpellExpire] = int(SpellName::en_heart_void);
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_void:
		c = Color(255, 0, 255);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::cooldown] = 160;
		data[SpellParameters::speed] = 10;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::rangeCap] = 50;
		data[SpellParameters::oppositeDirection] = 1;
		data[SpellParameters::isGhost] = 1;
		data[SpellParameters::storedSpellExpire] = (int)SpellName::en_heart_void;
		data[SpellParameters::range] = 1000;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_heart_vomit:
		c = Color(158, 0, 76);
		data[SpellParameters::castTime] = 80;
		data[SpellParameters::speed] = 8;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::rangeCap] = 1000;
		size = 30;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::range] = 4000;
		data[SpellParameters::randomDirectionAngle] = 180;

		isChannel = true;
		data[SpellParameters::channelLength] = 180;
		data[SpellParameters::channelTicks] = 36;

		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::rangedBossAttack:
		//fjostjijke
		c = Color(2, 217, 255);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::cooldown] = 200;
		data[SpellParameters::speed] = 3;
		size = 30;
		data[SpellParameters::damage] = 30;
		data[SpellParameters::range] = 400;
		data[SpellParameters::enemyCastType] = EnemyCastType::tracking;
		castType = SpellCastType::Aim;
		surf = "projectile";
		break;
	case SpellName::en_ritual:
		c = Color(211, 0, 255);
		data[SpellParameters::castTime] = 80;
		//data[SpellParameters::cooldown] = 160;
		data[SpellParameters::nProjectiles] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::speed] = 4;
		data[SpellParameters::range] = 700;
		size = 30;
		castType = SpellCastType::Aim;
		surf = "projectile";
		desc = "Embrace Darkness\ndarken your body,\ngenerating dark\npower";
		break;
	case SpellName::en_call:
		c = Color(255, 0, 204);
		data[SpellParameters::castTime] = 80;
		//data[SpellParameters::cooldown] = 160;
		data[SpellParameters::nProjectiles] = 8;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::speed] = 4;
		data[SpellParameters::range] = 700;
		data[SpellParameters::isGhost] = 1;
		data[SpellParameters::distanceFromCaster] = 600;
		size = 30;
		castType = SpellCastType::Aim;
		surf = "projectile";
		desc = "Embrace Darkness\ndarken your body,\ngenerating dark\npower";
		break;
	case SpellName::en_trap:
		c = Color(53, 0, 64);
		data[SpellParameters::castTime] = 80;
		//data[SpellParameters::cooldown] = 160;
		data[SpellParameters::nProjectiles] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::speed] = 4;
		data[SpellParameters::range] = 100;
		isChannel = true;
		data[SpellParameters::channelLength] = 60;
		data[SpellParameters::channelTicks] = 10;
		size = 30;
		castType = SpellCastType::Aim;
		surf = "projectile";
		desc = "Embrace Darkness\ndarken your body,\ngenerating dark\npower";
		break;
	case SpellName::explodeEnemy:
		c = Color(53, 0, 64);
		data[SpellParameters::castTime] = 50;
		data[SpellParameters::nProjectiles] = 20;
		data[SpellParameters::damage] = 20;
		data[SpellParameters::cooldown] = 1000;
		data[SpellParameters::speed] = 4;
		data[SpellParameters::range] = 40;
		size = 30;
		castType = SpellCastType::Aim;
		surf = "projectile";
		desc = "Embrace Darkness\ndarken your body,\ngenerating dark\npower";
		break;
		//blade slinger
	case SpellName::quickStrike:
		c = Color(0, 214, 54);
		isScaleable = true;
		data[SpellParameters::requiresWheapon] = 1;
		data[SpellParameters::castTime] = 10;
		data[SpellParameters::cooldown] = 80;
		size = 10;
		data[SpellParameters::damage] = 15;
		data[SpellParameters::coneR] = 150;
		data[SpellParameters::coneAngle] = 5;
		castType = SpellCastType::Angle;
		data[SpellParameters::resourceCostStamina] = 35;
		desc = "Quick strike";
		break;
	case SpellName::sweep:
		c = Color(247, 165, 0);
		isScaleable = true;
		data[SpellParameters::isPhysical] = 1;
		data[SpellParameters::requiresWheapon] = 1;
		data[SpellParameters::castTime] = 20;
		//data[SpellParameters::cooldown] = 40;
		size = 10;
		data[SpellParameters::damage] = 20;
		data[SpellParameters::coneR] = 120;
		data[SpellParameters::coneAngle] = 90;
		castType = SpellCastType::Angle;
		data[SpellParameters::resourceCostStamina] = 70;
		desc = "Sweeping strike";
		break;
	case SpellName::shockwave:
		c = Color(247, 74, 0);
		data[SpellParameters::speed] = 5;
		data[SpellParameters::damage] = 7;
		data[SpellParameters::nProjectiles] = 12;
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::cooldown] = 80;
		data[SpellParameters::resourceCostStamina] = 70;
		size = 30;
		castType = SpellCastType::Aim;
		surf = "projectile";
		desc = "Shockwave";
		break;
	case SpellName::quickDash:
		c = Color(196, 171, 59);
		data[SpellParameters::resourceCostStamina] = 100;
		//data[SpellParameters::cooldown] = 160;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Quick Dash";
		break;
		//pyromaniac
	case SpellName::fireball:
		c = Color(244, 150, 0);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::resourceCostMana] = 70;
		//data[SpellParameters::cooldown] = 180;
		data[SpellParameters::speed] = 6;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::damage] = 10;
		data[SpellParameters::isChaos] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Fireball";
		surf = "projectile";
		break;
	case SpellName::heatUp:
		c = Colors::Red;
		data[SpellParameters::castTime] = 60;
		data[SpellParameters::nIgniteAdded] = 1;
		data[SpellParameters::resourceCostMana] = 200;
		//data[SpellParameters::cooldown] = 600;
		castType = SpellCastType::Aim;
		desc = "Heating Up";
		break;
	case SpellName::fireNova:
		c = Color(244, 100, 20);
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::resourceCostMana] = 200;
		//data[SpellParameters::cooldown] = 300;
		data[SpellParameters::speed] = 6;
		data[SpellParameters::nProjectiles] = 12;
		data[SpellParameters::damage] = 10;
		data[SpellParameters::isChaos] = 1;
		data[SpellParameters::nIgniteRequired] = 2;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Fire Nova";
		surf = "projectile";
		break;
	case SpellName::callOfFire:
		c = Color(244, 100, 20);
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::resourceCostMana] = 70;
		//data[SpellParameters::cooldown] = 150;
		data[SpellParameters::speed] = 9;
		castType = SpellCastType::Aim;
		desc = "Call of Fire";
		break;
	case SpellName::flameTap:
		c = Color(200, 100, 20);
		data[SpellParameters::nIgniteRequired] = 1;
		data[SpellParameters::resourceCostMana] = 150;
		data[SpellParameters::castTime] = 60;
		data[SpellParameters::healingDone] = 30;
		castType = SpellCastType::Aim;
		desc = "Flame Tap";
		break;
		//shadow mage
	case SpellName::shadowball:
		c = Color(61, 0, 92);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::resourceCostMana] = 0;
		//data[SpellParameters::cooldown] = 180;
		data[SpellParameters::speed] = 6;
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::damage] = 10;
		data[SpellParameters::isChaos] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Fireball";
		surf = "projectile";
		break;
	case SpellName::healPlayer:
		c = Color(255, 0, 0);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::resourceCostMana] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Healing spell";
		surf = "projectile";
		break;
	case SpellName::shadowNova:
		c = Color(0, 117, 115);
		data[SpellParameters::castTime] = 60;
		data[SpellParameters::resourceCostMana] = 35;
		data[SpellParameters::cooldown] = 300;
		data[SpellParameters::speed] = 3;
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::damage] = 5;
		data[SpellParameters::isShadow] = 1;
		data[SpellParameters::nSoulsRequired] = 1;
		data[SpellParameters::playerAttraction] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "ShadowNova";
		surf = "projectile";
		break;
	case SpellName::commandShadow:
		c = Color(110, 2, 56);
		data[SpellParameters::castTime] = 40;
		data[SpellParameters::resourceCostMana] = 10;
		data[SpellParameters::cooldown] = 150;
		data[SpellParameters::speed] = 9;
		castType = SpellCastType::Aim;
		desc = "Command Shadow";
		break;
	case SpellName::reap:
		c = Color(77, 0, 125);
		isScaleable = true;
		data[SpellParameters::castTime] = 60;
		data[SpellParameters::cooldown] = 100;
		data[SpellParameters::resourceCostMana] = 50;
		data[SpellParameters::resourceCostStamina] = 150;
		data[SpellParameters::damage] = 65;
		data[SpellParameters::coneR] = 150;
		data[SpellParameters::range] = 100;
		data[SpellParameters::coneAngle] = 90;
		data[SpellParameters::nSoulsAdded] = 1;
		castType = SpellCastType::Angle;
		desc = "Reap";
		break;
		//soul spells
	case SpellName::ss_empathy:
		isSoulSpell = true;
		c = Color(3, 163, 22);
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::resourceCostMana] = 35;
		data[SpellParameters::cooldown] = 300;
		data[SpellParameters::castSpeed] = 1;

		data[SpellParameters::ss_mod_speed] = 1;
		data[SpellParameters::ss_mod_damage] = 1;
		data[SpellParameters::ss_mod_nProjectiles] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Empathy\nHealing abilities in this\nworld are truly an exception";
		surf = "projectile";
		break;
	case SpellName::ss_ritual:
		isSoulSpell = true;
		c = Color(53, 0, 64);;
		data[SpellParameters::castTime] = 30;
		data[SpellParameters::resourceCostMana] = 35;
		data[SpellParameters::cooldown] = 600;
		data[SpellParameters::castSpeed] = 1;
		data[SpellParameters::isBaited] = 1;

		data[SpellParameters::ss_mod_speed] = 2;
		data[SpellParameters::ss_mod_damage] = 1;
		data[SpellParameters::ss_mod_nProjectiles] = 3;
		data[SpellParameters::ss_independentDormant] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "Ritual\nInfested with erupting dark magic\nit doesn't stay in it's dormant form for long";
		surf = "projectile";
		break;
	case SpellName::stored_guilt:
		isSoulSpell = false;
		c = Color(178, 199, 42);
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::coneAngle] = 30;
		data[SpellParameters::speed] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 200;

		data[SpellParameters::storedSpellRebound] = -1;

		/*data[SpellParameters::notDeleteOnHit] = 1;
		data[SpellParameters::returnOnHit] = 1;
		data[SpellParameters::returnOnHeroHit] = 1;
		data[SpellParameters::returnCount] = 5;*/

		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored spell of guilt";
		surf = "projectile";
		break;
	case SpellName::stored_claw:
		isSoulSpell = false;
		c = Color(0, 223, 247);
		data[SpellParameters::nProjectiles] = 2;
		data[SpellParameters::coneAngle] = 45;
		data[SpellParameters::speed] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 70;

		data[SpellParameters::startingOffset] = 70;
		data[SpellParameters::oppositeDirection] = 1;
		data[SpellParameters::notDeleteOnHit] = 3;

		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored spell of shotgun";
		surf = "projectile";
		break;
	case SpellName::stored_scatter:
		isSoulSpell = false;
		c = Color(255, 255, 255);
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::coneAngle] = 40;
		data[SpellParameters::speed] = 13;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 130;

		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored spell of shotgun";
		surf = "projectile";
		break;
	case SpellName::stored_return:
		c = Color(0, 255, 0);
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::coneAngle] = 70;
		data[SpellParameters::speed] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 200;

		data[SpellParameters::notDeleteOnHit] = 3;
		data[SpellParameters::oppositeDirection] = 1;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored hate rebound spell";
		surf = "projectile";
		break;
	case SpellName::stored_explode:
		isSoulSpell = false;
		c = Color(255, 255, 255);
		data[SpellParameters::nProjectiles] = 3;
		data[SpellParameters::coneAngle] = 360;
		data[SpellParameters::speed] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 100;
		data[SpellParameters::startingOffset] = 30;

		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored hate rebound spell";
		surf = "projectile";
		break;
	case SpellName::stored_dash_explode:
		isSoulSpell = false;
		c = Color(255, 255, 255);
		data[SpellParameters::nProjectiles] = 8;
		data[SpellParameters::coneAngle] = 360;
		data[SpellParameters::speed] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 500;
		data[SpellParameters::startingOffset] = 30;

		size = 30;
		castType = SpellCastType::Aim;
		desc = "stored hate rebound spell";
		surf = "projectile";
		break;
	case SpellName::ss_basic:
		isSoulSpell = true;
		c = Color(255, 255, 255);
		data[SpellParameters::castSpeed] = 0.7;
		data[SpellParameters::nAmmo] = 2;
		data[SpellParameters::storedSpell] = (int)SpellName::basic;
		size = 30;
		castType = SpellCastType::Aim;
		desc = "no desc";
		surf = "projectile";
		break;
	case SpellName::basic:
		isSoulSpell = false;
		c = Color(255, 255, 255);
		data[SpellParameters::nProjectiles] = 1;
		data[SpellParameters::coneAngle] = 30;
		data[SpellParameters::speed] = 10;
		data[SpellParameters::damage] = 1;
		data[SpellParameters::rangeCap] = 200;

		size = 30;
		castType = SpellCastType::Aim;
		desc = "no desc";
		surf = "projectile";
		break;
	}
}

float Spell::GetDamage() {
	return data[SpellParameters::damage];
}

void Spell::DrawBar(Graphics &gfx, std::vector<Spell> &spells, Spell &gcd, int crntCastIndex_in)
{
	int width = 30;
	int height = 30;
	int spacing = 3;
	int x = gfx.ScreenWidth / 2 - (spells.size()*width + (spacing * (spells.size() - 1))) / 2;
	int y = gfx.ScreenHeight - height - spacing;
	for (int i = 2; i < spells.size(); i++) {
		if (i == crntCastIndex_in) {
			int padding = 3;
			Color paddingC = Colors::CastBarGold;
			gfx.DrawRect(x + width * i + spacing * i, y, width, height, paddingC);
			spells[i].Draw(Vec2(x + width * i + spacing * i + padding, y + padding), width - padding * 2, height - padding * 2, gfx, gcd);
		}
		else {
			spells[i].Draw(Vec2(x + width * i + spacing * i, y), width, height, gfx, gcd);
		}
	}
}

void Spell::Draw(Vec2 vec_in, int width_in, int height_in, Graphics &gfx, Spell &gcd) {
	if (cdLeft > gcd.cdLeft) {
		cdBar.Draw(vec_in, width_in, height_in, GetData(SpellParameters::cooldown) - cdLeft, GetData(SpellParameters::cooldown), c, Colors::BackgroundGrey, gfx, 0);
	}
	else {
		cdBar.Draw(vec_in, width_in, height_in, gcd.GetData(SpellParameters::cooldown) - gcd.cdLeft, gcd.GetData(SpellParameters::cooldown), c, Colors::BackgroundGrey, gfx, 0);
	}
}

void Spell::Draw(Vec2 vec_in, int width_in, int height_in, Spell gcd, Graphics &gfx) {
	cdBar.Draw(vec_in, width_in, height_in, cdLeft, GetData(SpellParameters::cooldown), c, Colors::BackgroundGrey, gfx, 0);
}

void Spell::Update(float d) {
	if (cdLeft > 0) {
		cdLeft--;
	}
	else {
		cdLeft = 0;
	}
}



float Spell::GetData(SpellParameters input)
{
	if (data.count(input)) {
		float out = 0;
		switch (input) {
		case SpellParameters::storedSpellRebound:
			if (data[input] == -1) {//key for itself
				return type;
			}
			break;
		case SpellParameters::storedSpellExpire:
			if (data[input] == -1) {//key for itself
				return type;
			}
			break;
		}
		//we have the default output
		out = data[input];
		return out;
	}
	else {
		return 0;
	}
}

void Spell::AddUpdate(std::map < SpellParameters, float > update_in) {
	updates.push_back(update_in);
}

bool Spell::AddStoredSpell(SpellName spell)
{
	if (storedSpells.count(spell) > 0) {
		return true;
	}
	else {
		storedSpells[spell] = new Spell(spell);
		return false;
	}
}

Spell * Spell::GetStoredSpell(SpellName spell)
{
	AddStoredSpell(spell);
	return storedSpells[spell];
}

std::string Spell::GetParameterDesc(SpellParameters spellP_in)
{
	std::string output = "unknown (" + std::to_string(int(spellP_in)) + ")";
	switch (spellP_in) {
	case SpellParameters::speed:
		output = "speed";
		break;
	case SpellParameters::nProjectiles:
		output = "ammount of projectiles";
		break;
	case SpellParameters::reboundCount:
		output = "ammount of rebounds";
		break;
	case SpellParameters::distanceFromCaster:
		output = "distance from caster";
		break;
	case SpellParameters::storedSpell:
		output = "stored spell";
		break;
	case SpellParameters::castSpeed:
		output = "casting speed";
		break;
	case SpellParameters::notDeleteOnHit:
		output = "pass through";
		break;
	case SpellParameters::storedSpellRebound:
		output = "spell released on rebound";
		break;
	case SpellParameters::returnOnHit:
		output = "bounce from enemies";
		break;
	case SpellParameters::returnOnHeroHit:
		output = "bounce from hero";
		break;
	case SpellParameters::returnCount:
		output = "ammount of bounces";
		break;
	case SpellParameters::nAmmo:
		output = "shots saved";
		break;
	case SpellParameters::rangeCap:
		output = "range";
		break;
	case SpellParameters::coneAngle:
		output = "spread";
		break;
	case SpellParameters::storedSpellExpire:
		output = "spell released on expiring";
		break;
	case SpellParameters::addAmmoOnHeroHit:
		output = "add ammo on hero hit";
		break;
	case SpellParameters::damage:
		output = "damage";
		break;
	}
	return output;
}

using namespace std;

void Spell::DrawSpells(std::vector<Spell>& spells_in, Vec2 vec_in, Font & font, Graphics & gfx)
{
	Vec2 vec = vec_in;
	int padding = 3;
	for (auto &spell : spells_in) {
		std::vector<std::string> out;
		out.push_back("cdCrnt: " + to_string(spell.cdLeft) + " ");
		out.push_back("cdMax: " + to_string(spell.GetData(SpellParameters::cooldown)) + " ");
		out.push_back("ctCrnt: " + to_string(spell.castTimeLeft) + " ");
		out.push_back("ctMax: " + to_string(spell.GetData(SpellParameters::castTime)) + " ");

		font.DrawBg("SpellId: " + to_string(spell.type) + " ", vec, gfx);
		vec += Vec2(0, font.getGlyphHeight() + padding);
		for (auto line : out) {
			font.DrawBg("  " + line, vec, gfx);
			vec += Vec2(0, font.getGlyphHeight() + padding);
		}
	}
}


void Spell::DrawRangeIndicator(Vec2 & heroVec, Vec2 targetVec, Vec2 pos, Graphics & gfx) {
	Vec2 edge =
		Vec2(
			Vec2::GetVecByAngle(Vec2::GetDeltaVec(targetVec, heroVec).GetAngle() -
			(GetData(SpellParameters::coneAngle) * M_PI / 180) / float(2)).GetNormalized().x * GetData(SpellParameters::coneR),
			Vec2::GetVecByAngle(Vec2::GetDeltaVec(targetVec, heroVec).GetAngle() -
			(GetData(SpellParameters::coneAngle) * M_PI / 180) / float(2)).GetNormalized().y * GetData(SpellParameters::coneR)
		)
		+ heroVec;


	if (drawRangeIndicator && castType == SpellCastType::Angle) {
		gfx.DrawConeOutline(heroVec + pos, edge + pos, GetData(SpellParameters::coneAngle), 30, c);
		drawRangeIndicator = false;

	}
	if (drawRangeIndicator && castType == SpellCastType::Aim) {
		//gfx.DrawVec(heroVec + pos, targetVec + pos, Colors::White);
		drawRangeIndicator = false;
	}
}



