#pragma once
#include <fstream>
#include <string>
#include "Room.h"
#include "Door.h"
#include <map>
#include "json.hpp"
#include <deque>

using json = nlohmann::json;

class RoomParser {
	public:
		static vector<Room> LoadModel(json source, json jTexts);
		static Vec2 GetEnemySpawnLoc(string loc_in);
};