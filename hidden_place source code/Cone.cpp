#include "Cone.h"
#include "Vec2.h"
#include "Enemy.h"
#include "Player.h"
#include "Room.h"
#define M_PI 3.14159265358979323846 

Cone::Cone(Vec2 vec_in, Vec2 edge_in, Spell &spell_in)
{
	vec = vec_in;
	p1 = edge_in;
	spell = &spell_in;
	c = Colors::White;
	int r = spell->GetData(SpellParameters::coneR);
	p2 = 
		Vec2::GetVecByAngle(Vec2::GetDeltaVec(p1, vec_in).GetAngle() + 
		float(spell->GetData(SpellParameters::coneAngle)) * M_PI / float(180)).GetNormalized()
		* r + vec_in;
	spike = p1 + Vec2::GetDeltaVec(p2, vec_in);
}


int Cone::GetRadius()
{
	return int(Vec2::GetDeltaVec(p1, vec).GetLenght());
}

Vec2 Cone::GetVec()
{
	return vec;
}

void Cone::Draw(Vec2 pos, Graphics & gfx)
{
	for (int i = 0; i < 3; i++) {
		gfx.DrawConeEdge(vec + pos, p1 + pos, spell->GetData(SpellParameters::coneAngle), 0, 
			Vec2::GetDeltaVec(vec, p1).GetLenght() - i * i
			- fadingTimer * (Vec2::GetDeltaVec(vec, p1).GetLenght() / 10), c);
	}
	
}

void Cone::Update(std::vector<Enemy>& enemies, Player & hero, Room & room, bool isParry){
	
}
void Cone::Update(Player &hero, Enemy & enem, Room & room) {
	if (!isFading) {
		if (Vec2::IsColliding(hero.vec, hero.width, hero.height, *this)) {
			hero.ProcessHit(&enem, spell);
		}
	}
	else {
		fadingTimer--;
		if (fadingTimer == 0) {
			isActive = false;
		}
	}
	isFading = true;
}
