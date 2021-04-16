#pragma once
#include "Entity.h"
#include <vector>
#include "Vec2.h"
#include "Colors.h"
#include "Graphics.h"
#include "Surface.h"
#include <map>
#include <string>
#include <set>
#include "Animation.h"
#include "Parameters.h"
#include "Light.h"

class Projectile : public Entity {
private:
	int isRebound = 0;
public:
	Light l;
	class Enemy *pursuit;
	class Player *pursuitPlayer;
	class Inventory *inv = nullptr;
	Player * hero = nullptr;
	Animation a;
	Vec2 *bait = nullptr;
	bool isActive = true;
	bool isDormant = false;
	bool hasHitHero = true;
	bool isTimed = false;
	int returnCount = 0;
	int timeLeft = -1;
	float distanceTraveled = 0.0f;
	float coatIntensity = 0.5;
	class Spell *spell;
	class SoulItem *item = nullptr;
	float speedTarget = 0;
	std::set<Enemy*> collidedEnemies;
	std::map<enum SpellParameters, int> data;
	std::map<enum ItemParameters, float> itemData;
public:
	Projectile(Spell& spell_in, Vec2 vec_in,
		Vec2 direction, Parameters params, bool isDormant_in = false);
	//for hero
	void Update(std::map<int, class Enemy*> &enemies, class Player &hero, class Room & room,
		std::vector<class Projectile> &addProj);
	//for enemy
	void Update(class Player &hero, class Enemy &enemy, class Room & room,
		std::vector<Projectile> &addProj);
	void Draw(Graphics &gfx,  Vec2 pos);
	float GetData(SpellParameters spellP_in);
	int GetData(ItemParameters itemP_in);
	float GetDamage();
	int SetData(SpellParameters spellP_in, int data_in);
	void SetRebound(int rebound_in);
	void SetDormant();
	void SetActive();
	bool SetOut(Vec2 target, class Player & player, Vec2 *bait_in);
	void OnDeath();
	void OnExpire(std::vector<Projectile> &addProj);
};