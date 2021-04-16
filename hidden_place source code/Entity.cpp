#pragma once
#include "Entity.h"
#include "Colors.h"
#include "Vec2.h"
#include <iostream>
#include "Graphics.h"
#include "Font.h"

using namespace std;

Entity::Entity() :vec(10, 10), light(500, -1, false, c) {
}

Entity::Entity(Vec2 vec_in, int width_in, int height_in) :
	light(500, -1, false, c)
{
	vec.x = vec_in.x;
	vec.y = vec_in.y;
	width = width_in;
	height = height_in;
}

void Entity::Draw(Graphics &gfx, Vec2 pos) {
	gfx.DrawRect(vec.x + pos.x, vec.y + pos.y, width, height, c);
}

void Entity::DrawTooltip(Vec2 pos, Font & font, Graphics & gfx)
{
	int padding = 5;
	if (isTooltip) {
		font.DrawBg(tooltip, getCenterVec() - Vec2(tooltip.size() * font.getGlyphWidth() / 2,
			height / 2 + font.getGlyphHeight() + padding) + pos, gfx);
	}
}

void Entity::DrawBacklight(Vec2 pos, Graphics & gfx)
{
	if (isTooltip) {
		light.On();
		//draw the light
		//gfx.DrawLight(getCenterVec() + pos, c);
	}
	else {
		light.Off();
	}
	light.Draw(getCenterVec() + pos, gfx);
}

void Entity::ProcessCommand(std::string command_in)
{
}


void Entity::MoveTo(Vec2 vec_in) {
	//delta vector
	//we're substracting main vec from the parameter cuz we are moving the
	//main vec. It's kinda verid but if you think about it it starts making
	//sense
	if (vec_in != vec) {//if we aren't there
		Vec2 del(vec_in.x - getCenterVec().x, vec_in.y - getCenterVec().y);
		Vec2 delNorm = del.GetNormalized();
		vel.x = delNorm.x;
		vel.y = delNorm.y;
		//if we are close near enough we stop moving
	}

}

void Entity::MoveToA(Vec2 vec_in) {
	//delta vector
	//we're substracting main vec from the parameter cuz we are moving the
	//main vec. It's kinda verid but if you think about it it starts making
	//sense
	Vec2 del(vec_in.x - vec.x, vec_in.y - vec.y);
	Vec2 delNorm = del.GetNormalized();
	vel.x += delNorm.x;
	vel.y += delNorm.y;
	vel = vel.GetNormalized();
}

void Entity::MoveToAMagnetic(Vec2 vec_in, float strength) {
	if (strength < 1) {
		strength = 1;
	}
	//strength is at which distance it does sortof an instant 180
	//delta vector
	//we're substracting main vec from the parameter cuz we are moving the
	//main vec. It's kinda verid but if you think about it it starts making
	//sense
	if (vec_in.x - getCenterVec().x != 0 &&
		vec_in.y - getCenterVec().y != 0 &&
		strength != 0) {
		Vec2 del(vec_in.x - getCenterVec().x, vec_in.y - getCenterVec().y);
		//direction to the vector
		Vec2 delNorm = del.GetNormalized();
		//we make the actual velocity bigger based on the distance and the strength of the "magnetism"
		vel = vel * (del.GetLenght() / strength);
		vel.x += delNorm.x;
		vel.y += delNorm.y;
		vel = vel.GetNormalized();
	}
}

void Entity::Update() {
	//not moving if we're out
	if (vec.x + width >= Graphics::ScreenWidth || vec.x < 0 ||
		vec.y + height >= Graphics::ScreenHeight || vec.y < 0) {
		isMoving = false;
	}

	//not moving if not moving 
	if (isMoving == false) {
		vel.x = 0;
		vel.y = 0;
	}
	//adds up velocities
	vec.x += vel.x * speed;
	vec.y += vel.y * speed;
	//don't go out of screen
	while (vec.x + width >= Graphics::ScreenWidth) {
		vec.x--;
		isMoving = false;
	}
	while (vec.x <= 0) {
		vec.x++;
		isMoving = false;
	}
	while (vec.y + height >= Graphics::ScreenHeight) {
		vec.y--;
		isMoving = false;
	}
	while (vec.y <= 0) {
		vec.y++;
		isMoving = false;
	}
}

void Entity::MoveIn(Vec2 direction) {

	vel.x = direction.x;
	vel.y = direction.y;
}

void Entity::MoveInA(Vec2 direction) {

	vel.x += direction.x;
	vel.y += direction.y;
	if (vel.x > 1 | vel.x < -1 | vel.y > 1 | vel.y < -1) {
		vel = vel.GetNormalized();
	}
}



Rect Entity::GetHitbox()
{
	if (hasHitbox) {
		return hitBox + vec;
	}
	else {
		return Rect(vec, width, height);
	}
}

Vec2 Entity::getCenterVec()
{
	return Vec2(vec.x + width / 2, vec.y + height / 2);
}

void Entity::Draw(Vec2 pos, Font font, Graphics & gfx)
{
	DrawBacklight(pos, gfx);
	gfx.DrawSprite(vec + pos, width, height, surf);
	DrawTooltip(pos, font, gfx);
}

float Entity::GetDrawHealth(int health_in)
{
	if (drawnHealth != health_in) {
		drawnHealth += (health_in - drawnHealth) / 2;
	}

	return drawHealth;
}
