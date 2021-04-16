#pragma once
#include "Player.h"
#include "Projectile.h"
#include "Health.h"
#include <vector>
#include "Enemy.h"
#include "ProgressBar.h"
#include "Room.h"
#include "MainWindow.h"
#include "Effect.h"
#include "Animation.h"
#include "Font.h"
#include <string>
#include "Pet.h"
#include "SoundManager.h"
#include <cmath>
#include <algorithm>

//ignore this
#define M_PI 3.14159265358979323846 

using namespace std;

void Player::DrawAt(Graphics & gfx, Vec2 pos)
{
	for (int i = 0; i < projectiles.size(); i++) {
		projectiles[i].Draw(gfx, pos);
	}
	for (int i = 0; i < cones.size(); i++) {
		cones[i].Draw(pos, gfx);
	}
	heroLight.Draw(getCenterVec() + pos, gfx);
	a.Draw(vec, drawRect, pos, gfx);
	a.DrawMask("mask", vec, drawRect, pos, coatColor, 0.5, gfx);
}

void Player::Draw(Graphics &gfx, Vec2 pos, Font font, MainWindow & wnd) {
	if (bool(usedSoulItem >= (int)soulItems.size())) {
		usedSoulItem = (usedSoulItem % soulItems.size());
	}
	while (usedSoulItem < 0) {
		usedSoulItem += soulItems.size();
	}

	for (int i = 0; i < projectiles.size(); i++) {
		projectiles[i].Draw(gfx, pos);
	}
	for (int i = 0; i < cones.size(); i++) {
		cones[i].Draw(pos, gfx);
	}
	if (isCasting && crntCastIndex != -1) {
		float intensity = 
			(spellbook[crntCastIndex].GetData(SpellParameters::castTime) -
				spellbook[crntCastIndex].castTimeLeft) /
			max(spellbook[crntCastIndex].GetData(SpellParameters::castTime), 0)
			* 100;
		auto &soulItem = soulItems[usedSoulItem];
		gfx.DrawLight(getCenterVec() + Vec2(0, height / 7) + pos,
			intensity, 0.3, soulItem.GetSpell().c,
			{
			{Graphics::DrawSpecials::jagged, 3},
			{Graphics::DrawSpecials::squary, 5}
			});

	}
	heroLight.Draw(getCenterVec() + pos, gfx);
	a.Draw(vec, drawRect, pos, gfx);
	a.DrawMask("mask", vec, drawRect, pos, coatColor, 0.5, gfx);
	Animation::DrawStatic("strike_left", getCenterVec(), pos, gfx);
	Animation::DrawStatic("strike_right", getCenterVec(), pos, gfx);
	Animation::DrawStatic("strike_top", getCenterVec(), pos, gfx);
	Animation::DrawStatic("strike_bottom", getCenterVec(), pos, gfx);
	if (isBeingHit > 0) {
		a.Draw(vec, drawRect, pos, Colors::White, gfx);
	}
	if (isRooted) {
		aRoot.Draw(vec - Vec2(0, 0), drawRect, pos, gfx);
	}
	//drawPet
	for (int i = 0; i < pets.size(); i++) {
		pets[i].Draw(gfx, pos);
	}

	if (crntCastIndex != -1 && target != nullptr) {
		ProgressBar::DrawManual(spellbook[crntCastIndex].castTimeLeft, spellbook[crntCastIndex].GetData(SpellParameters::castTime),
			1, true, Vec2(gfx.ScreenWidth / 2 - 200 / 2, gfx.ScreenHeight - 60), Color(Color(194, 149, 0)), Colors::BackgroundGrey, 200, 10, gfx);
	}
	//drawing info bars
	//health
	if (drawnHealth != health) {
		drawnHealth += (health - drawnHealth)/2;
	}
	float healthIntensity = drawnHealth / healthMax * 300;
	Vec2 healthDrawVec = Vec2(gfx.ScreenWidth - 100, gfx.ScreenHeight - 50);
	/*gfx.DrawLight(healthDrawVec, healthIntensity, 0.5, Colors::Red,
		{
				{Graphics::DrawSpecials::jagged, 3},
				{Graphics::DrawSpecials::squary, 5},
		}
	);
	Vec2 draw = healthDrawVec;
	font.DrawNoBg(sp, Vec2(draw.x - (sp.length() * font.getGlyphWidth()) / 2,
		draw.y - (font.getGlyphHeight()) / 2), gfx);*/

	if (resourceStamina >= 100) {
		////gfx.DrawRect(10, Graphics::ScreenHeight - 20 - 10, 20, 20, Colors::DarkGreen);
		//gfx.DrawLight({ 20, Graphics::ScreenHeight - 20 - 10 }, 100, 0.5, Colors::Green,
		//	{
		//		{Graphics::DrawSpecials::jagged, 3},
		//		{Graphics::DrawSpecials::squary, 5},
		//	}
		//);
	}
	if (resourceStamina >= 200) {
		/*gfx.DrawLight({ 50, Graphics::ScreenHeight - 20 - 10 }, 100, 0.5, Colors::Green,
			{
				{Graphics::DrawSpecials::jagged, 3},
				{Graphics::DrawSpecials::squary, 5},
			});*/

	}
	if (resourceStamina >= 300) {
		/*gfx.DrawLight({ 80, Graphics::ScreenHeight - 20 - 10 }, 100, 0.5, Colors::Green,
			{
				{Graphics::DrawSpecials::jagged, 3},
				{Graphics::DrawSpecials::squary, 5},
			});*/

	}
	std::string sp = to_string((int)health);

	if (bool(usedSoulItem >= (int)soulItems.size())) {
		usedSoulItem = (usedSoulItem % soulItems.size());
	}
	while (usedSoulItem < 0) {
		usedSoulItem += soulItems.size();
	}

	Vec2 staminaDrawVec = Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight - 50);
	staminaLight.DrawControlled(staminaDrawVec,
		float(int(resourceStamina) / 100) / 3.0f, Colors::Green, gfx);
	std::string staminaTextValue = to_string(int(resourceStamina / 100));
	//font.DrawNoBg(staminaTextValue, staminaDrawVec
	//	+ Vec2(-(font.getGlyphWidth() * staminaTextValue.size()) / 2, -font.getGlyphHeight() / 2), gfx);

	soulPowerLight.DrawControlled(Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight - 100),
		float(health) / 2.0f, Colors::Red, gfx);

	font.DrawNoBg(sp, Vec2(Graphics::ScreenWidth / 2 - (sp.length() * font.getGlyphWidth())/2,
		Graphics::ScreenHeight - 100 - (font.getGlyphHeight()) / 2), gfx);

	
	
}

Player::Player() :
	gcd(SpellName(0)),
	bait(SpellName::bait),
	surf("hero"),
	a("hero"),
	aRoot("root"),
	drawRect(Vec2(-29, -22), 90, 90),
	inv(),
	e(&inv),
	heroLight(300, -1, false, Color(0, 128, 255)),
	soulPowerLight(100, -1, false, Color(0, 128, 255)),
	staminaLight(100, -1, false, Color(0, 255, 0))
	//,attackSound(L"Resources\\Sounds\\Effect_Hit_01.wav")
{
	hitSound = "hit_cloth";
	soulItems[0] = SoulItem(SpellName::ss_basic);
	soulItems[1] = SoulItem(SpellName::ss_guilt);
	soulItems[2] = SoulItem(SpellName::ss_shotgun);


	

	//base stats
	stats[ItemParameters::vitality] = 0;
	stats[ItemParameters::agility] = 0;
	stats[ItemParameters::castSpeedItem] = 0;
	stats[ItemParameters::moveSpeed] = 0;
	//set based on stats
	speed = 3;
	health = GetStats(ItemParameters::vitality);
	//filling spell book
	int specialization = 0;
	spellbook.push_back(Spell(SpellName::quickDash));
	spellbook.push_back(Spell(SpellName::shadowball));
	spellbook.push_back(Spell(SpellName::healPlayer));
	//drawing and hitbox bullshit
	width = 30;
	height = 55;
	heroLight.Off();
	isActive = true;
	healthMax = health;
	c = Colors::DeepBlue;
	a.Load("hero", 5, { AnimationType::RunLeft, AnimationType::RunRight, AnimationType::RunUp, AnimationType::RunDown,
			AnimationType::Idle , AnimationType::Casting, AnimationType::Striking });
	aRoot.Load("root", 4, { AnimationType::Idle });
	a.AddMask("mask", "heroMask");
}

void Player::Update(std::map<int, Enemy*> &enemies, Room& room, MainWindow &wnd) {

	centerVecRef = getCenterVec();
	lastHeroVec = vec;
	//updating stats
	////increasing health if we have more max health
	//int newMaxHealth = GetStats(ItemParameters::vitality) + 3;
	//if (health == healthMax) {
	//	health = newMaxHealth;
	//}
	/*healthMax = newMaxHealth;
	resourceManaMax = toll;*/

	if (bool(usedSoulItem >= (int)soulItems.size())) {
		usedSoulItem = (usedSoulItem % soulItems.size());
	}
	while (usedSoulItem < 0) {
		usedSoulItem += soulItems.size();
	}


	//rounding out the stats
	if (health >= healthMax) {
		health = healthMax;
	}
	resourceMana = min(resourceManaMax, resourceMana);

	//not moving if at the edge
	if (vec.x + width >= room.width || vec.x < 0 ||
		vec.y + height >= room.height || vec.y < 0) {
		isMoving = false;
	}
	if (isMoving == false) {
		vel.x = 0;
		vel.y = 0;
	}


	//moving according to the velocity
	if (!isRooted) {
		if (!isCasting) {
			float thisSpeed = (speed + (0.5*speed * GetStats(ItemParameters::moveSpeed) / 10));
			vec.x += vel.x * thisSpeed;
			vec.y += vel.y * thisSpeed;
			if (stepSoundCount <= 0) {
				SoundManager::Play(room.stepSound);
				stepSoundCount = 30 * speed;
			}
			else {
				stepSoundCount -= vel.GetLenght() * thisSpeed;
				if (!isInControll) {
					stepSoundCount -= vel.GetLenght() * thisSpeed * 2;
				}
			}
			/*	for (auto & proj : projectiles) {
					if (proj.isDormant) {
						proj.vec += vel * speed;
					}
				}*/
		}
		else {
			float moveSpeedReduction = 10.0f - ( 5.0f * (float(GetStats(ItemParameters::agility)) / 10.0f));
			vec.x += vel.x * (speed / moveSpeedReduction);
			vec.y += vel.y * (speed / moveSpeedReduction);
			if (stepSoundCount <= 0) {
				SoundManager::Play(room.stepSound);
				stepSoundCount = 30 * speed;
			}
			else {
				stepSoundCount -= vel.GetLenght() * speed / 10;
			}
			/*for (auto & proj : projectiles) {
				if (proj.isDormant) {
					proj.vec += vel * (speed/10);
				}
			}*/
		}
	}
	if (regenTimeOutManaCrnt == 0) {
		if (resourceMana < resourceManaMax) {
			resourceMana += float(resourceManaMax);
		}
	}
	else {
		regenTimeOutManaCrnt--;
	}

	if (resourceStamina < resourceStaminaMax) {
		resourceStamina += (float(resourceStaminaMax) / (max(1, 300 - 10 * (stats[ItemParameters::strength] - 10)))) * 
			(1.0f + (0.5f * float(GetStats(ItemParameters::agility))/5.0f));
	}

	//collision processing for the player and enemies
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i]->isActive) {
			if (Vec2::IsColliding(enemies[i]->getCenterVec(), getCenterVec(),
				enemies[i]->width / 2, width / 2)
				) {
				//if (!*enemies[i].e.Has(EffectType::dash)) {
				Vec2 deltaVec = Vec2::GetDeltaVec(enemies[i]->getCenterVec(), getCenterVec());
				//how much we need to move them apart
				float moveIndex = deltaVec.GetLenght() - (enemies[i]->width / 2 + width / 2);
				//we're moving only the hero
				vec = vec + (deltaVec.GetNormalized() * moveIndex *0.5);
				//not the enemy
				enemies[i]->vec = enemies[i]->vec;
				//we process collision (for spells, etc.)
				OnCollision(&*enemies[i]);
				//}
			}
		}
	}
	//collision for chests if they're actually active
	if (room.enemies.size() == 0) {
		for (int i = 0; i < room.chests.size(); i++) {
			if (Vec2::IsColliding(room.chests[i].getCenterVec(), getCenterVec(),
				room.chests[i].width / 2, width / 2)
				) {
				Vec2 deltaVec = Vec2::GetDeltaVec(room.chests[i].getCenterVec(), getCenterVec());
				//how much we need to move them apart
				float moveIndex = deltaVec.GetLenght() - (room.chests[i].width / 2 + width / 2);
				//we're moving only the hero
				vec = vec + (deltaVec.GetNormalized() * moveIndex);
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
	//clipping the player in the room
	if (vec.x + width >= room.width) {
		vec.x = room.width - width - 1;
		isMoving = false;
	}
	if (vec.x < 0) {
		vec.x = 0;
		isMoving = false;
	}
	if (vec.y + height >= room.height) {
		vec.y = room.height - height - 1;
		isMoving = false;
	}
	if (vec.y < 0) {
		vec.y = 0;
		isMoving = false;
	}

	cam.Update();
	cam.LerpToLocation(getCenterVec());
	//clipping the view
	if (screenShake) {
		//screen shaking effect
		screenShake--;
		int offset = (rand() % 3) - 1;
		room.pos.x = -cam.pos.x + offset + Graphics::ScreenWidth / 2;
		room.pos.y = -cam.pos.y + offset + Graphics::ScreenHeight / 2;
	}
	else {
		room.pos.x = -cam.pos.x + Graphics::ScreenWidth / 2;
		room.pos.y = -cam.pos.y + Graphics::ScreenHeight / 2;
	}
	isInControll = true;
	//start casting basic attack if we're not casting
	if (crntCastIndex == -1) {
		StartCasting(mainSpellId);
	}
	//find targets if we are casting
	if (crntCastIndex != -1 && target == nullptr) {
		LookForTarget(spellbook[crntCastIndex], room, wnd);
	}
	//process casts
	ProcessCasts(room, wnd);

	//process effects
	ProcessEffects(e, room);

	//custom conditions
	for (int i = 0; i < projectiles.size(); i++) {
		if (projectiles[i].isDormant && projectiles[i].spell->type != SpellName::ss_ritual) {
			//radius of the snap circle
			int innerRadius = 20;
			int strength = 10;
			if (projectiles[i].GetData(ItemParameters::ss_accuracy) > 0) {
				if (isCasting) {
					strength = 100;
					innerRadius = 1;
				}
			}
			if (projectiles[i].spell->type == SpellName::ss_empathy) {
				health++;
			}
			if (Vec2::GetDeltaVec(projectiles[i].getCenterVec(), getCenterVec()).GetLenght() >= innerRadius) {
				projectiles[i].MoveToAMagnetic(getCenterVec(), strength);
			}
			
			//collision between the projectiles
			for (int o = 0; o < projectiles.size(); o++) {
				if (&projectiles[i] != &projectiles[o]) {
					if (Vec2::IsColliding(projectiles[o].getCenterVec(), projectiles[i].getCenterVec(),
						projectiles[o].width / 5, projectiles[i].width / 5)
						) {
						Vec2 deltaVec = Vec2::GetDeltaVec(projectiles[o].getCenterVec(), projectiles[i].getCenterVec());
						float moveIndex = deltaVec.GetLenght() - (projectiles[o].width / 5 + projectiles[i].width / 5);
						if (deltaVec.GetLenght() != 0) {
							projectiles[i].vec = projectiles[i].vec + (deltaVec.GetNormalized()* moveIndex);
						}
						else {
							projectiles[i].vec = projectiles[i].vec + (Vec2(rand() % 100 + 1, rand() % 100 + 1).GetNormalized() * moveIndex);
						}

					}
				}
			}
		}
	}

	//updating the spellbook
	for (int i = 0; i < spellbook.size(); i++) {
		float cdSpeed = 1;
		if (NDormant() > 0) {
			cdSpeed = 2;
		}
		spellbook[i].Update(cdSpeed);
	}
	gcd.Update(1);

	//updating effects
	e.Update();

	//updating animation
	a.Update();
	aRoot.Update();

	//check resource overcapping
	if (resourceDark > resourceDarkMax) {
		resourceDark = resourceDarkMax;
	}

	//updating the projectiles
	std::vector<Projectile> addProj;
	for (int i = 0; i < projectiles.size(); i++) {
		projectiles[i].Update(enemies, *this, room, addProj);
	}
	for (auto i : addProj) {
		projectiles.push_back(i);
	}
	/*for (int i = 0; i < cones.size(); i++) {
		cones[i].Update(enemies, *this, room, bool(inv.GetBonuses(ItemParameters::isParry)));
	}*/
	
	//reseting action
	isAction = false;
	//sort of dying
	if (health <= 0) {
		isActive = false;
	}
}

void Player::SummonProjectiles(int nProjectiles, vector<Projectile> &projectiles_in)
{
	auto &soulItem = soulItems[usedSoulItem];
	soulItem.GetSpell().Cast(
		getCenterVec(), projectiles_in,
		getCenterVec() + Vec2((rand() % 11) - 5, (rand() % 11) - 5).GetNormalized(),
		nProjectiles, 360,
		soulItem,
		true);
}



void Player::ProcessCasts(Room &room, MainWindow &wnd) {
	//processing cast
	if (crntCastIndex != -1 && target != nullptr) {
		SoundManager::PlayOne("cast");
		float castSpeedValue = 
			inv.GetData(soulItems[usedSoulItem].GetSpell().GetData(SpellParameters::castSpeed), 
				SpellParameters::castSpeed);
		castSpeedValue += (castSpeedValue  * ((float(GetStats(ItemParameters::castSpeedItem))/10.0f)));

		spellbook[crntCastIndex].castTimeLeft -= castSpeedValue;
		if (NDormant() > 0 && crntCastIndex == 1) {
			spellbook[crntCastIndex].castTimeLeft = 0;
		}
		ShakeScreen(1);
		isCasting = true;
		a.SetAnimationType(AnimationType::Casting);
		spellbook[crntCastIndex].drawRangeIndicator = true;
		if (spellbook[crntCastIndex].castTimeLeft <= 0) {
			spellbook[crntCastIndex].drawRangeIndicator = false;
			resourceDark -= spellbook[crntCastIndex].GetData(SpellParameters::resourceCostDark);
			resourceDark += spellbook[crntCastIndex].GetData(SpellParameters::resourceGenDark);

			resourceMana -= spellbook[crntCastIndex].GetData(SpellParameters::resourceCostFire);
			//heals -= spellbook[crntCastIndex].GetData(SpellParameters::resourceCostMana);
			resourceStamina -= spellbook[crntCastIndex].GetData(SpellParameters::resourceCostStamina);
			
			int rangeForCommand = 150;

			bool isDashing = false;
			int dashLength = 0;
			int howManyTimesDoWeDoThisDeletionProcess = e.GetStacks(EffectType::soul);
			if (spellbook[crntCastIndex].GetData(SpellParameters::resourceCostStamina) > 0) {
				regenTimeOutStaminaCrnt = regenTimeOutStaminaMax;
			}
			if (spellbook[crntCastIndex].GetData(SpellParameters::resourceCostMana) > 0) {
				regenTimeOutManaCrnt = regenTimeOutManaMax;
			}
			SoundManager::Stop("cast");

			switch (spellbook[crntCastIndex].type) {
			case SpellName::debug:
				break;
				//shadow spells
			case SpellName::shadowball: {
				canSkipGcdForDash = true;
				//offset camvel vector
				Vec2 direction = Vec2::GetDeltaVecNorm(*target, getCenterVec());
				float camNudgeStrength = 1;
				int nDoormantSnapshot = NDormant();
				if (nDoormantSnapshot != 0) {
					cam.AddForce(Vec2(-direction.x, -direction.y) * camNudgeStrength);
				}
				//send out existing souls
				for (int i = 0; i < projectiles.size(); i++) {
					if (projectiles[i].SetOut(*target, *this, nullptr)) {
						//SoundManager::Play("sendoff");
						Spell* sSpell = Spell::GetStoredSpell((SpellName)(int)projectiles[i].spell->GetData(SpellParameters::storedSpell));
						Vec2 direction = Vec2::GetDeltaVecNorm(*target, getCenterVec());
						sSpell->Cast(direction, projectiles,
							getCenterVec(),
							inv.GetData(sSpell->GetData(SpellParameters::nProjectiles), SpellParameters::nProjectiles),
							inv.GetData((int)sSpell->GetData(SpellParameters::coneAngle), SpellParameters::coneAngle),
							Parameters().Set<SoulItem*>(&soulItems[usedSoulItem]).Set<Player*>(this).Set<Inventory*>(&inv),
							false);
						SoundManager::Play("sendoff");
						//Dash(Vec2(-direction.x, -direction.y), 50);
						break;
					}
				}
				if (nDoormantSnapshot == 0) {
					SoundManager::Play("castoff");
					//summon all projectiles
					auto &soulItem = soulItems[usedSoulItem];
					SummonProjectiles(inv.GetData(soulItem.GetSpell().GetData(SpellParameters::nAmmo), SpellParameters::nAmmo), projectiles);
					
				}
				else {
					Vec2 direction = Vec2::GetDeltaVecNorm(*target, getCenterVec());
					if (abs(direction.x) > abs(direction.y)) {
						if (direction.x > 0) {
							Animation::Play("strike_right");
						}
						else if (direction.x < 0) {
							Animation::Play("strike_left");
						}
					}
					else {
						if (direction.y > 0) {
							Animation::Play("strike_bottom");
						}
						else if (direction.y < 0) {
							Animation::Play("strike_top");
						}
					}
				}
				break;
			}
			case SpellName::healPlayer:
				health += 1;
				break;
			case SpellName::quickDash: {
				/*if (inv.GetBonuses(ItemParameters::isFox)) {
					e.Add(EffectType::invulnerable, 10);
				}*/
				//kinda pain to figure out, but this is it i guess
				Dash(vel);
				//inv.GetData(0, SpellParameters::canPlayerDash)
				if (int(inv.GetData(0, SpellParameters::spellCastOnDash)) != 0) {
					Spell* sSpell = Spell::GetStoredSpell((SpellName)(int)inv.GetData(0, SpellParameters::spellCastOnDash));
					Vec2 direction = Vec2::GetDeltaVecNorm(*target, getCenterVec());
					sSpell->Cast(direction, projectiles,
						getCenterVec(),
						sSpell->GetData(SpellParameters::nProjectiles),
						(int)sSpell->GetData(SpellParameters::coneAngle),
						Parameters().Set<SoulItem*>(&soulItems[usedSoulItem]).Set<Player*>(this),
						false);
				}
				break; 
			}
			}
			isCasting = false;
			crntCastIndex = -1;
			target = nullptr;
			//attackSound.Play();
		}
	}
}

void Player::ProcessEffects(EffectGroup &e, Room &room) {
	isRooted = false;
	for (std::map<EffectType, Effect>::iterator it = e.effects.begin(); it != e.effects.end(); it++) {
		switch (it->first) {
		case EffectType::burning:
			if (!e.Has(EffectType::demon)) {
				health -= float(e.Get(EffectType::burning)->GetData(EffectParameters::damageP));
			}
			break;
		case EffectType::dash:
			isMoving = true;
			isInControll = false;
			vel = Vec2::GetDeltaVec(
				Vec2(e.Get(EffectType::dash)->GetData(EffectParameters::targetX),
					e.Get(EffectType::dash)->GetData(EffectParameters::targetY)),
				getCenterVec()) * 0.1;
			break;
		case EffectType::healing:
			health++;
			break;
		case EffectType::root:
			isRooted = true;
			break;
		}
	}
}

void Player::LookForTarget(Spell & spell_in, Room& room, MainWindow &wnd) {
	//indicates, wether we found a target in this method and there for we are able to proceed
	//in the casting process
	bool hasFoundTarget = false;
	switch (spell_in.castType) {
		case SpellCastType::Angle:
		case SpellCastType::Aim:
			//list of spells with "manual" casting
			if (spell_in.type == SpellName::shadowball) {
				if (wnd.kbd.KeyIsPressed(VK_UP) ||
					wnd.kbd.KeyIsPressed(VK_DOWN) ||
					wnd.kbd.KeyIsPressed(VK_LEFT) ||
					wnd.kbd.KeyIsPressed(VK_RIGHT)
					) {
					target = new Vec2(getCenterVec());
					int distanceFromPlayer = 100;
					if (wnd.kbd.KeyIsPressed(VK_UP)) {
						*target = *target - Vec2(0, distanceFromPlayer);
					}
					if (wnd.kbd.KeyIsPressed(VK_DOWN)) {
						*target = *target + Vec2(0, distanceFromPlayer);
					}
					if (wnd.kbd.KeyIsPressed(VK_LEFT)) {
						*target = *target - Vec2(distanceFromPlayer, 0);
					}
					if (wnd.kbd.KeyIsPressed(VK_RIGHT)) {
						*target = *target + Vec2(distanceFromPlayer, 0);
					}
					hasFoundTarget = true;
				}
			}
			else {
				//special rule for dash, target is the vel
				if (spell_in.type == SpellName::quickDash ||
					spell_in.type == SpellName::healPlayer) {
					//actually doesn't matter what so ever :)
					target = new Vec2(100, 100);
					hasFoundTarget = true;
				}
				else { //everything else (soul spells mostly)
					Vec2 temp = Vec2(getCenterVec()) + Vec2(0, -1);
					target = new Vec2(temp);
					hasFoundTarget = true;
				}
			}
			if (hasFoundTarget) {
				//when it finds the target we start with the spell casting process
					//if the spell has a cooldown we reset it
				if (spell_in.GetData(SpellParameters::cooldown) != 0) {
					spell_in.cdLeft = spell_in.GetData(SpellParameters::cooldown);
				}
				//reset the gcd
				gcd.cdLeft = gcd.GetData(SpellParameters::cooldown);
				//set the spell cast time
				spell_in.castTimeLeft = spell_in.GetData(SpellParameters::castTime);
			}
		break;
	}
}

void Player::OnHit(Spell & spell_in, Enemy & enemy_in)
{
	switch (spell_in.type) {
	case SpellName::reap:
		e.Add(EffectType::soul);
	}
	e.RefreshTime(EffectType::ignite);
}

void Player::OnCollision(Enemy * trgt)
{
	/*if (inv.GetBonuses(ItemParameters::isRam)) {
		if (!isInControll) {
			trgt->ProcessHit(4);
			trgt->health -= 1;
		}
	}*/
}

void Player::ProcessHit(Enemy * enemy_in, Spell * spell_in)
{
	SoundManager::Play(hitSound);
	if (!e.Has(EffectType::invulnerable)) {
		health -= spell_in->GetDamage() * enemy_in->GetDamage();
		isBeingHit = 5;
	}
}

void Player::AddSpell(Spell spell_in)
{
	spellbook.push_back(spell_in);
}

void Player::AddEffect(EffectType effectType_in, float timeLeft)
{
	e.Add(effectType_in, timeLeft);
}

void Player::ProcessItemUse(Item &item)
{
	
}

void Player::SetMainSpell(int spellId)
{
	if (spellbook.size() > spellId) {
		mainSpellId = spellId;
		//stop casting anyting
		if (target == nullptr) {
			crntCastIndex = -1;
		}
	}
}

void Player::ShakeScreen(int shakeIncrease)
{
	if (shakeIncrease == -1) {
		screenShake = screenShakeMax;
	}
	else {
		screenShake = shakeIncrease;
	}
}



bool Player::IsAction()
{
	if (isAction) {
		isAction = false;
		return true;
	}
	return false;

}

int Player::NDormant()
{
	int out = 0;
	for (auto &proj : projectiles) {
		if (proj.isDormant) {
			out++;
		}
	}
	return out;
}

void Player::Dash(Vec2 direction_in, int dashLength)
{
	float dashTolerance = 10;
	float dashK = 10;
	float nDashFrames = log10(dashTolerance / (dashLength / speed)) / log10((dashK - 1.0) / dashK);
	e.Add(EffectType::dash, nDashFrames);
	if (direction_in.GetLenght() != 0) {
		*(e.Get(EffectType::dash)->GetDataDirect(EffectParameters::targetX)) = 
			getCenterVec().x + (direction_in.GetNormalized().x * dashLength);
		*(e.Get(EffectType::dash)->GetDataDirect(EffectParameters::targetY)) = 
			getCenterVec().y + (direction_in.GetNormalized().y * dashLength);
	}
}

void Player::AttemptToDash()
{
	//da
	if (inv.GetData(0, SpellParameters::canPlayerDash) || true) {
		if (vel.GetLenghtSqrt() > 0.0f && resourceStamina >= spellbook[0].GetData(SpellParameters::resourceCostStamina)
			&& !isCasting) {
			if (gcd.cdLeft <= 0 || canSkipGcdForDash) {
				Dash(vel);
				resourceStamina -= spellbook[0].GetData(SpellParameters::resourceCostStamina);
				//inv.GetData(0, SpellParameters::canPlayerDash)
				if (inv.GetData(0, SpellParameters::spellCastOnDash) != 0) {
					Spell* sSpell = Spell::GetStoredSpell((SpellName)(int)inv.GetData(0, SpellParameters::spellCastOnDash));
					Vec2 direction = vel;
					sSpell->Cast(direction, projectiles,
						getCenterVec(),
						sSpell->GetData(SpellParameters::nProjectiles),
						(int)sSpell->GetData(SpellParameters::coneAngle),
						Parameters().Set<SoulItem*>(&soulItems[usedSoulItem]).Set<Player*>(this),
						false);
				}
				canSkipGcdForDash = false;
				gcd.cdLeft = gcd.GetData(SpellParameters::cooldown);
			}
		}
		//StartCasting(0);
	}
}

int Player::GetCoins()
{
	return coins;
}

void Player::AddCoins(float coins_in)
{
	coins += coins_in;
}

bool Player::RemoveCoins(float coins_in)
{
	if (coins < coins_in) {
		return false;
	}
	else {
		coins -= coins_in;
		return true;
	}
}

vector<ItemParameters> Player::GetUpgradableStats()
{
	return {ItemParameters::vitality,
			ItemParameters::castSpeedItem,
			ItemParameters::agility,
			ItemParameters::moveSpeed };
}

int Player::GetStatCost(int crntLevel)
{
	if (crntLevel < 2) {
		return 1;
	}
	if (crntLevel < 4) {
		return 2;
	}
	return 3;
}

void Player::Respawn(int toll_in)
{

	healthMax = 5-toll_in;
	resourceStamina = resourceStaminaMax;
	heals = toll_in;
	health = healthMax;
	projectiles.erase(projectiles.begin(), projectiles.end());
	SummonProjectiles(2, projectiles);

}

void Player::SetTarget(Vec2 &trgt) {
	target = &trgt;
}

void Player::MoveControls(MainWindow & wnd) {
	if (isInControll) {
		if (wnd.kbd.KeyIsPressed('W') ||
			wnd.kbd.KeyIsPressed('S') ||
			wnd.kbd.KeyIsPressed('A') ||
			wnd.kbd.KeyIsPressed('D')
			) {
			if (wnd.kbd.KeyIsPressed('W')) {
				isMoving = true;
				MoveInA(Vec2(0, -1));
				if (!wnd.kbd.KeyIsPressed('A') && !wnd.kbd.KeyIsPressed('D')) {
					a.SetAnimationType(AnimationType::RunUp);
				}
			}
			if (wnd.kbd.KeyIsPressed('S')) {
				isMoving = true;
				MoveInA(Vec2(0, 1));
				if (!wnd.kbd.KeyIsPressed('A') && !wnd.kbd.KeyIsPressed('D')) {
					a.SetAnimationType(AnimationType::RunDown);
				}
			}
			if (wnd.kbd.KeyIsPressed('A')) {
				isMoving = true;
				MoveInA(Vec2(-1, 0));
				a.SetAnimationType(AnimationType::RunLeft);
			}
			if (wnd.kbd.KeyIsPressed('D')) {
				isMoving = true;
				MoveInA(Vec2(1, 0));
				a.SetAnimationType(AnimationType::RunRight);
			}
		}
		else {
			if (!e.Has(EffectType::dash)) {
				isMoving = false;
			}
			a.SetAnimationType(AnimationType::Idle);
		}
	}
}


void Player::StartCasting(int spellIndex)
{
	//indicates, wether the spell can be cast
	bool isCast = false;
	if ( spellIndex < spellbook.size() ) {
		//checking basic resources and cooldowns
		if (!isCasting &&
			spellbook[spellIndex].cdLeft <= 0 &&
			heals >= spellbook[spellIndex].GetData(SpellParameters::resourceCostMana) &&
			resourceStamina >= spellbook[spellIndex].GetData(SpellParameters::resourceCostStamina) &&
			gcd.cdLeft <= 0)
		{
			//checking effects  & items
			if (e.GetStacks(EffectType::soul) >= spellbook[spellIndex].GetData(SpellParameters::nSoulsCost) &&
				e.GetStacks(EffectType::soul) >= spellbook[spellIndex].GetData(SpellParameters::nSoulsRequired) &&
				e.GetStacks(EffectType::ignite) >= spellbook[spellIndex].GetData(SpellParameters::nIgniteRequired)) {
				//checking extra resources
				if (0 != spellbook[spellIndex].GetData(SpellParameters::resourceCostFire)) {
					if (resourceMana >= spellbook[spellIndex].GetData(SpellParameters::resourceCostFire) && e.Has(EffectType::ignite)); {
						//set the current cast to this spell
						crntCastIndex = spellIndex;
					}
				}
				else {
					//super confusing way of checking the dash condition stuff
					if ((spellbook[spellIndex].type == SpellName::quickDash) <= (vel.GetLenght() != 0)) {
						//set the current cast to this spell
						crntCastIndex = spellIndex;
					}
				}
			}
		}
	}
}

void Player::ProcessKill(Enemy & enemy_in) {
	xp += 10;
	if (xp >= xpRequiredNext) {
		level++;
		xp -= xpRequiredNext;
		xpRequiredNext += xpRequiredNext;
		isCasting = false;
		crntCastIndex = -1;
		target = nullptr;
		//projectiles.clear();
		//increase stats
	}
}

int Player::GetStats(ItemParameters itemP_in) {
	//return the stats defaults to zero if stat not found
	int output = stats[itemP_in];
	//output += inv.GetBonuses(itemP_in);
	return output;
}