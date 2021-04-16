#pragma once
#include "Door.h"
#include "Room.h"
#include "Entity.h"
#include "Player.h"
#include "Font.h"
#include <string>

Door::Door(int type_in, int rnChange_in, Room & room, std::string nextLoad_in):
	a("doorMask"),
	light(300, -1, false, Colors::Yellow){
	rnChange = rnChange_in;
	tooltip = "Press \'E\' to enter";
	width = 100;
	height = 100;
	size = 10;
	type = type_in;
	nextLoad = nextLoad_in;
	coat = Color(255, 255, 255);
	coatOpacity = 0.1;
	if (nextLoad != "none") {
	}
	a.Load("doorMask", 8, { AnimationType::Idle });
	a.AddMask("doorMaskBottom", "doorMaskBottom");
	a.AddMask("doorMaskLeft", "doorMaskLeft");
	a.AddMask("doorMaskRight", "doorMaskRight");
	a.AddMask("doorMaskTop", "doorMaskTop");
	c = Color(109, 54, 38);
	switch (type_in) {
	case DoorType::top:
		vec.x = room.width / 2 - width / 2;
		vec.y = 0;
		break;
	case DoorType::bottom:
		vec.x = room.width / 2 - width / 2;
		vec.y = room.height - height;
		break;
	case DoorType::left:
		vec.x = 0;
		vec.y = room.height / 2 - height / 2;
		break;
	case DoorType::right:
		vec.x = room.width - width;
		vec.y = room.height / 2 - height / 2;
		break;
	}
}
int Door::Update(Player &hero) {
	a.Update();
	isTooltip = false;
	if (Vec2::GetDeltaVec(hero.getCenterVec(), getCenterVec()).GetLenght() <= 100) {
		isTooltip = true;
		if (nextLoad != "none") {
			//changing the tooltip to reflect the nature of the door
			tooltip = "Press \'E\' to proceed";
		}
		else {
			//changing the tooltip to reflect the nature of the door
			tooltip = "Press \'E\' to proceed";
		}
		if (hero.IsAction()) {
			if (nextLoad != "none") {
				return -2; //load another dung
			}
			return rnChange;
		}
	}
	return -1; //no detection
}
void Door::Draw(Font &font, Graphics & gfx, Vec2 pos) {
	Vec2 drawVec;
	switch (type) {
	case DoorType::top:
		drawVec = vec + Vec2(width / 2, height/2);
		break;
	case DoorType::bottom:
		drawVec = vec + Vec2(width / 2, height / 2);
		//a.DrawMask("doorMaskTop", vec, Rect(Vec2(0, 0), width, height), pos, coat, coatOpacity, sm, gfx);
		//a.DrawMask("doorMaskBottom", vec , Rect(Vec2(0, 0), width, height), pos, coat, coatOpacity, sm, gfx);
		break;
	case DoorType::left:
		drawVec = vec + Vec2(width / 2, height / 2);
		break;
	case DoorType::right:
		drawVec = vec + Vec2(width / 2, height / 2);
		//a.DrawMask("doorMaskLeft", vec, Rect(Vec2(0, 0), width, height), pos, coat, coatOpacity, sm, gfx);
		//a.DrawMask("doorMaskRight", vec, Rect(Vec2(0, 0), width, height), pos, coat, coatOpacity, sm, gfx);
		break;
	}
	//light.Draw(vec + pos, gfx);
	gfx.DrawLight(drawVec + pos, 300, 0.3, Colors::Yellow, 
		{
		{Graphics::DrawSpecials::jagged, 3},
		{Graphics::DrawSpecials::squary, 5}
		} 
	);
	DrawTooltip(pos, font, gfx);
}
//top bot left right
//- Vec2(0, height - size)