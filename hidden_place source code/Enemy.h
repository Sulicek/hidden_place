#pragma once
#include "Entity.h"
#include <vector>
#include "Vec2.h"
#include "Health.h"
#include "Projectile.h"
#include "ProgressBar.h"
#include "Surface.h"
#include "Spell.h"
#include "Cone.h"
#include "Circle.h"
#include "Animation.h"
#include <map>
#include "EffectGroup.h"
#include <queue>
#include "Font.h"
#include "json.hpp"
#include "Inventory.h"
#include <queue>

using json = nlohmann::json;

enum EnemyType {
	test = -2, slime = 0, archer = 1, hell = 2, midget = 3, wolfBoss = 4, reapBoss = 5,
	dummy = 10, lock, blast,
	et_ritual, et_ranged, et_conduit, et_blast, et_caller, et_pestilence, et_pursuiter, 
	et_trap, et_afk, et_boss_reaper, et_boss_chort, et_grenade, et_rebound, et_snipe,
	et_root, et_vomit, et_boss_tree, et_boss_fountain,
	et_boss_hammers, et_boss_heartFake, et_boss_heart
};

enum EnemyGfxType {
	eg_crystal, eg_hammer, eg_goblet, eg_leaves, eg_souls, eg_bones,
	eg_lock, eg_chort, eg_tree, eg_fountain, eg_vomit, eg_nade, eg_hammers,
	eg_heart, eg_heartRooted
};

using namespace std;

class Enemy : public Entity {
public:
	Rect drawRect;
	std::vector<Projectile> projectiles;
	std::vector<Cone> cones;
	ProgressBar healthBar, castBar;
	std::string surf;
	std::string mask = "none";
	std::map < int,std::vector<Spell>> spellbook;
	Vec2 *target = nullptr;
	Spell *crntCast = nullptr;
	float projectileSpawnPointOffset = 0;
	Spell gcd;
	Animation a, preloadA;
	bool showPreloadAnim = false;
	EffectGroup e;
	map<int,Inventory*> inv;
	Color coat = Colors::White;
	std::queue<std::string> soundQueue;
	queue<string>* textCommandsPointer;
	float coatOpacity = 0;
	int isPursuing;
	int maxHealth;
	int health;
	int damage = 1;
	int detectionRange = 8000;
	int meleeRange = 20;
	bool isTargeted = false;
	bool isActive = true;
	bool isBoss = false;
	int isBeingHit = 0;
	int aFramesToAttack = 0;
	float playerGain = 0;
	int mainSpellId = 0;
	int stage = 0;
	int dummyCounter = 0;
	int maxStage = stage;
	bool cycleStage = false;
	int nAliveFrames = 0;
	int nCasts = 0;
	bool deleteProjectilesOnDeath = false;
	bool showHealthBar = true;
	bool noDamageTaken = false;
	bool isFinalBoss = false;
public:
	Enemy(Vec2 vec_in, json source, std::string biom);
	void Pursue(Vec2 target);
	bool IsInRange(Vec2 vec_in, int range);
	void Update(class Player &hero, std::map<int, class Enemy*> &enemyVec_in, 
		class Room & room);
	void Draw(Graphics &gfx, Font &font, Vec2 pos);
	void ProcessAttacks(class Player &hero_in);
	void ProcessCasts(Vec2 &hero_in); 
	void CastSpell();
	void ProcessEffects();
	void StartCasting(Spell *spell);
	void LookForTarget(Player &hero_in, Room& room);
	void ProcessHit(float damage);
	float GetDamage();
	void UpdateStage();
	void(*GetStage)(class Enemy*) = nullptr;
	Inventory *GetInventory();
	static json enemyJ;
};