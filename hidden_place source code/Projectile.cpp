#pragma once
#include "Projectile.h"
#include <vector>
#include "Enemy.h"
#include "Spell.h"
#include "Player.h"
#include "Room.h"
#include "Entity.h"
#include "Rect.h"
#include "Item.h"
#include "SoundManager.h"

//soul spell item constructor variant
Projectile::Projectile(Spell& spell_in, Vec2 vec_in,
	Vec2 direction, Parameters params, bool isDormant_in):a(spell_in.surf),
	l(50, -1, true,spell_in.c){
	hero = params.Get<Player*>();
	item = params.Get<SoulItem*>();
	inv = params.Get<Inventory*>();
	//spell for a reference
	spell = &spell_in;
	speed = GetData(SpellParameters::speed);
	isRebound = GetData(SpellParameters::reboundCount);
	//we make it moving
	isMoving = true;
	//we make it move in that direction
	Vec2 directionNew = direction;
	if (GetData(SpellParameters::oppositeDirection)) {
		directionNew = Vec2(-direction.x, -direction.y);
	}
	MoveIn(directionNew.GetNormalized());
	//set size
	width = spell_in.size;
	height = spell_in.size;
	//starting location (input is centre of the projectile)
	vec.x = vec_in.x - width / 2;
	vec.y = vec_in.y - height / 2;
	isDormant = isDormant_in;
	a.Load(spell->surf, 1, { AnimationType::Idle });
	a.AddMask("mask", "projectileMask");
	//l.Shine();
}

//drawing shit
void Projectile::Draw(Graphics &gfx, Vec2 pos){
	//a.Draw(vec, Rect(Vec2(0, 0), width, height), pos, gfx);
	//a.DrawMask("mask", vec, Rect(Vec2(0, 0), width, height), pos, spell->c, coatIntensity, gfx);
	l.Draw(getCenterVec() + pos, gfx, 20);
}

enum CollidedType {
	ct_none = 0, ct_top, ct_bottom, ct_left, ct_right
};

CollidedType ProcessCollision(Projectile &proj, Room &room) {
	CollidedType out = CollidedType::ct_none;
	int leeway = 0;
	if (proj.GetData(SpellParameters::isGhost)) {
		leeway = 2000;
	}
	if (proj.vec.x + proj.width >= room.width + leeway) {
		proj.vec.x = room.width - 1 - proj.width;
		out = CollidedType::ct_right;
	}
	if (proj.vec.x < 0 - leeway) {
		proj.vec.x = 0;
		out = CollidedType::ct_left;
		
	}
	if (proj.vec.y + proj.height >= room.height + leeway) {
		proj.vec.y = room.height - 1 - proj.height;
		out = CollidedType::ct_bottom;
	}
	if (proj.vec.y < 0 - leeway) {
		proj.vec.y = 0;
		out = CollidedType::ct_top;
	}
	return out;
}


void Projectile::OnExpire(std::vector<Projectile> &addProj)
{
	if ((int)GetData(SpellParameters::storedSpellExpire) != 0) {
		Spell* ss = Spell::GetStoredSpell(
			(SpellName)(int)GetData(SpellParameters::storedSpellExpire));
		Vec2 direction = vel;
		int nProjEx;
		if (inv != nullptr) {
			//running projectile number through the inventory
			nProjEx = inv->GetData(ss->GetData(SpellParameters::nProjectiles), SpellParameters::ex_nProjectiles);
		}
		else {
			nProjEx = ss->GetData(SpellParameters::nProjectiles);
		}
		SoundManager::Play("sendoff");
		ss->Cast(direction, addProj,
			getCenterVec(),
			nProjEx,
			(int)ss->GetData(SpellParameters::coneAngle), Parameters());

	}
	isActive = false;
}

//updating for enemy
void Projectile::Update(Player & hero, Enemy & enemy, Room & room, std::vector<Projectile> &addProj){


	//moving
	vec += vel * speed;
	distanceTraveled += speed;

	if (speed == 0) {
		vec = (hero.getCenterVec()- Vec2(width/2, height/2));
	}

	//expiration
	if (GetData(SpellParameters::rangeCap) > 0 &&
		GetData(SpellParameters::rangeCap) < distanceTraveled) {

		OnExpire(addProj);
	}

	//we clip it into the screen
	if (GetData(SpellParameters::storedSpellRebound) == 0 &&
		isRebound <= 0) {
		if ((bool)ProcessCollision(*this, room)) {
			isActive = false;
		}
	}
	else {
		bool collided = false;
		switch (ProcessCollision(*this, room)) {
		case CollidedType::ct_right:
		case CollidedType::ct_left:
			vel.x = -vel.x;
			collided = true;
			break;
		case CollidedType::ct_bottom:
		case CollidedType::ct_top:
			vel.y = -vel.y;
			collided = true;
			break;
		}
		if (collided) {
			if ((int)GetData(SpellParameters::storedSpellRebound) != 0) {
				Spell* ss = Spell::GetStoredSpell(
					(SpellName)(int)GetData(SpellParameters::storedSpellRebound));
				Vec2 direction = vel;
				ss->Cast(direction, addProj,
					getCenterVec(),
					ss->GetData(SpellParameters::nProjectiles),
					(int)ss->GetData(SpellParameters::coneAngle), Parameters());

			}
			isRebound--;
			if (isRebound < 0) {
				isActive = false;
			}
		}
	}
	
	switch (spell->castType) {
	case SpellCastType::Targetable:
		//it can only hit its pursue
		if (pursuitPlayer->isActive) {
			if (Vec2::IsColliding(vec, pursuitPlayer->vec, spell->size,
								spell->size, pursuitPlayer->width, 
								pursuitPlayer->height)) {
				hero.ProcessHit(&enemy, spell);
				isActive = false;
			}
		}
		break;
	case SpellCastType::Aim:
		//it can hit anything including nothing
			if (Vec2::IsColliding(vec, hero.vec, spell->size, spell->size, hero.width, hero.height)) {

				if ((int)GetData(SpellParameters::storedSpellHit) != 0) {
					Spell* ss = Spell::GetStoredSpell(
						(SpellName)(int)GetData(SpellParameters::storedSpellHit));
					Vec2 direction = vel;
					direction = direction.GetNormalized();
					int directionShiftInt = inv->GetData(ss->GetData(SpellParameters::directionShift), SpellParameters::directionShift);
					switch (directionShiftInt) {
					case 0:
						break;
					case 1:
						direction = Vec2(-direction.x, -direction.y);
						break;
					case 2:
						direction = Vec2(-direction.y, direction.x);
						break;
					case 3:
						direction = Vec2(direction.y, -direction.x);
						break;
					}

					ss->Cast(direction, addProj,
						getCenterVec(),
						inv->GetData(ss->GetData(SpellParameters::nProjectiles), SpellParameters::ex_nProjectiles),
						false, ss->GetData(SpellParameters::distanceFromCaster));
				}
				if (GetData(SpellParameters::isRoot)) {
					hero.AddEffect(EffectType::root, GetData(SpellParameters::rootTime));
				}
				hero.ProcessHit(&enemy, spell);
				isActive = false;
			}
		break;
	}
}

//updating for hero
void Projectile::Update(std::map<int, Enemy*> &enemies, Player &hero, Room & room, 
	std::vector<Projectile> &addProj) {
	//moving it according to the velocity based on the spell speed
	vec += vel * speed;
	distanceTraveled += speed;

	//expiration
	if (GetData(SpellParameters::rangeCap) > 0 &&
		GetData(SpellParameters::rangeCap) < distanceTraveled) {
		OnExpire(addProj);
	}

	if (speedTarget != 0) {
		speed += (speedTarget - speed) * 0.3;
	}

	if (isDormant && !GetData(SpellParameters::ss_independentDormant)) {
		//we clamp the dormant projectiles to the player
		vec += hero.vec-hero.lastHeroVec;
	}

	//we clip it into the screen
	if (GetData(SpellParameters::storedSpellRebound) == 0 && 
		isRebound <= 0 && 
		!isDormant) {
		if ((bool)ProcessCollision(*this, room)) {
			isActive = false;
		}
	}
	else {
		bool collided = false;
		switch (ProcessCollision(*this, room)) {
		case CollidedType::ct_right:
		case CollidedType::ct_left:
			vel.x = -vel.x;
			collided = true;
			break;
		case CollidedType::ct_bottom:
		case CollidedType::ct_top:
			vel.y = -vel.y;
			collided = true;
			break;
		}
		if (collided && !isDormant) {
			if ((int)GetData(SpellParameters::storedSpellRebound) != 0) {
				Spell* ss = Spell::GetStoredSpell(
					(SpellName)(int)GetData(SpellParameters::storedSpellRebound));
				Vec2 direction = vel;
				ss->Cast(direction, addProj,
					getCenterVec(),
					ss->GetData(SpellParameters::nProjectiles),
					(int)ss->GetData(SpellParameters::coneAngle), Parameters());

			}
			isRebound--;
			if (isRebound < 0) {
				isActive = false;
			}
		}
	}
	
	if (!isDormant) { //not hitting anything if we're dormant
		switch (spell->castType) {//legacy switch for unused mode
		case SpellCastType::Aim:
			//it can hit anything including nothing
			for (int i = 0; i < enemies.size(); i++) {
				if (Vec2::IsColliding(GetHitbox(), enemies[i]->GetHitbox())) {
					//if we haven't collided with that enemy  yet 
					if (collidedEnemies.count(&*enemies[i]) == 0) { //(for ss_guilt)
						if (enemies[i]->health > 0 && !room.noDamage) { //missing enemy on hit
							collidedEnemies.insert(&*enemies[i]);
							
							enemies[i]->ProcessHit(GetDamage());
							hero.OnHit(*spell, *enemies[i]);
							if (GetData(SpellParameters::notDeleteOnHit) < collidedEnemies.size()) {
								isActive = false;//delete on hit only if we are to delete it on hit
							}
							if ( 0 >=GetData(SpellParameters::returnOnHit) 
								- collidedEnemies.size()
								&& returnCount < GetData(SpellParameters::returnCount)) {
								vel = Vec2(-vel.x, -vel.y);
								returnCount++;
								collidedEnemies.erase(collidedEnemies.begin(), collidedEnemies.end());
								hasHitHero = false;
								collidedEnemies.insert(&*enemies[i]);
							}
							if (enemies[i]->health <= 0) {
								hero.ProcessKill(*enemies[i]);
							}
						}
					}
				}
			}
			//hitting hero
			if (Vec2::IsColliding(GetHitbox(), hero.GetHitbox())) {
				if (GetData(SpellParameters::addAmmoOnHeroHit)) {
					if (!hasHitHero) {
						hero.SummonProjectiles(1, addProj);
						hasHitHero = true;
					}
				}
			}
			else {
				hasHitHero = false;
			}
			break;
		}
	}
}

float Projectile::GetData(SpellParameters spellP_in)
{
	if(inv == nullptr){
		return spell->GetData(spellP_in);
	}
	else {
		return inv->GetData(spell->GetData(spellP_in),spellP_in);
	}
}

void Projectile::OnDeath() {
	if (!isDormant) {
		Animation::Show("disappear", getCenterVec());
	}
}


int Projectile::GetData(ItemParameters itemP_in)
{
	return 0;
	
}

float Projectile::GetDamage()
{
	if (item != nullptr) {
		return GetData(SpellParameters::damage);
	}
	else {
		return GetData(SpellParameters::damage);
	}
}

int Projectile::SetData(SpellParameters spellP_in, int data_in)
{
	return data[spellP_in] = data_in;
}

void Projectile::SetRebound(int rebound_in)
{
	isRebound = rebound_in;
}

void Projectile::SetDormant()
{
	isDormant = true;
}

void Projectile::SetActive()
{
	if (!isTimed) {
		isDormant = false;
	}
}

bool Projectile::SetOut(Vec2 target, Player & player, Vec2 *bait_in)
{
	if (isDormant) {
		if (GetData(SpellParameters::storedSpell) != 0) {
				isActive = false;
				return true;
		}
		else {
			SetActive();
			speedTarget = speed;
			speedTarget = GetData(SpellParameters::setOutSpeed);
			//if (spell->type != SpellName::ss_ritual) {
			if (bait_in != nullptr && (bool)GetData(SpellParameters::isBaited)) {
				bait = bait_in;
			}
			else {
				if (GetData(SpellParameters::ss_isAccurate) > 0) {
					MoveIn(Vec2::GetDeltaVecNorm(target, player.getCenterVec()));
				}
				else {
					MoveTo(target);
				}
			}
			/*}
			else {
				vel = Vec2(-vel.x, -vel.y);
			}*/
		}
	}
	return false;
}
