#pragma once
#include "Room.h"
#include <random>
#include "Graphics.h"
#include <chrono>
#include "json.hpp"
#include "Player.h"
#include <deque>
#include "RoomParser.h"
#include "Font.h"

using json = nlohmann::json;

using namespace std;

map<string, bool> Room::roomFlags = {};

void Room::LoadRoom(RoomType roomType_in, string gfxType_in, json roomJ, int level) {
	roomType = roomType_in;
	biom = gfxType_in;
	stepSound = "step_" + biom;
	//making the surface name out of the room type and the room biom
	surf = biom;
	switch (roomType) {
	case medium:
		surf += " medium";
		width = 800;
		height = 800;
		if (biom == "forest" | biom == "deep" | biom == "outskirts" | biom == "altered"
			| biom == "" | biom == "path" | biom == "kingdom") {
			drawRect = Rect(Vec2(width / 2, height / 2), width + width / 1, height + height / 1);
		}
		else {
			drawRect = Rect(Vec2(0, 0), width, height);
		}
		nIds = 5;
		break;
	case smallH:
		surf += " small horizontal";
		width = 800;
		height = 500;
		drawRect = Rect(Vec2(0, 0), width, height);
		nIds = 3;
		break;
	case smallV:
		surf += " small vertical";
		width = 500;
		height = 800;
		drawRect = Rect(Vec2(0, 0), width, height);
		nIds = 3;
		break;
	}
	//special rules for "mirror" biom
	if (biom == "mirror") {
		isMirrored = true;
	}
	if (biom == "mirror") {
		surf = "mirror";
		width = 400;
		height = 400;
		drawRect = Rect(Vec2(0, 0), width, height);
		nIds = 3;//?
	}

	if (roomJ.count("surf") > 0) {
		//overriding any setting with a direct setting of the surface
		surf = roomJ["surf"].get<std::string>();
		if (surf == "hut_inside") {
			width = 400;
			height = 400;
			drawRect = Rect(Vec2(0, 0), width, height);
		}
	}

	if (roomJ.count("lightLevel") > 0) {
		//overriding any setting with a direct setting of the surface
		lightLevel = roomJ["lightLevel"].get<float>();
	}
}

void Room::LoadEnemies(json j) {
	for (int i = 0; i < j.size(); i++) {
		int nSpawned = 1;
		if (j[i].count("n") > 0) {
			nSpawned = j[i]["n"];
		}
		for (int o = 0; o < nSpawned; o++) {
			Vec2 spawnLoc;
			if (j[i].count("pos")) {
				spawnLoc = RoomParser::GetEnemySpawnLoc(j[i]["pos"]);
			}
			else {
				spawnLoc = { j[i]["position"][0], j[i]["position"][1] };
			}
			enemies.insert(
				std::map<int, Enemy* > ::value_type(
					enemies.size(),
					new Enemy(
						Vec2((width / 100) * spawnLoc.x,
						(height / 100) * spawnLoc.y),
						j[i], biom)));
		}
	}
	for (auto en : enemies) {
		enemiesNative[en.first] = new Enemy(*en.second);
	}
}

void Room::LoadWalls(nlohmann::json j) {
	for (int o = 0; o < j.size(); o++) {
		walls.push_back(Capsule(
			{
				(float)j[o]["1"][0] * width / 100,
				(float)j[o]["1"][1] * height / 100
			},
				{
				 (float)j[o]["2"][0] * width / 100,
				 (float)j[o]["2"][1] * height / 100
				},
			j[o]["r"],
			Color(0, 0, 0)
		));
	}
}

void Room::LoadSigns(nlohmann::json j, nlohmann::json jTexts)
{
	for (auto &item : j) {
		string textSource = item["text"];
		Sign& newSign = AddSign(Vec2(item["position"][0], item["position"][1]), 
			jTexts[textSource], item["surf"]);
		if (item.count("preload") > 0) {
			newSign.AddPreload(jTexts[item["preload"].get<std::string>()]);
		}
	}
}

Sign& Room::AddSign(Vec2 vec_in, std::vector<std::string> text_in, std::string surf_in)
{
	Vec2 spawnLoc = Vec2(vec_in.x * width / 100, vec_in.y * height / 100);
	if (surf_in == "friend") {
		signs.push_back(Sign(spawnLoc, 80, 80, text_in, surf_in));
	}
	else if (surf_in == "soul") {
		signs.push_back(Sign(spawnLoc, 80, 80, text_in, surf_in));
	}
	else if (surf_in == "soul_red") {
		signs.push_back(Sign(spawnLoc, 80, 80, text_in, "soul"));
		Color signC = Colors::Red;
		signs[signs.size() - 1].coatC = signC;
		signs[signs.size() - 1].light.c = signC;
	}
	else if (surf_in == "soul_blue") {
		signs.push_back(Sign(spawnLoc, 80, 80, text_in, "soul"));
		Color signC = Colors::Blue;
		signs[signs.size() - 1].coatC = signC;
		signs[signs.size() - 1].light.c = signC;
	}
	else if (surf_in == "soul_green") {
		signs.push_back(Sign(spawnLoc, 80, 80, text_in, "soul"));
		Color signC = Colors::Green;
		signs[signs.size() - 1].coatC = signC;
		signs[signs.size() - 1].light.c = signC;
	}
	else if (surf_in == "soul_orange") {
		Color signC = Colors::LightOrange;
		signs.push_back(Sign(spawnLoc, 80, 80, text_in, "soul"));
		signs[signs.size() - 1].coatC = signC;
		signs[signs.size() - 1].light.c = signC;
	}
	else if (surf_in == "witch" || surf_in == "witchAltered") {
		Color signC = Colors::LightOrange;
		signs.push_back(Sign(spawnLoc, 96, 96, text_in, surf_in));
	}
	else if (surf_in == "hut" || surf_in == "hutAltered") {
		Color signC = Colors::LightOrange;
		signs.push_back(Sign(spawnLoc, 400, 400, text_in, surf_in));
	}
	else if (surf_in == "entrance") {
		Color signC = Colors::White;
		signs.push_back(Sign(spawnLoc, 400, 400, text_in, surf_in));
	}
	else {
		signs.push_back(Sign(spawnLoc, 100, 100, text_in, surf_in));
	}
	return signs[signs.size() - 1];
}

void Room::LoadChests(nlohmann::json j, int level){
	vector<Item> itemTemp;
	for (auto &chest : j) {
		//spawning the chest for each time we should
		for (int i = 0; i < chest["count"]; i++) {
			//checking the spawn chance of the room
			if (chest["chance"] >= (rand() % 100) + 1) {
				//loading in the items
				for (auto &item : chest["items"]) {
					//spawning adding an item for each time we should
					for (int o = 0; o < item["count"]; o++) {
						//checking the spawn chance for the item
						if (item["chance"] >= (rand() % 100) + 1) {
							//special "json codes" for making a spell or effect or normal item
							if (item["id"] < 1000) {
								//itemTemp.push_back(Item((ItemName)item["id"], level));
							}
							else if(item["id"] < 2000){
								//itemTemp.push_back(Item::GetSpellItem(SpellName((int)item["id"] - 1000)));
							} else if (item["id"] < 3000){
								//itemTemp.push_back(Item::GetEffectItem(EffectType((int)item["id"] - 2000), 50));
							}else if (item["id"] < 4000) {
								//temporarily removed - testing
								//itemTemp.push_back(Item::GetSoulItem(SpellName((int)item["id"] - 3000), nullptr, level));
							}
						}
					}
				}
				std::string chestSurf = "soul";
				//AddChest(Vec2((rand() % 70 + 10) * width / 100, (rand() % 70 + 10) *  height / 100), itemTemp, chestSurf);
				itemTemp.erase(itemTemp.begin(), itemTemp.end());
			}
		}
	}
}

void Room::AddChest(Vec2 vec, std::vector<Item> items_in, std::string chestSurf)
{
	if (chestSurf == "soul") {
	//	chests.push_back(Chest(vec, "chestHead", "chestBase", "chestMask", biom, items_in));
	}
}

Vec2 RotateRightEnemies(Vec2 source, int width, int height, int count) {
	switch (count) {
	case 1:
		return Vec2(height - source.y - 1, source.x);
		break;
	case 2:
		return Vec2(width - source.x - 1, height - source.y - 1);
		break;
	case 3:
		return Vec2(source.y, width - source.x - 1);
		break;
	}
}

void Room::Respawn(DoorType enterFrom)
{
	for (auto en : enemies) {
		delete en.second;
	}
	for (auto en : enemiesNative) {	
		enemies[en.first] = new Enemy(*en.second);
		//rotate the enemy based on the entry point
		int enemyWidth = enemies[en.first]->width;
		int enemyHeight = enemies[en.first]->height;
		switch (enterFrom) {
		case DoorType::left:
			enemies[en.first]->vec = RotateRightEnemies(enemies[en.first]->vec + Vec2(enemyWidth/2, enemyHeight / 2), 
				width, height, 3)- Vec2(enemyWidth / 2, enemyHeight / 2);
			break;
		case DoorType::right:
			enemies[en.first]->vec = RotateRightEnemies(enemies[en.first]->vec + Vec2(enemyWidth / 2, enemyHeight / 2),
				width, height, 1)- Vec2(enemyWidth / 2, enemyHeight / 2);
			break;
		}
	}
}

void Room::AddShrine()
{
	shrines.push_back(Shrine(Vec2(width / 2 - 100 / 2, height / 2 - 100/ 2)));
}

void Room::OnEnter(std::queue<string>&textCommands_in, double timePassed, DoorType enterFrom)
{
	bool save = true;
	bool heal = true;
	for (auto line : text) {
		if (line.substr(0, 1) == "!") {
			textCommands_in.push(line.substr(1));
		}
		else if (line.substr(0, 1) == "#") {
			if (line.substr(1) == "no save") {
				save = false;
			}
			if (line.substr(1) == "no heal") {
				heal = false;
			}
			ProcessCommand(line.substr(1), timePassed, textCommands_in);
		}
	}
	for (auto &sign : signs) {
		//sign.light.Off();
		sign.Preload(textCommands_in);
	}
	if (save) {
		textCommands_in.push("save");
	}
	if (heal) {
		textCommands_in.push("heal 5");
	}
	Respawn(enterFrom);
}

void Room::ProcessCommand(std::string command_in, double timePassed, std::queue<string>&textCommands_in)
{
	if (command_in == "show counter") {
		float decimals = timePassed - float(int(timePassed));
		int seconds = (int)timePassed % 60;
		int minutes = (int)timePassed / 60;
		floorText = to_string(minutes) + ":" + to_string(seconds) +  ":" + 
			to_string(decimals).substr(to_string(decimals).find(".")+1,to_string(decimals).find(".")+1 );
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
					ProcessCommand(newCommand.substr(1), timePassed, textCommands_in);
				}
				else if (newCommand.substr(0, 1) == "!") {
					textCommands_in.push(newCommand.substr(1));
				}
			}
		}
	}
	else if (command_in.substr(0, 4) == "text") {
			floorText = command_in.substr(5);
	}
}		


bool Room::IsCleared()
{
	/*if (forceSignUpdate) {
		return true;
	}*/
	for (auto enemy : enemies) {
		if (enemy.second->isActive) {
			return false;
		}
	}
	return true;
}

bool Room::DoSignUpdate()
{
	return forceSignUpdate || IsCleared();
}

void Room::UnlockAllSigns()
{
	for (auto &sign : signs) {
		sign.isLocked = false;
	}
}

void Room::EnableDamageForAllEnemies()
{
	for (auto &enemy : enemies) {
		enemy.second->noDamageTaken = false;
	}
}

void Room::GiveCommandRef(queue<string>* textCommands_in)
{
	for (auto &enemy : enemies) {
		enemy.second->textCommandsPointer = textCommands_in;
	}
}

bool Room::IsLocked()
{
	return isLocked;
}

void Room::LoadDoors(vector <Door> doors_in) {
	doors = doors_in;
}
Room::Room():pos(0, 0),
	drawRect(Vec2(0, 0), 0, 0)
{
}
void Room::Draw(Font font, Graphics &gfx, Player &hero) {
	//mirroring effect
	if (isMirrored) {
		for (int i = 0; i < 4; i++)
		{
			DrawAt(*this, i, pos, gfx, font, &hero);
		}
	}
	//gfx.DrawRect(pos.x, pos.y,width, height, Colors::BackgroundGreen);
	//Surface* asdf = (*sm)[surf];
	gfx.DrawSpritePortion(Vec2(pos.x, pos.y) - drawRect.offsetVec,
		drawRect.width, drawRect.height, Colors::Magenta, {0, 0},surf, 1,
		{ {Graphics::DrawSpecials::rotation, rotation}}, nullptr);
	int scale = 3;
	if (floorText != "") {
		if (floorText.find("\n") != string::npos) {
			font.Draw(floorText,
				pos + Vec2(width / 2, height / 2)
				- Vec2(floorText.substr(floorText.find("\n")).length() * font.getGlyphWidth() * scale / 2, 0)
				, Colors::Black, Colors::Magenta, scale, gfx);
		}
		else {
			font.Draw(floorText,
				pos + Vec2(width / 2, height / 2)
				- Vec2(floorText.length() * font.getGlyphWidth() * scale / 2, 0)
				, Colors::Black, Colors::Magenta, scale, gfx);
		}
	}
	for (int i = 0; i < ids.size(); i++) {
		ids[i].Draw(gfx, pos);
	}
	for (int i = 0; i < enemies.size(); i++) {
		enemies[i]->Draw(gfx, font, pos);
	}
	for (auto &wall : walls) {
		wall.Draw(pos, gfx);
	}
	//if (enemies.size() <= 0) {
	if (DoSignUpdate()) {
		for (auto& sign : signs) {
			sign.Draw(pos, font, gfx);
		}
	}
	if (IsCleared()) {
		for (int i = 0; i < doors.size(); i++) {
			doors[i].Draw(font, gfx, pos);
		}
		for (auto& chest : chests) {
			chest.Draw(pos, font, gfx);
		}
	}
}

void Room::DrawAt(Room &ogRoom, int doorType_in, Vec2 pos_in,
	Graphics &gfx, Font font, Player *hero_in) {
	Vec2 drawAtPos;
	switch (doorType_in) {
	case 0:
		drawAtPos.y = pos_in.y - height;
		drawAtPos.x = pos_in.x + ogRoom.width / 2 - width / 2;
		break;
	case 1:
		drawAtPos.y = pos_in.y + ogRoom.height;
		drawAtPos.x = pos_in.x + ogRoom.width / 2 - width / 2;
		break;
	case 2:
		drawAtPos.y = pos_in.y + ogRoom.height/2 - height / 2;
		drawAtPos.x = pos_in.x - width;
		break;
	case 3:
		drawAtPos.y = pos_in.y + ogRoom.height / 2 - height / 2;
		drawAtPos.x = pos_in.x + ogRoom.width;
		break;
	}


	//gfx.DrawRect(drawAtPos.x, drawAtPos.y,width, height, Colors::BackgroundGreen);
	//Surface* asdf = (*sm)[surf];
	gfx.DrawSprite(Vec2(drawAtPos.x, drawAtPos.y), width, height, surf);
	for (int i = 0; i < ids.size(); i++) {
		ids[i].Draw(gfx, pos);
	}
	for (int i = 0; i < enemies.size(); i++) {
		//*enemies[i].Draw(gfx, pos);
	}
	for (int i = 0; i < doors.size(); i++) {
		doors[i].Draw(font, gfx, drawAtPos);
	}
	if (&ogRoom == this) {
		hero_in->DrawAt(gfx, drawAtPos);
	}
}

void Room::DrawAtMap(int recLimit, int crntRec,Vec2 vec, Room & ogRoom, int doorType_in, 
		Vec2 pos_in, std::vector<class Room> rooms_in, Graphics & gfx, Font font, Player * hero_in)
{
	int factor = 10;
	int specialWidth = width / factor;
	int specialHeight = height / factor;
	Vec2 drawAtPos = Vec2(0, 0);
	switch (doorType_in) {
	case -1:
		drawAtPos.y = vec.y;
		drawAtPos.x = vec.x;
		break;
	case 0:
		drawAtPos.y = pos_in.y - specialHeight;
		drawAtPos.x = pos_in.x + ogRoom.width/(factor*2) - specialWidth / 2;
		break;
	case 1:
		drawAtPos.y = pos_in.y + ogRoom.height / (factor);
		drawAtPos.x = pos_in.x + ogRoom.width / (factor * 2) - specialWidth / 2;
		break;
	case 2:
		drawAtPos.y = pos_in.y + ogRoom.height / (factor * 2) - specialHeight/2;
		drawAtPos.x = pos_in.x - specialWidth;
		break;
	case 3:
		drawAtPos.y = pos_in.y + ogRoom.height / (factor * 2) - specialHeight / 2;
		drawAtPos.x = pos_in.x + ogRoom.width / (factor );
		break;
	}
	gfx.DrawRect(drawAtPos.x, drawAtPos.y, specialWidth, specialHeight, Color(38, 24, 46));
	int padding = 3;
	if (drawAtPos.x == vec.x && drawAtPos.y == vec.y) {
		gfx.DrawRect(drawAtPos.x + padding, drawAtPos.y + padding, specialWidth - padding*2, specialHeight - padding * 2, Color(43, 207, 202));
	}
	else {
		gfx.DrawRect(drawAtPos.x + padding, drawAtPos.y + padding, specialWidth - padding * 2, specialHeight - padding * 2, Color(30, 135, 132));
	}
	font.DrawNoBg(to_string(id), drawAtPos + Vec2(padding*2, padding * 2), gfx);
	crntRec++;
	if (recLimit > crntRec) {
		for (auto& door : doors) {
			if (door.rnChange != ogRoom.id && door.rnChange != -1) {
				rooms_in[door.rnChange].DrawAtMap(recLimit , crntRec, vec, *this, door.type, drawAtPos, rooms_in, gfx, font, hero_in);
			}
		}
	}
}
