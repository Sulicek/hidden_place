#pragma once
#include <map>
#include <string>
#include <typeindex>
#include "json.hpp"
#include "Player.h"

using nlohmann::json;
using nlohmann::json;
using namespace std;

class Saver {
public:
	string saveFile = "Resources\\save.json";
	void Save(Player& hero, int rn_in, string level_in, int entryDirection,
		bool debug) {
		fstream input;
		input.open(saveFile);
		json j = json::parse(input);
		input.close();
		j["saveCount"] = j["saveCount"] + 1;
		j["rn"] = rn_in;
		j["direction"] = entryDirection;
		j["level"] = level_in;
		j["debug"] = int(debug);
		j["pointsLeft"] = hero.statPointsToAllocate;
		j["deathCount"] = hero.deathCount;
		//write stats
		for (auto stat : hero.GetUpgradableStats()) {
			j["stats"][to_string(int(stat))] = vector<int>({ int(stat), hero.GetStats(stat) });
		}
		vector<int> itemIds;
		for (auto item : hero.inv.items) {
			itemIds.push_back(item.second.id);
		}
		j["inv"] = itemIds;
		fstream output;
		output.open(saveFile, ofstream::trunc | ofstream::out);
		output << j;
		output.close();
	};
	json GetSavedData() {
		fstream input;
		input.open(saveFile);
		json j =  json::parse(input);
		input.close();
		return j;
	};


};