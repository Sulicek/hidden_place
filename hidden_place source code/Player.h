#pragma once
#include "Entity.h"
#include <vector>
#include "Projectile.h"
#include "Health.h"
#include "Enemy.h"
#include <vector>
#include "MainWindow.h"
#include "Spell.h"
#include "ProgressBar.h"
#include "Surface.h"
#include "Effect.h"
#include "EffectGroup.h"
#include "Animation.h"
#include "Rect.h"
#include "Cone.h"
#include "Inventory.h"
#include "Pet.h"
#include <map>
#include "Circle.h"
#include "Sound.h"
#include "Light.h"
#include "Camera.h"

using namespace std;

class Player : public Entity {
private:
public:
	float stepSoundCount = 0;
	Vec2 centerVecRef;
	float soulPower = 10;
	Color soulPowerColor;
	Light soulPowerLight;
	Light staminaLight;
	int usedSoulItem = 0;
	std::map<int, SoulItem> soulItems;
	Camera cam;
	Vec2 lastHeroVec;
	//Vec2 camPoint;
	//Vec2 camVel;
	//float camSpeed;
	std::vector <Projectile> projectiles;
	std::vector <class Cone> cones;
	std::vector <Spell> spellbook;
	Rect drawRect;
	int crntCastIndex = -1;
	Vec2 *target = nullptr;
	std::vector <class Pet> pets;
	Spell gcd;
	Spell bait;
	std::string surf;
	//Sound attackSound;
	class EffectGroup e;
	Animation a;
	Animation aRoot;
	Inventory inv;
	Inventory *chestInv = nullptr;
	Light heroLight;
	Color coatColor = Color(19, 42, 79);
	float coatOpacity = 0.5;
	std::map<ItemParameters, int> stats;
	int statMax = 5;
	float healthMax;
	int level = 0;
	int statPointsToAllocate = 0;
	int mainSpellId = 1;
	int regenTimeOutManaCrnt = 0;
	int regenTimeOutManaMax = 30;
	int regenTimeOutStaminaCrnt = 0;
	int regenTimeOutStaminaMax = 30;
	float resourceMana = 0;
	int resourceManaMax = 300;
	float resourceStamina = 0;
	int resourceStaminaMax = 200;
	float resourceDark = 0;
	int resourceDarkMax = 100;
	int coins = 0;
	float health;
	int heals = 0;
	float xp = 0;
	int chaosLevel = 0; 
	int xpRequiredNext = 100;
	int deathCount = 0;
	//for interacting with the outside world, if true we interacted this frame
	bool isAction = false;
	bool isActive = true;
	bool isCasting = false;
	bool isTargeting = false;
	bool isInControll = true;
	bool isInChest = false;
	bool isRooted = false;
	bool canSkipGcdForDash = false;
	int isBeingHit = 0;
	int screenShake = 0;
	int screenShakeMax = 40;
public:
	Player();
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	//drawing the player figure
	void DrawAt(Graphics &gfx, Vec2 pos);
	void Draw(Graphics &gfx, Vec2 pos, class Font font, class MainWindow & wnd);
	//updating the player, the game loop essentially
	void Update(std::map<int, Enemy*> &enemies, class Room & room,
		class MainWindow &wnd);
	//adjust the movement of the player, using the keyboard in the main window (wnd)
	void MoveControls(MainWindow & wnd);
	void SummonProjectiles(int nProjectiles, 
		std::vector<Projectile> &projectiles_in);
	//casting process
	//attempt to start the casting process of the spell with the spellbook index spellIndex,
	void StartCasting(int spellIndex);
	//called when we have no target and a spell (spell) to be cast based on spell cast type
	void LookForTarget(Spell & spell_in,
		Room& room, MainWindow &wnd);
	//called in the update method, we process the casts of the player. Handles the actual casting process as well as the cast it self
	void ProcessCasts(Room& room, MainWindow &wnd);
	//called in the update method, we process the effects of the effect group e
	void ProcessEffects(EffectGroup &e, Room & room);
	//method called when a spell (spell_in) succesfully hits and enemy(enemy_in)
	void OnHit(class Spell & spell_in, Enemy &enemy_in);
	//called when we kill the enemy (enemy_in)
	void ProcessKill(Enemy & enemy_in);
	//function for returning stats with based on the item parameter itemP_in
	int GetStats(ItemParameters itemP_in);
	//simple setting of a target
	void SetTarget(Vec2 &trgt);
	//called when a hero collides with the enemy pointed to by the *trgt
	void OnCollision(Enemy *trgt);
	//is called when a hero is hit by a spell_in by the enemy
	void ProcessHit(Enemy *enemy_in, Spell *spell_in);
	//add a spell to player's spellbook
	void AddSpell(Spell spell_in);
	//add an effect to the player
	void AddEffect(EffectType effectType_in, float timeLeft);
	//Process a using of an item in the inventory with the id of invId
	void ProcessItemUse(Item &item);
	//setting a spell we autocast on each attack
	void SetMainSpell(int spellId);
	//shake the screen
	void ShakeScreen(int shakeIncrease = -1);
	//return wether we interacted with the outside world this frame
	bool IsAction();
	//return the number of dormant projectiles
	int NDormant();
	//dash
	void Dash(Vec2 direction_in, int dashLength = 100);
	void AttemptToDash();
	//coit stuff
	int GetCoins();
	void AddCoins(float coins_in);
	bool RemoveCoins(float coins_in);
	vector<ItemParameters> GetUpgradableStats();
	int GetStatCost(int crntLevel);
	void Respawn(int toll_in);
};