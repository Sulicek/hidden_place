#pragma once
#include "Graphics.h"
#include "Colors.h"
#include <string>
#include "Projectile.h"
#include "ProgressBar.h"
#include <map>
#include <vector>
#include <string>

enum SpellParameters {
	resourceCostFire, resourceCostDark, damage, range, cooldown, speed, castTime,
	resourceGenFire, resourceGenDark, nIgniteRemoved, nIgniteAdded, nProjectiles, coneAngle,
	coneR, enemyCastType, healingDone, nSoulsAdded, nSoulsCost, nIgniteRequired,
	healthLost, resourceCostMana, resourceCostStamina, playerAttraction, isChaos,
	nSoulsRequired, isShadow, requiresWheapon, isPhysical, dashLength, dashDistance, reboundCount,
	ss_mod_speed, ss_mod_damage, ss_mod_nProjectiles, ss_mod_powerCost, ss_isAccurate, 
	ss_independentDormant,//bad name i know xd
	channelLength, channelTicks, isChannel, distanceFromCaster, 
	storedSpell, castSpeed, storedSpellRebound, setOutSpeed, isBaited, deleteOnRebound,
	notDeleteOnHit, returnOnHit, returnOnHeroHit, returnCount, nAmmo, rangeCap, storedSpellExpire, 
	oppositeDirection, startingOffset, addAmmoOnHeroHit, ex_nProjectiles,
	randomDirectionAngle, isGhost, isRoot, rootTime, 
	canPlayerDash, spellCastOnDash, //kinda mess (:
	storedSpellHit, directionShift
};

enum SpellCastType {
	Targetable, Aim, AoE, Angle
};

enum SpellName {
	globalCoolDown, debug, 
	dashEnemy, retreatEnemy, meleeBasicAttack, en_ranged,
	quickStrike, sweep, shockwave, quickDash,
	fireball, heatUp, fireNova, callOfFire, flameTap,
	shadowNova, commandShadow, reap, explodeEnemy,meleeBossAttack, rangedBossAttack,
	shadowball, leapEnemy, meleeWolfEnemy, rangedWolfEnemy,
	ss_hate, ss_basic, ss_guilt, ss_empathy, ss_ritual, 
	en_blast, en_ritual, en_trap, en_call, 
	ss_shotgun, stored_claw, stored_return, ss_teleport,
	bait, stored_explode, stored_guilt, basic, stored_scatter,
	en_stored_explode, en_grenade, en_rebound, en_rangedChannel, 
	en_snipe, en_root, en_vomit,
	en_tree_v, en_tree_s, en_tree_rebound,
	en_fountain_grenade, en_fountain_stored_explode, en_fountain_vomit, 
	en_fountain_ritual, en_chort_ritual, en_chort_blast, stored_dash_explode,
	healPlayer, en_hammers_blast, en_hammers_stored_blast, 
	en_hammers_void, en_hammers_stored_void, en_hammers_spray,
	en_heart_sprayLock, en_heart_spraySnipe, en_heart_jail, 
	en_heart_stored_jail,
	en_heart_explode, en_heart_sprayLockShort,
	en_heart_voidTrigger,en_heart_void, en_heart_stored_void, en_heart_vomit
};

enum EnemyCastType {
	tracking, notTracking
};

class Spell {
public:
	std::string desc = "*Description missing*";
	bool isScaleable = false;
	bool isCast = false;
	bool drawRangeIndicator = false;
	bool isSoulSpell = false;
	int cdLeft = 0;
	float castTimeLeft = -1;
	bool isChannel = false;
	int channelTicksLeft = 0;
	int channelTimeLeft = -1;
	int size;
	SpellName type;
	int speed;	//if speed is zero it doesn't need to hit the target and just 
	Color c;	//hits it right away
	std::string surf;
	ProgressBar castBar, cdBar;
	SpellCastType castType;
	//std::map <SpellSoundEffect, Sound> sounds;
	//std::vector <Sound> sounds;
public:
	Projectile* Cast(Vec2 direction_in, std::vector<Projectile>& projectiles, Vec2  heroVec_in, int nShots_in, bool isDormant_in = false, int distance = 0);
	//projectiles in a cone around the player
	void Cast(Vec2 direction_in, std::vector<Projectile>& projectiles, Vec2  heroVec_in, 
		int nShots_in, int angle_in, Parameters param, bool isDormant_in = false);
	//soul item variant
	void Cast(Vec2 direction_in, std::vector<Projectile>& projectiles, Vec2  heroVec_in,
		int nShots_in, int angle_in, class SoulItem &item_in, bool isDormant_in);
	//cone spells
	void Cast(Vec2 & heroVec_in, Vec2 direction_in, std::vector<class Cone>& cones);
	
	Spell(SpellName type_in);
	void DrawRangeIndicator(Vec2 & heroVec, Vec2 targetVec, Vec2 pos, Graphics & gfx);

	static void DrawBar(Graphics &gfx, std::vector <Spell> &spells, Spell &gcd, int crntCastIndex_in);
	void Draw(Vec2 vec_in, int width_in, int height_in, Spell gcd, Graphics &gfx);
	void Draw(Vec2 vec_in, int width_in, int height_in, Graphics &gfx, Spell &gcd);
	void Update(float d);
	float GetDamage();
	float GetData(SpellParameters input);
	void AddUpdate(std::map < SpellParameters, float > update_in);
	static std::map<SpellName, Spell*> storedSpells;
	static bool AddStoredSpell(SpellName spell);
	static Spell* GetStoredSpell(SpellName spell);
	static std::string GetParameterDesc(SpellParameters spellP_in);
	static void DrawSpells(std::vector<Spell> &spells_in, Vec2 vec_in, Font &font, Graphics &gfx);
private:
	union SpellData {
		int i;
		float f;
		bool b;
	};
	std::map < SpellParameters, float > data;
	std::vector< std::map < SpellParameters, float > > updates;
};

