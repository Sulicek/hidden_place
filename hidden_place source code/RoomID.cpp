#pragma once
#include "RoomID.h"
#include "Room.h"

void RoomID::Load(int root, class Room & room) {
	if (!isCreated) {
		isCreated = true;
		vec.x = rand() % (room.width - width);
		vec.y = rand() % (room.height - height);
		c = Colors::Gray;
	}
};

void RoomID::Draw(Graphics &gfx, Vec2 roomPos) {
	//gfx.DrawRect(vec.x + roomPos.x, vec.y + roomPos.y, width, height, c);
	//gfx.DrawSprite(Vec2(vec.x + roomPos.x, vec.y + roomPos.y), width, height, surf);
};

RoomID::RoomID():surf("Resources\\Sprites\\WoodPlank_object_01.bmp"){

};