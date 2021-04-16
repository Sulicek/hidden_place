#include "Shrine.h"
#include "Player.h"
#include "Effect.h"
#include "Enemy.h"

using namespace std;

void Shrine::Draw(Vec2 & pos, map<string, Surface*> *sm, Graphics & gfx) {
//	gfx.DrawSpritePortion(vec + pos, width, height, Colors::Magenta, { 0, 0 }, innerC, *(*sm)[maskSurf]);
	//gfx.DrawSprite(vec + pos, width, height, *(*sm)[surf]);
}

Shrine::Shrine(Vec2 vec_in) {
	vec = vec_in;
	width = 100;
	height = 100;
	surf = "Resources\\Sprites\\DungeonHole_object_01.bmp";
	maskSurf = "Resources\\Sprites\\DungeonHole_mask_01.bmp";
}

void Shrine::Update(class Player &  player_in, std::vector<Enemy> &enemies)
{
	int range = 100;
	if (Vec2::GetDeltaVec(player_in.getCenterVec(), getCenterVec()).GetLenght() <= range){
		player_in.AddEffect(EffectType::canChangeSpells, 1);
	}
	//collision for enemies
	//for (int i = 0; i < enemies.size(); i++) {
	//	if (Vec2::IsColliding(*enemies[i].getCenterVec(), getCenterVec(),
	//		*enemies[i].width / 2, Shrine::width / 2)
	//		) {
	//		Vec2 deltaVec = Vec2::GetDeltaVec(*enemies[i].getCenterVec(), getCenterVec());
	//		//how much we need to move them apart
	//		float moveIndex = deltaVec.GetLenght() - (*enemies[i].width / 2 + Shrine::width / 2);
	//		//only moving the enemies
	//		*enemies[i].vec = *enemies[i].vec - (deltaVec.GetNormalized() * moveIndex);
	//	}
	//}
	//collision for hero
	if (Vec2::IsColliding(player_in.getCenterVec(), getCenterVec(),
		player_in.width / 2, Shrine::width / 2)
		) {
		Vec2 deltaVec = Vec2::GetDeltaVec(player_in.getCenterVec(), getCenterVec());
		//how much we need to move them apart
		float moveIndex = deltaVec.GetLenght() - (player_in.width / 2 + Shrine::width / 2);
		//only moving the enemies
		player_in.vec = player_in.vec - (deltaVec.GetNormalized() * moveIndex);
	}
}