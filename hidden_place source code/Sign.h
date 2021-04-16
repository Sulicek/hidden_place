#pragma once
#include "Entity.h"
#include "Animation.h"
#include <queue>

class Sign : public Entity {
public:
	std::vector<std::string> text;
	std::vector<std::string> preload = {};
	Animation a;
	int page = 0;
	int prg = 0;
	bool isPlaying = false;
	bool isPaused = false;
	bool isLocked = false;
	bool isAnimPaused = false;
	Color coatC = Colors::Cyan;
	float coatOpacity = 0.3;
	std::string defaultTooltip = "Press \'E\' to talk";
public:
	Sign(Vec2 vec_in, int width_in, int height_in, std::vector<std::string> text_in, std::string surf_in);
	void Update(class Player &hero, std::queue<std::string>&textCommands_in);
	void Draw(Vec2 &pos_in, Font &font, Graphics &gfx);
	void ProcessCommand(std::string command_in, std::queue<std::string>&textCommands_in);
	void Preload(std::queue<std::string>&textCommands_in);
	void AddPreload(std::vector<std::string> text_in);
};