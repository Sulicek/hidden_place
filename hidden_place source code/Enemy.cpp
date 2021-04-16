#include "Enemy.h"
#include "Vec2.h"
#include "Graphics.h"
#include <random>
#include <cmath>
#include "Colors.h"
#include "Projectile.h"
#include "Entity.h"
#include "Room.h"
#include "SoundManager.h"
#include "Player.h"
#include "Spell.h"
#include "Light.h"
#include "Parameters.h"

#define M_PI 3.14159265358979323846

using namespace std;

json Enemy::enemyJ = {};



void Enemy::Draw( Graphics &gfx, Font &font, Vec2 pos) {
	if (isActive) {
		//gfx.DrawSprite(Vec2(vec.x + pos.x, vec.y + pos.y), width, height, surf);
		if (!showPreloadAnim) {
			a.Draw(vec, drawRect, pos, gfx);
		}
		else {
			preloadA.Draw(vec, drawRect, pos, gfx);
		}
		if (mask == "none") {
			//a.Draw(vec, Rect(Vec2(0, 0), width, height), pos, coat, coatOpacity, sm, gfx);
		}
		else {
			a.DrawMask(mask, vec, drawRect, pos, coat, coatOpacity, gfx);
		}
		if (isBeingHit > 0) {
			a.Draw(vec, drawRect, pos, Colors::White, gfx);
		}
		
		//castBar.Draw(gfx, 0, pos); 
	}
	if (showHealthBar) {
		if (isBoss) {
			std::pair<int, int> wh = { 600, 30 };
			Vec2 drawLocation = Vec2(Graphics::ScreenWidth / 2 - 300, 30) - pos;
			healthBar.Draw(gfx, 2, pos,
				Parameters().Set<std::pair<int, int>*>(&wh).Set<Vec2*>(&drawLocation));
			
		}
		else {
			healthBar.Draw(gfx, 0, pos);
		}
	}
	for (int i = 0; i < projectiles.size(); i++) {
		projectiles[i].Draw(gfx, pos);
	}
	for (int i = 0; i < cones.size(); i++) {
		cones[i].Draw(pos, gfx);
	}
}

void SetSprite(EnemyGfxType enGfxType, Enemy & en) {
	bool defaultAnimLoad = true;
	bool defaultDrawRect = true;
	switch (enGfxType) {
	case EnemyGfxType::eg_lock:
		en.hitSound = "hit_metal";
		defaultAnimLoad = false;
		en.health = 10;
		en.speed = 0;
		en.width = 64;
		en.height = 64;
		en.surf = "lock";
		en.mask = "none";
		en.a.Load(en.surf, 8, { AnimationType::Idle, AnimationType::Death });
		break;
	case EnemyGfxType::eg_crystal:
		en.hitSound = "hit_crystal";
		en.hasHitbox = true;
		en.width = 120;
		en.height = 120;
		en.hitBox = Rect(Vec2(en.width / 4, 0), en.width / 2, en.height);
		en.coatOpacity = 0.5;
		en.surf = "crystal";
		en.mask = "none";
		en.a.AddMask(en.mask, en.mask);
		break;
	case EnemyGfxType::eg_hammer:
		en.hitSound = "hit_metal";
		en.width = 120;
		en.height = 120;
		en.coatOpacity = 0.5;
		en.surf = "hammer";
		en.mask = "none";
		en.a.AddMask(en.mask, en.mask);
		break;
	case EnemyGfxType::eg_goblet:
		en.hitSound = "hit_woodHollow";
		en.width = 120;
		en.height = 120;
		en.coatOpacity = 0.5;
		en.surf = "goblet";
		en.mask = "none";
		en.a.noAttackAnim = true;
		en.a.AddMask(en.mask, en.mask);
		break;
	case EnemyGfxType::eg_leaves:
		en.hitSound = "hit_leaves";
		en.width = 80;
		en.height = 80;
		en.coatOpacity = 0;
		en.surf = "leaves";
		en.mask = "leavesMask";
		en.a.AddMask(en.mask, en.mask);
		break;
	case EnemyGfxType::eg_vomit:
		en.hitSound = "hit_leaves";
		en.width = 80;
		en.height = 80;
		en.coatOpacity = 0;
		en.surf = "vomit";
		en.mask = "none";
		defaultDrawRect = false;
		en.drawRect = Rect(Vec2(-40, -40), 160, 160);
		en.a.AddMask(en.mask, en.mask);
		break;
	case EnemyGfxType::eg_nade:
		en.hitSound = "hit_leaves";
		en.width = 80;
		en.height = 80;
		en.coatOpacity = 0;
		en.surf = "nade";
		en.mask = "none";
		defaultDrawRect = false;
		en.drawRect = Rect(Vec2(-40, -40), 160, 160);
		en.a.AddMask(en.mask, en.mask);
		//en.a.tempMax /= 2;
		break;
	case EnemyGfxType::eg_souls:
		en.hitSound = "hit_cloth";
		en.width = 40;
		en.height = 40;
		en.coatOpacity = 0.5;
		en.surf = "souls";
		en.mask = "soulsMask";
		en.a.AddMask(en.mask, en.mask);
		break;
	case EnemyGfxType::eg_chort:
		en.hitSound = "hit_woodHollow";
		en.isBoss = true;
		en.width = 160;
		en.height = 160;
		en.coat = Colors::Magenta;
		en.coatOpacity = 0;
		en.surf = "wolf";
		break;
	case EnemyGfxType::eg_tree:
		en.hitSound = "hit_woodChip";
		en.isBoss = true;
		en.width = 320;
		en.height = 320;
		en.coat = Colors::Magenta;
		en.coatOpacity = 0;
		en.surf = "tree";
		break;
	case EnemyGfxType::eg_fountain:
		en.hitSound = "hit_water";
		en.isBoss = true;
		en.width = 160;
		en.height = 160;
		en.coat = Colors::Magenta;
		en.aFramesToAttack = 0;
		en.coatOpacity = 0;
		en.surf = "fountain";
		en.a.tempMax /= 2;
		break;
	case EnemyGfxType::eg_hammers:
		en.hitSound = "hit_metal";
		en.isBoss = true;
		en.width = 240;
		en.height = 240;
		en.coat = Colors::Magenta;
		en.aFramesToAttack = 0;
		en.coatOpacity = 0;
		en.surf = "hammers";
		en.a.tempMax /= 2;
		break;
	case EnemyGfxType::eg_heart:
		en.hitSound = "hit_cloth";
		en.isBoss = true;
		en.width = 300;
		en.height = 300;
		en.coat = Colors::Magenta;
		en.aFramesToAttack = 0;
		en.coatOpacity = 0;
		en.surf = "heart";
		en.a.tempMax /= 2;
		defaultAnimLoad = false;
		en.a.Load(en.surf, 10, { AnimationType::Idle, AnimationType::WindUp, AnimationType::Striking });
		en.preloadA.Load("heart rooted", 10, { AnimationType::Idle, AnimationType::WindUp, AnimationType::Striking });
		en.showPreloadAnim = true;
		defaultDrawRect = false;
		en.drawRect = Rect(Vec2(-90, -90), 480, 480);
		en.a.sound.frame = 5;
		en.a.sound.name = "special_heart";
		break;
	}
	if (defaultDrawRect) {
		en.drawRect = Rect(Vec2(0, 0), en.width, en.height);
	}
	if (defaultAnimLoad) {
		en.a.Load(en.surf, 8, { AnimationType::Idle, AnimationType::WindUp, AnimationType::Striking });
	}
}

void SetBehaviour(EnemyType enType, Enemy & en, int modifier) {
	en.speed = 1;
	en.mainSpellId = 0;
	switch (enType) {
	case EnemyType::et_afk:
		en.speed = 0;
		break;
	case EnemyType::et_ritual:
		en.speed = 0;
		en.spellbook[0].push_back(Spell(SpellName::en_ritual));
		break;
	case EnemyType::et_ranged:
		en.speed = 0;
		if (modifier != -1) {
			en.GetInventory()->AddItem(MinorItem({ {SpellParameters::nProjectiles, modifier},
			{SpellParameters::coneAngle, 25} }, { {SpellParameters::coneAngle,modifier }}));
		}
		en.spellbook[0].push_back(Spell(SpellName::en_ranged));
		break;
	case EnemyType::et_snipe:
		en.speed = 0;
		if (modifier != -1) {
			en.GetInventory()->AddItem(MinorItem({ {SpellParameters::speed, modifier} }, {}));
		}
		en.spellbook[0].push_back(Spell(SpellName::en_snipe));
		break;
	case EnemyType::et_root:
		en.speed = 0;
		en.spellbook[0].push_back(Spell(SpellName::en_root));
		break;
	case EnemyType::et_conduit:
		break;
	case EnemyType::et_blast:
		en.speed = 0;
		en.spellbook[0].push_back(Spell(SpellName::en_blast));
		break;
	case EnemyType::et_caller:
		en.speed = 0;
		en.spellbook[0].push_back(Spell(SpellName::en_call));
		break;
	case EnemyType::et_pestilence:
		break;
	case EnemyType::et_pursuiter:
		break;
	case EnemyType::et_trap:
		en.speed = 0;
		en.spellbook[0].push_back(Spell(SpellName::en_trap));
		en.detectionRange = 1000;
		break;
	case EnemyType::et_grenade:
		en.speed = 0;
		en.GetInventory()->AddItem(MinorItem({ { SpellParameters::ex_nProjectiles, modifier },{SpellParameters::coneAngle,30} }, {}));
		en.spellbook[0].push_back(Spell(SpellName::en_grenade));
		break;
	case EnemyType::et_rebound:
		en.speed = 0;
		en.GetInventory()->AddItem(MinorItem({ { SpellParameters::channelTicks, modifier } }, {}));
		en.spellbook[0].push_back(Spell(SpellName::en_rebound));
		break;
	case EnemyType::et_vomit:
		en.speed = 0;
		en.GetInventory()->AddItem(MinorItem({ { SpellParameters::channelTicks, modifier } }, {}));
		en.spellbook[0].push_back(Spell(SpellName::en_vomit));
		break;
	case EnemyType::et_boss_chort:
		en.cycleStage = true;
		en.maxStage = 1;
		en.speed = 1;

		en.spellbook[0].push_back(Spell(SpellName::leapEnemy));
		en.spellbook[0].push_back(Spell(SpellName::leapEnemy));
		en.spellbook[0].push_back(Spell(SpellName::leapEnemy));
		en.spellbook[0].push_back(Spell(SpellName::dashEnemy));
		en.spellbook[0].push_back(Spell(SpellName::rangedWolfEnemy));
		en.spellbook[0].push_back(Spell(SpellName::en_chort_ritual));

		en.spellbook[1].push_back(Spell(SpellName::leapEnemy));
		en.spellbook[1].push_back(Spell(SpellName::leapEnemy));
		en.spellbook[1].push_back(Spell(SpellName::leapEnemy));
		en.spellbook[1].push_back(Spell(SpellName::dashEnemy));
		en.spellbook[1].push_back(Spell(SpellName::en_chort_blast));
		en.inv[1] = new Inventory();
		en.inv[1]->AddItem(MinorItem({}, { { SpellParameters::channelTicks, 6 }, 
			{ SpellParameters::channelLength, 6 } }));

		break;
	case EnemyType::et_boss_tree:
		en.cycleStage = true;
		en.speed = 0;
		en.maxStage = 1;
		en.spellbook[0].push_back(Spell(SpellName::en_tree_v));
		en.spellbook[0].push_back(Spell(SpellName::en_tree_s));
		en.spellbook[1].push_back(Spell(SpellName::en_tree_rebound));
		
		en.GetStage = [](Enemy*en) mutable{
			if (en->stage == 0  && en->nCasts >= 6) {
				en->stage = 1;
				en->nCasts = 0;
			}
			if (en->stage == 1 && en->nCasts >= 1) {
				en->stage = 0;
				en->nCasts = 0;
			}
		};
		break;
	case EnemyType::et_boss_fountain:
		en.cycleStage = true;
		en.speed = 0;
		en.maxStage = 2;
		en.spellbook[0].push_back(Spell(SpellName::en_fountain_grenade));
		en.spellbook[1].push_back(Spell(SpellName::en_fountain_grenade));
		en.spellbook[2].push_back(Spell(SpellName::en_fountain_grenade));
		
		en.spellbook[3].push_back(Spell(SpellName::en_fountain_vomit));

		en.spellbook[4].push_back(Spell(SpellName::en_fountain_ritual));

		en.inv[1] = new Inventory();
		en.inv[2] = new Inventory();
		en.inv[4] = new Inventory();

		en.inv[0]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,3} }));
		en.inv[1]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,2} }));
		en.inv[2]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,1} }));

		en.GetStage = [](Enemy*en) mutable {
			if (en->stage == 0 && en->nCasts >= 1) {
				en->stage = 1;
				en->nCasts = 0;
			}
			if (en->stage == 1 && en->nCasts >= 1) {
				en->stage = 2;
				en->nCasts = 0;
			}
			if (en->stage == 2 && en->nCasts >= 1) {
				en->stage = 3;
				en->nCasts = 0;
			}
			if (en->stage == 3 && en->nCasts >= 1) {
				en->stage = 0;
				en->nCasts = 0;
			}

			if (en->health * 2 < en->maxHealth / 3) {

			}

			if (en->health < en->maxHealth / 3) {
				en->stage = 4;
			}
		};
		break;
	case EnemyType::et_boss_hammers:
		en.cycleStage = true;
		en.speed = 0;
		en.maxStage = 11;
		en.projectileSpawnPointOffset = 50;
		en.deleteProjectilesOnDeath = true;
		for (int i = 0; i < 9; i++) {
			en.spellbook[i].push_back(Spell(SpellName::en_hammers_blast));
		}
		en.spellbook[9].push_back(Spell(SpellName::en_hammers_void));
		en.spellbook[10].push_back(Spell(SpellName::en_hammers_spray));
		for (int i = 1; i < 11; i++) {
			en.inv[i] = new Inventory();
		}

		en.inv[0]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,4} }));
		en.inv[1]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,3} }));
		en.inv[2]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,2} }));
		en.inv[3]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,1} }));
		en.inv[4]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,0} }));
		en.inv[5]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,1} }));
		en.inv[6]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,2} }));
		en.inv[7]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,3} }));
		en.inv[8]->AddItem(MinorItem({}, { {SpellParameters::coneAngle,4} }));

		en.GetStage = [](Enemy*en) mutable {
			if (en->health <= en->maxHealth/2) {
				if (en->nCasts >= 1) {
					if (en->stage == 9) {
						en->stage = 10;
					}
					else {
						en->stage = 9;
					}
					en->nCasts = 0;
				}
			}
			else {
				if (en->stage < 10) {
					if (en->nCasts >= 1) {
						en->stage++;
						en->nCasts = 0;
					}
					if (en->stage == 10) {
						en->stage = 0;
					}
				}
			}
			
		};
		break;
	case EnemyType::et_boss_heartFake:
		en.cycleStage = true;
		en.speed = 0;
		en.maxStage = 1;
		en.showHealthBar = false;
		en.GetStage = [](Enemy*en) mutable {
			if (en->health+5 < en->maxHealth) {
				if (en->dummyCounter == 0) {
					en->dummyCounter = 1;
					en->showPreloadAnim = false;
					en->textCommandsPointer->push("nodamage");
					en->textCommandsPointer->push("unlocksigns");
				}
			}
			if (en->health + 10 < en->maxHealth) {
				if (en->dummyCounter == 1) {
					en->dummyCounter = 2;
					//en->textCommandsPointer->push("nodamage");
					en->textCommandsPointer->push("!sts friendbossfight1");
					en->noDamageTaken = true;
				}
			}
			if (en->health + 15 < en->maxHealth) {
				if (en->dummyCounter == 2) {
					en->dummyCounter = 3;
					//en->textCommandsPointer->push("nodamage");
					en->textCommandsPointer->push("!sts friendbossfight2");
				}
			}
		};
		break;
	case EnemyType::et_boss_heart:
		en.cycleStage = true;
		en.deleteProjectilesOnDeath = true;
		en.isFinalBoss = true;
		en.speed = 0;
		en.maxStage = 1;
		en.stage = 3;
		en.spellbook[0].push_back(Spell(SpellName::en_heart_sprayLockShort));
		en.inv[0]->AddItem(MinorItem({ {SpellParameters::coneAngle, 45} }, {}));
		en.spellbook[1].push_back(Spell(SpellName::en_heart_spraySnipe));
		en.spellbook[2].push_back(Spell(SpellName::en_heart_explode));
		en.spellbook[3].push_back(Spell(SpellName::en_heart_jail));
		for (int i = 1; i < 11; i++) {
			en.inv[i] = new Inventory();
		}
		en.spellbook[4].push_back(Spell(SpellName::en_heart_jail));
		en.inv[4]->AddItem(MinorItem({ {SpellParameters::ex_nProjectiles, 5} }, {}));
		en.spellbook[5].push_back(Spell(SpellName::en_heart_vomit));
		en.spellbook[6].push_back(Spell(SpellName::en_heart_sprayLockShort));
		en.spellbook[7].push_back(Spell(SpellName::en_heart_voidTrigger));
		en.spellbook[8].push_back(Spell(SpellName::en_heart_explode));
		en.inv[8]->AddItem(MinorItem({ {SpellParameters::castTime, 180} }, {}));
		en.GetStage = [](Enemy*en) mutable {
			int nFirst = 1;
			int nSecond = 1;
			en->showPreloadAnim = false;
			if (en->health > (en->maxHealth * 2) / 3) {
				if (en->dummyCounter >= 3) {
					en->stage = 3;
					en->dummyCounter = 0;
					en->nCasts = 0;
				}
				if (en->stage < 2 && en->nCasts > 0) {
					en->stage = 2;
					en->nCasts = 0;
				}
				if (en->stage == 2 && en->nCasts > 0) {
					en->stage = 0;
					if (en->stage == 2) {
						en->stage = 0;
					}
					en->nCasts = 0;
					en->dummyCounter++;
				}
				if (en->stage == 3 && en->nCasts >= 3) {
					en->stage = 2;
					en->nCasts = 0;
				}
			}
			else if (en->health > en->maxHealth /3 && en->dummyCounter != -1){
				en->stage = 5;
				en->dummyCounter = -1;
				en->nCasts = 0;
			}
			else if (en->health > en->maxHealth / 3 && en->nCasts > 0 && en->dummyCounter == -1) {
				if (en->stage != 8 && en->nCasts >= (nFirst + nSecond)) {
					en->nCasts = nFirst;
					en->stage = 8;
				}
				else if ((en->stage == 8 )&&en->nCasts > nFirst) {
					en->stage = 4;
					en->nCasts = nFirst;
				}
				else if (en->stage == 5) {
					en->stage = 8;
					en->nCasts = nFirst;
				}
			}
			else {
				if (en->stage != 6 && en->nCasts > 1) {
					en->stage = 6;
					en->nCasts = 0;
				}
				if (en->stage == 6 && en->nCasts > 0) {
					en->stage = 7;
					en->nCasts = 0;
				}
			}
		};
		break;
	}
}

float GetOptionalData(json source, std::string key) {
	return 0;
}

Enemy::Enemy(Vec2 vec_in, json source, std::string biom):
	healthBar(Vec2(vec_in.x - 50, vec_in.y - 10 - 2), Color(163, 0, 0), Colors::BackgroundGrey, 100, 10),
	castBar(Vec2(vec_in.x - 50, vec_in.y - 10 - 2), Colors::Yellow, Colors::BackgroundGrey, 100, 3),
	a("Resources\\Sprites\\Bones_spriteSheet_01.bmp"),
	preloadA("Resources\\Sprites\\Bones_spriteSheet_01.bmp"),
	gcd(SpellName(0)),
	drawRect(Vec2(0, 0), width, height){
	vel.x = 0;
	vel.y = 0;
	vec.x = vec_in.x;
	vec.y = vec_in.y;
	width = 40;
	height = 40;
	inv[0] = new Inventory();

	if (source.count("name") > 0) {//if we have a name
		//we use that
		string name = source["name"];
		json eData = Enemy::enemyJ["enemies"][name];
		SetSprite((EnemyGfxType)Enemy::enemyJ["config"]["gfxTree"][eData["gfx"].get<std::string>()], *this);
		int modifier = -1;
		if (eData.count("modifier") > 0) {
			modifier = eData["modifier"];
		}
		SetBehaviour((EnemyType)Enemy::enemyJ["config"]["behaviourTree"][eData["behaviour"].get<std::string>()], *this, modifier);
		damage = 1;//eData["damage"];
		health = eData["health"];
		playerGain = 0;//eData["reward"].get<float>();
	}
	else {//id (legacy mode!!!)
		int type = source["id"];
		switch (type) {
		case EnemyType::lock:
			health = 10;
			speed = 0;
			width = 64;
			height = 64;
			surf = "lock";
			mask = "none";
			//spellbook.push_back(Spell(SpellName::globalCoolDown));
			//load animation in
			a.Load(surf, 8, { AnimationType::Idle, AnimationType::Death });
			drawRect = Rect(Vec2(0, 0), width, height);
			break;
		}
	}
	//set max health
	maxHealth = health;
	//fix up placement
	vec = vec - getCenterVec() + vec;
}



void Enemy::Update(Player &hero, map<int, class Enemy*> &enemyVec_in, Room &room) {
	if (isActive) {
		if (health < 0) {
			health = 0;
		}
		if (health == 0 && isBeingHit == 0) {
			if (a.Has(AnimationType::Death)) {
				a.SetAnimationType(AnimationType::Death);
				if (a.progress == a.nFrames - 1) {
					isDead = true;
					isActive = false;
				}
			}
			else {
				if (isBoss) {
					Animation::Show("explosion", getCenterVec());
				}
				else {
					Animation::Show("smol", getCenterVec() + Vec2(0, 20));
				}
				if (isFinalBoss) {
					textCommandsPointer->push("sts end");
				}
				if (deleteProjectilesOnDeath) {
					for (auto &projectile : projectiles) {
						projectile.isActive = false;
					}
				}
				isDead = true;
				isActive = false;
			}
		}

		if (isMoving && crntCast == nullptr) {
			vec.x += speed * vel.x;
			vec.y += speed * vel.y;
		}

		//why is this here?
		vel.x = 0;
		vel.y = 0;

		while (vec.x + width >= room.width) {
			vec.x--;
			isMoving = false;
		}
		while (vec.x < 0) {
			vec.x++;
			isMoving = false;
		}
		while (vec.y + height >= room.height) {
			vec.y--;
			isMoving = false;
		}
		while (vec.y < 0) {
			vec.y++;
			isMoving = false;
		}



		//updating animation
		if (!showPreloadAnim) {
			a.Update();
		}
		else {
			preloadA.Update();
		}

		//updating effects
		e.Update();
		//collision processing for the other enemies with this specific enemy
		for (int i = 0; i < enemyVec_in.size(); i++) {
			if (this != enemyVec_in[i]) {
				if (Vec2::IsColliding(enemyVec_in[i]->getCenterVec(), getCenterVec(),
					enemyVec_in[i]->width / 2, width / 2)
					) {
					Vec2 deltaVec = Vec2::GetDeltaVec(enemyVec_in[i]->getCenterVec(), getCenterVec());
					float moveIndex = deltaVec.GetLenght() - (enemyVec_in[i]->width / 2 + width / 2);
					if (deltaVec.GetLenght() != 0) {
						vec = vec + (deltaVec.GetNormalized()* moveIndex);
					}
					else {
						vec = vec + (Vec2(rand() % 100 + 1, rand() % 100 + 1).GetNormalized() * moveIndex);
					}

				}
			}
		}
		for (auto &wall : room.walls) {
			//get the closest point on the wall to the player   
			Vec2 ps = Vec2::GetDeltaVec(getCenterVec(), wall.vec1);
			Vec2 se = Vec2::GetDeltaVec(wall.vec2, wall.vec1);
			//clip the t, so that it E <0, 1>
			float t = (ps * se) / se.GetLenght();
			if (t < 0) {
				t = 0;
			}
			if (t > se.GetLenght()) {
				t = se.GetLenght();
			}
			//the closest point to the player on the wall, we have the start vec of the wall and add to it
			//the distance to the point (which is t * the length of the wall)
			Vec2 point = wall.vec1 + (se.GetNormalized() * t);
			//we treat it like a collision with a ball
			if (Vec2::IsColliding(point, getCenterVec(),
				wall.r, width / 2)
				) {
				//the direction we are moving in
				Vec2 deltaVec = Vec2::GetDeltaVec(point, getCenterVec());
				//how much we need to move them apart
				float moveIndex = deltaVec.GetLenght() - (wall.r + width / 2);
				//we're moving only the hero
				vec = vec + (deltaVec.GetNormalized() * moveIndex);
			}
		}
		//processing hitting animation
		if (isBeingHit > 0) {
			isBeingHit--;
		}
		if (crntCast != nullptr) {
			//		castBar.Update(Vec2(vec.x - 100 / 2 + width / 2, vec.y - 10 - 2), crntCast->castTimeLeft, crntCast->GetData(SpellParameters::castTime), room);
		}
		else {
			//			castBar.Update(Vec2(vec.x - 100 / 2 + width / 2, vec.y - 10 - 2), spellbook[0].cdLeft, spellbook[0].GetData(SpellParameters::cooldown), room);
		}
		//cool down cooldowns
		if (crntCast == nullptr) {
			gcd.cdLeft--;
		}
		for (int i = 0; i < spellbook[stage].size(); i++) {
			if (crntCast != &spellbook[stage][i]) {
				if (spellbook[stage][i].cdLeft > 0) {
					spellbook[stage][i].cdLeft--;
				}
			}
		}

		ProcessAttacks(hero);
		if (crntCast != nullptr && target == nullptr) {
			LookForTarget(hero, room);
		}
		if (target != nullptr && crntCast != nullptr) {
			//processing casts
			ProcessCasts(*target);
		}
		ProcessEffects();
		//updating the projectiles
	}
	//updating the healthbar
	healthBar.Update(Vec2(vec.x - 100 / 2 + width / 2, vec.y - 10 - 2), maxHealth - health, maxHealth, room);
	vector<Projectile> addProj;
	for (int i = 0; i < projectiles.size(); i++) {
		projectiles[i].Update(hero, *this, room, addProj);
	}
	for (auto proj : addProj) {
		projectiles.push_back(proj);
	}
	for (int i = 0; i < cones.size(); i++) {
		cones[i].Update(hero, *this, room);
	}
	nAliveFrames += 1;
	UpdateStage();
}

bool Enemy::IsInRange(Vec2 vec_in, int range)
{
	//we get the delta vector of the 2 vectors (how far they are from each other)
	Vec2 del = Vec2::GetDeltaVec(vec_in, getCenterVec());
	//and compare range
	return (abs(del.GetLenght()) <= range);
}

void Enemy::Pursue(Vec2 target) {
	MoveIn(Vec2::GetDeltaVecNorm(target, vec));
}

void Enemy::ProcessAttacks(Player &hero_in) {
	//clean actually i think
	if (hero_in.isActive) {
		if (IsInRange(hero_in.getCenterVec(), detectionRange)) {
			for (auto &spell : spellbook[stage]) {
				if (IsInRange(hero_in.getCenterVec(), spell.GetData(SpellParameters::range))) {
					isMoving = false;
					StartCasting(&spell);
				}
			}
			if (crntCast == nullptr) {
				MoveTo(hero_in.getCenterVec());
				isMoving = true;
			}
		}
	}
}

void Enemy::StartCasting(Spell *spell) {
	if (crntCast == nullptr &&
		spell->cdLeft <= 0 &&
		gcd.cdLeft <= 0)
	{
		//set the current cast to this spell
		//reseting stuff
		crntCast = spell;
	}
}



void Enemy::LookForTarget(Player &hero_in, Room& room) {
	switch ((int)crntCast->GetData(SpellParameters::enemyCastType)) {
	case EnemyCastType::tracking://LEGACY!!!
		target = &hero_in.centerVecRef;
		break;
	case EnemyCastType::notTracking:
		target = new Vec2(hero_in.getCenterVec());
		break;
	}
	gcd.cdLeft = gcd.GetData(SpellParameters::cooldown);
	crntCast->castTimeLeft = GetInventory()->GetData(crntCast->GetData(SpellParameters::castTime), SpellParameters::castTime);
	crntCast->cdLeft = crntCast->GetData(SpellParameters::cooldown);
}

void Enemy::ProcessHit(float damage){
	if (!noDamageTaken) {
		health -= damage;
	}
	SoundManager::Play(hitSound);
	//flashing on hit
	isBeingHit = 5;
}

float Enemy::GetDamage()
{
	return (float)damage;
}

void Enemy::UpdateStage() {
	//update stages
	//default
	int stageSnapshot = stage;
	if (GetStage == nullptr) {
		if (health < (stage + 1) * maxHealth / (maxStage + 1)) {
			if (stage < (maxStage)) {
				stage++;
			}
		}
	}
	else {
		GetStage(this);
	}
	if (stageSnapshot != stage) { //stage changed
		//refresh spellbook
		spellbook[stage][0].cdLeft = 0;
		//int timeSkip = spellbook[stage][0].GetData(SpellParameters::cooldown);
		for (int i = 1; i < spellbook[stage].size(); i++) {
			spellbook[stage][i].cdLeft = spellbook[stage][i].GetData(SpellParameters::cooldown)
				- spellbook[stage][i].GetData(SpellParameters::castTime);
		}
	}
}

Inventory * Enemy::GetInventory()
{
	for (int i = stage; i >= 0; i--) {
		if (inv.count(i) > 0) {
			return inv[i];
		}
	}
}

void Enemy::ProcessCasts(Vec2 &hero_in) {
	if (crntCast != nullptr && target != nullptr) {
		if (crntCast->channelTimeLeft <= -1) { //stupid condidion for channel system
			a.SetAnimationType(AnimationType::WindUp);
		}
		crntCast->castTimeLeft--;
		crntCast->drawRangeIndicator = true;
		if (crntCast->castTimeLeft <= aFramesToAttack * a.tempMax && !crntCast->isChannel) {
			a.SetAnimationType(AnimationType::Striking);
		}

		if (crntCast->castTimeLeft <= 0){ 
			if (!crntCast->isChannel || 
				GetInventory()->GetData(crntCast->GetData(SpellParameters::channelTicks), SpellParameters::channelTicks) < 1) {
				CastSpell();
			}
			crntCast->drawRangeIndicator = false; //???
			//we reset and are ready to cast 
			bool reset = true;
			//dealing with channeling (starting to channel
			if (crntCast->isChannel && //spell can be channeled
				crntCast->channelTimeLeft <= -1 //and we are not channeling it atm
				) {
				crntCast->channelTicksLeft = 
					GetInventory()->GetData(crntCast->GetData(SpellParameters::channelTicks), SpellParameters::channelTicks);
				crntCast->channelTimeLeft = 
					GetInventory()->GetData(crntCast->GetData(SpellParameters::channelLength), SpellParameters::channelLength);

				/*crntCast->channelTicksLeft--;
				a.SetAnimationType(AnimationType::Striking);
				a.Boost(2);
				a.Refresh();
				CastSpell();*/
			}
			//continuing in channeling
			if (crntCast->isChannel && crntCast->channelTimeLeft > -1) {
				reset = false;
				crntCast->channelTimeLeft--; 
				int timePerTick = 
					GetInventory()->GetData(crntCast->GetData(SpellParameters::channelLength), SpellParameters::channelLength) /
					GetInventory()->GetData(crntCast->GetData(SpellParameters::channelTicks), SpellParameters::channelTicks);
				if (crntCast->channelTimeLeft <= timePerTick * (crntCast->channelTicksLeft)) {
					crntCast->channelTicksLeft--;
					a.SetAnimationType(AnimationType::Striking);
					a.Boost(2);
					a.Refresh();
					CastSpell();
				}
				//crntCast->channelTicksLeft--;//don't forget to do this :)
				if (crntCast->channelTimeLeft <= -1) { //escaping
					reset = true;
				}
			}
			//we reset and are ready to cast again
			if (reset) {
				nCasts += 1;
				crntCast = nullptr;
				target = nullptr;
			}
		}
	}
}

void Enemy::CastSpell()
{
	float dashSpeedCrnt;
	Vec2 dodgeDirection;
	switch (crntCast->type) {
	case SpellName::meleeBasicAttack:
		crntCast->Cast(getCenterVec(), Vec2::GetDeltaVec(*target, getCenterVec()), cones);
		break;
	case SpellName::meleeBossAttack:
		crntCast->Cast(getCenterVec(), Vec2::GetDeltaVec(*target, getCenterVec()), cones);
		break;
	case SpellName::meleeWolfEnemy:
		crntCast->Cast(getCenterVec(), Vec2::GetDeltaVec(*target, getCenterVec()), cones);
		break;
	/*case SpellName::en_ranged:
		crntCast->Cast(Vec2::GetDeltaVecNorm(*target, getCenterVec()), projectiles, getCenterVec(), 1);
		break;*/
	case SpellName::rangedWolfEnemy:
		crntCast->Cast(Vec2::GetDeltaVecNorm(*target, getCenterVec()), projectiles, getCenterVec(), crntCast->GetData(SpellParameters::nProjectiles),
			(int)crntCast->GetData(SpellParameters::coneAngle));
		//crntCast->Cast(Vec2::GetDeltaVecNorm(*target, getCenterVec()), projectiles, getCenterVec(), 1);
		break;
	case SpellName::rangedBossAttack:
		crntCast->Cast(Vec2::GetDeltaVecNorm(*target, getCenterVec()), projectiles, getCenterVec(), 1);
		break;
	case SpellName::quickStrike:
		crntCast->Cast(getCenterVec(), Vec2::GetDeltaVec(*target, getCenterVec()), cones);
		break;
	case SpellName::en_ritual:
		crntCast->Cast(Vec2::GetDeltaVec(*target,
			getCenterVec()), projectiles, getCenterVec(),
			crntCast->GetData(SpellParameters::nProjectiles));
		break;
	case SpellName::en_fountain_ritual:
		crntCast->Cast(Vec2::GetDeltaVec(*target,
			getCenterVec()), projectiles, getCenterVec(),
			crntCast->GetData(SpellParameters::nProjectiles));
		break;
	case SpellName::en_trap:
		crntCast->Cast(Vec2::GetDeltaVec(*target,
			getCenterVec()), projectiles, getCenterVec(),
			crntCast->GetData(SpellParameters::nProjectiles));
		break;
	case SpellName::en_call:
		crntCast->Cast(Vec2::GetDeltaVec(*target,
			getCenterVec()), projectiles, getCenterVec(),
			crntCast->GetData(SpellParameters::nProjectiles), 
			false, crntCast->GetData(SpellParameters::distanceFromCaster));
		break;
	case SpellName::en_heart_stored_jail:
		crntCast->Cast(Vec2::GetDeltaVec(*target,
			getCenterVec()), projectiles, getCenterVec(),
			crntCast->GetData(SpellParameters::nProjectiles), 
			false, crntCast->GetData(SpellParameters::distanceFromCaster));
		break;
	case SpellName::explodeEnemy:
		crntCast->Cast(Vec2::GetDeltaVec(*target,
			getCenterVec()), projectiles, getCenterVec(),
			crntCast->GetData(SpellParameters::nProjectiles));
		break;
	case SpellName::dashEnemy:
		e.Add(EffectType::dash, crntCast->GetData(SpellParameters::dashLength));
		dodgeDirection = Vec2::GetDeltaVecNorm(*target, getCenterVec());
		break;
	case SpellName::retreatEnemy:
		e.Add(EffectType::dash, crntCast->GetData(SpellParameters::dashLength));
		//we dodge to the left/right of the player
		if (rand() % 2 == 0) {
			dodgeDirection = Vec2(Vec2::GetDeltaVecNorm(*target, getCenterVec()).x,
				-Vec2::GetDeltaVecNorm(*target, getCenterVec()).y);
		}
		else {
			dodgeDirection = Vec2(-Vec2::GetDeltaVecNorm(*target, getCenterVec()).x,
				Vec2::GetDeltaVecNorm(*target, getCenterVec()).y);
		}
		break;
	case SpellName::leapEnemy:
		e.Add(EffectType::dash, crntCast->GetData(SpellParameters::dashLength));
		//we dodge to the left/right of the player
		if (rand() % 2 == 0) {
			dodgeDirection = Vec2::GetVecByAngle(Vec2::GetDeltaVec(*target, getCenterVec()).GetAngle() + (float)crntCast->GetData(SpellParameters::coneAngle) * M_PI / 180.0);
		}
		else {
			dodgeDirection = Vec2::GetVecByAngle(Vec2::GetDeltaVec(*target, getCenterVec()).GetAngle() - (float)crntCast->GetData(SpellParameters::coneAngle) * M_PI / 180.0);
		}
		break;
	default:
		if (crntCast->castType == SpellCastType::Aim) {
			Vec2 direction = Vec2::GetDeltaVecNorm(*target, getCenterVec());
			if (crntCast->GetData(SpellParameters::randomDirectionAngle) != 0) {
				float angle = direction.GetAngle();
				float shift = rand() % int(crntCast->GetData(SpellParameters::randomDirectionAngle));
				shift -= int(crntCast->GetData(SpellParameters::randomDirectionAngle)) / 2;
				shift = shift / 360.0f * 2.0f * M_PI;
				angle += shift;
				direction = Vec2::GetVecByAngle(angle);
			}
			crntCast->Cast(direction, projectiles, getCenterVec() - (direction * projectileSpawnPointOffset),
				GetInventory()->GetData(crntCast->GetData(SpellParameters::nProjectiles), SpellParameters::nProjectiles),
				GetInventory()->GetData((int)crntCast->GetData(SpellParameters::coneAngle), SpellParameters::coneAngle), 
				Parameters().Set<Inventory*>(GetInventory()), false);
		}
		break;
	}
	if (e.Has(EffectType::dash)) {
		float dashLength = crntCast->GetData(SpellParameters::dashDistance);
		float dashTolerance = 10;
		float dashK = 10;
		float nDashFrames = log10(dashTolerance / (dashLength / speed)) / log10((dashK - 1.0) / dashK);
		dodgeDirection = dodgeDirection.GetNormalized();
		*(e.Get(EffectType::dash)->GetDataDirect(EffectParameters::timeLeft)) = nDashFrames;
		*(e.Get(EffectType::dash)->GetDataDirect(EffectParameters::targetX)) = (dodgeDirection.x * dashLength) + getCenterVec().x;
		*(e.Get(EffectType::dash)->GetDataDirect(EffectParameters::targetY)) = (dodgeDirection.y * dashLength) + getCenterVec().y;
	}
}

void Enemy::ProcessEffects()
{
	for (auto &effect : e.effects) {
		switch (effect.first) {
		case EffectType::dash:
			isMoving = true;
			vel = Vec2::GetDeltaVec(
				Vec2(e.Get(EffectType::dash)->GetData(EffectParameters::targetX),
					e.Get(EffectType::dash)->GetData(EffectParameters::targetY)),
				getCenterVec()) * 0.1;
			isMoving = true;
			break;
		}
	}
}
