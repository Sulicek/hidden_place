#pragma once
#include "Sign.h"
#include "Player.h"
#include "Room.h"

using namespace std;

Sign::Sign(Vec2 vec_in, int width_in, int height_in, std::vector<std::string> text_in, string surf_in)
	:
	a(surf_in)
{
	text = text_in;
	width = width_in;
	height = height_in;
	surf = surf_in;
	vec = vec_in - Vec2(width_in/2,height_in/2);
	if (surf_in == "friend") {
		a.Load(surf_in, 20, { AnimationType::Idle });
	}
	if (surf_in == "soul" || surf_in == "hut" || surf_in == "hutAltered" || surf_in == "entrance") {
		a.Load("chestHead", 10, { AnimationType::Idle });
		a.AddMask("mask", "chestMask");
		c = coatC;
		defaultTooltip = "";
	}
	if (surf_in == "witch" || surf_in == "witchAltered") {
		a.Load(surf_in, 8, { AnimationType::Idle });
		a.tempMax = 30;
	}
	light.c = coatC;
	light.Off();
}

void Sign::ProcessCommand(string command_in, queue<string>&textCommands_in)
{
	if (command_in == "pause") {
		isAnimPaused = true;
	}
	else if (command_in == "play") {
		isAnimPaused = false;
	}
	else if (command_in == "lon") {
		light.On();
	}
	else if (command_in == "loff") {
		light.Off();
	}
	else if (command_in == "lt") {
		light.Shine();
	}
	else if (command_in == "lf") {
		light.NotShine();
	}
	else if (command_in == "lock") {
		isLocked = true;
	}
	else if (command_in.substr(0, 2) == "if") {
		//command structure
		//command_in: if someCondition !someCommand someCommandParameter
		//command_in: if someCondition #someCommand someCommandParameter
		string condition = command_in.substr(3, command_in.substr(3).find(" "));
		//condition: someCondition
		string newCommand = command_in.substr(command_in.substr(3).find(" ") + 1 + 3);
		//newCommand: !someCommand someCommandParameter
		//newCommand: #someCommand someCommandParameter
		if (Room::roomFlags.count(condition) > 0) {
			if (Room::roomFlags[condition]) {
				if (newCommand.substr(0, 1) == "#") {
					ProcessCommand(newCommand.substr(1), textCommands);
				}
				else if (newCommand.substr(0, 1) == "!") {
					textCommands_in.push(newCommand.substr(1));
				}
			}
		}
	}
}

void Sign::Preload(std::queue<std::string>&textCommands_in)
{
	for (auto text : preload) {
		if (text.substr(0, 1) == "!") {
			textCommands_in.push(text.substr(1));
		}
		else if (text.substr(0, 1) == "#") {
			textCommands.push(text.substr(1));
		}
	}
}

void Sign::AddPreload(std::vector<std::string> text_in)
{
	preload = text_in;
}

void Sign::Update(Player &hero, queue<string>&textCommands_in ){
	if (!isAnimPaused) {
		a.Update();
	}
	isTooltip = false;
	if (Vec2::GetDeltaVec(hero.getCenterVec(), getCenterVec()).GetLenght() <= 100) {
		if (!isLocked) {
			tooltip = defaultTooltip;
		}
		else {
			tooltip = "";
		}
		isTooltip = true;
		if (hero.IsAction() && !isLocked) {
			if (isPlaying) {
				isPaused = false;   
				page++;
				prg = 0;
			}
			else {
				isPlaying = true;
			}
		}
	}
	if (page == text.size()) {
		page = 0;
		isPlaying = false;
	}
	if (isPlaying && page < text.size()) {
		isTooltip = true;
		//checking for commands
		if (text[page].substr(0, 1) == "!") {
			textCommands_in.push(text[page].substr(1));
			tooltip = " ";
			//found a command and we skip all of this stuff :>
			isPaused = false;
			page++;
			prg = 0;
		}
		else if(text[page].substr(0, 1) == "#") {
			textCommands.push(text[page].substr(1));
			tooltip = " ";
			//found a command and we skip all of this stuff :>
			isPaused = false;
			page++;
			prg = 0;
		}
		else {
			tooltip = text[page].substr(0, prg + 1);
			int spacesMissing = text[page].size() - tooltip.size();
			for (int i = 0; i < spacesMissing; i++) {
				tooltip += " ";
			}
			if (!isPaused) {
				prg++;
				if (prg == text[page].size()) {
					isPaused = true;
				}
			}
		}
	}
	while (textCommands.size() > 0) {
		ProcessCommand(textCommands.front(), textCommands_in);
		textCommands.pop();
	}
}

void Sign::Draw(Vec2 &pos_in, Font &font, Graphics &gfx) {
	//light.Draw(getCenterVec() + pos_in, gfx);
	if (surf == "friend" || surf == "soul" || surf == "witch" || surf == "witchAltered") {
		a.Draw(vec, Rect(Vec2(0, 0), width, height), pos_in, gfx);
		a.DrawMask("mask", vec, Rect(Vec2(0, 0), width, height), pos_in,
			coatC, coatOpacity, gfx);
	}
	else {
		gfx.DrawSprite(vec + pos_in, width, height, surf);
	}
	DrawTooltip(pos_in, font, gfx);
}