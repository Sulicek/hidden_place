#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include "Colors.h"
#include <vector>
#include <string>
#include <map>
#include "Light.h"
#include "Rect.h"
#include <queue>
#include <string>

class Entity {
protected:
public:
	Vec2 vec;
	Vec2 vel = Vec2(0, 0);
	Vec2 acc = Vec2(0, 0);
	Light light;
	std::string hitSound = "hit";
	Rect hitBox = Rect(Vec2(0, 0), 0, 0);
	Color c = Colors::White;
	std::queue<std::string> textCommands;
	std::string tooltip;
	std::string surf;
	int width = 30, height = 30;
	float drawnHealth = 0;
	float speed = 1;
	bool isMoving = false;
	bool isTooltip = false;
	bool hasHitbox = false;
	bool isDead = false;
	float drawHealth = 0;
public:
	Entity();
	Entity(Vec2 vec_in, int width_in, int height_in);
	void Draw(Graphics &gfx, Vec2 pos);
	void DrawTooltip(Vec2 pos, class Font &font, Graphics &gfx);
	void DrawBacklight(Vec2 pos, Graphics &gfx);
	void ProcessCommand(std::string command_in);
	template <typename T>
	void ProcessCommands(T* obj) {
		while (textCommands.size() > 0) {
			obj->ProcessCommand(textCommands.front());
			textCommands.pop();
		}
	};
	void MoveTo(Vec2 vec_in);
	void MoveToA(Vec2 vec_in);
	void MoveToAMagnetic(Vec2 vec_in, float strength);
	void MoveIn(Vec2 direction);
	void MoveInA(Vec2 direction);
	Rect GetHitbox();
	Vec2 getCenterVec();
	void Update();
	void Draw(Vec2 pos, Font font, Graphics & gfx);
	//get it only ONCE per cycle!!!
	float GetDrawHealth(int health_in);
};