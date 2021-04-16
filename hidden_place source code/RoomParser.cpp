#pragma once
#include "RoomParser.h"
#include <fstream>
#include <string>
#include <assert.h>
#include "Door.h"
#include <deque>
#include <vector>

using namespace std;

DoorType GetNextDoor(vector<DoorType> nextDoors_in) {
	return nextDoors_in[rand() % nextDoors_in.size()];
};

vector<Room> RoomParser::LoadModel(json source, json jTexts)
{
	//eumerations that we're using

	/*typeMap["bv"] = Room::RoomType::bigV;
	typeMap["sv"] = Room::RoomType::smallV;
	typeMap["sh"] = Room::RoomType::smallH;
	typeMap["bh"] = Room::RoomType::bigH;
	typeMap["m"] = Room::RoomType::medium;
	doorTypeMap["left"] = DoorType::left;
	doorTypeMap["right"] = DoorType::right;
	doorTypeMap["top"] = DoorType::top;
	doorTypeMap["bottom"] = DoorType::bottom;*/

	vector<Room> out;
	map<std::string, Room> rooms;
	DoorType lastDoor = DoorType::top;
	//where we can put door if last door was x
	map<DoorType, vector<DoorType>> nextDoor = {
		{DoorType::top,{DoorType::left,DoorType::right,DoorType::top}},
		{DoorType::left,{DoorType::left,DoorType::top, DoorType::top}},
		{DoorType::right,{DoorType::right,DoorType::top, DoorType::top}},
	};
	map<DoorType, vector<DoorType>> nextDoorHard = {
		{DoorType::top,{DoorType::left,DoorType::right}},
		{DoorType::left,{DoorType::left}},
		{DoorType::right,{DoorType::right}},
	};
	std::map<std::string, json> temp = source["rooms"].get<std::map<std::string, json>>();
	for (auto room : temp)
	{
		Room &crntRoom = rooms[room.first];
		rooms[room.first] = Room();
		rooms[room.first].LoadRoom(Room::RoomType::medium, 
			source["config"]["type"], source["rooms"][room.first]);

		if (source["rooms"][room.first].count("text")) {
			if (source["rooms"][room.first]["text"].is_array()) {
				std::vector<std::string> textsForRoom;
				for (auto textName : source["rooms"][room.first]["text"].get<vector<string>>()) {
					for (auto textLine : jTexts[textName].get<std::vector<std::string>>()) {
						textsForRoom.push_back(textLine);
					}
				}
				rooms[room.first].text = textsForRoom;
			}
			else {
				rooms[room.first].text = 
				jTexts[source["rooms"][room.first]["text"].get<std::string>()].get<std::vector<std::string>>();
			}
		}

		//rooms[room.first].LoadChests(source["rooms"][room.first]["chests"]);
		rooms[room.first].LoadEnemies(source["rooms"][room.first]["enemies"]);
		if (source["rooms"][room.first].count("signs") > 0) {
			rooms[room.first].LoadSigns(source["rooms"][room.first]["signs"], jTexts);
		}
	}
	if (source["path"][0].get<std::string>() != "procedural") {
		int nRooms = source["path"].size();
		//non-procedural
		for (int i = 0; i < nRooms; i++) {
			out.push_back(rooms[source["path"][i]]);
			out[i].id = i;
			string nextLoad = "none";
			if (source["rooms"][source["path"][i].get<std::string>()].count("nextLoad")) {
				nextLoad = source["rooms"][source["path"][i].get<std::string>()]["nextLoad"].get<string>();
				out[i].LoadDoors({ Door(0, -1, out[i], nextLoad) });
			}
			else if (i + 1 < nRooms) {
				if (source["rooms"][source["path"][i + 1].get<std::string>()].count("nextLoad")) {
					//enter the last room from the bottom
					lastDoor = DoorType::top;
				}
				else if (source["rooms"][source["path"][i + 1].get<std::string>()].count("hard")) {
					//enter the hard room from the side (harder cuz hitbox shit
					lastDoor = GetNextDoor(nextDoorHard[lastDoor]);
				}
				else {
					lastDoor = GetNextDoor(nextDoor[lastDoor]);
				}
				out[i].LoadDoors({ Door(lastDoor, i + 1, out[i], nextLoad) });
			}
			//fitting door location to a sign
			if (source["rooms"][source["path"][i].get<std::string>()].count("fitDoorTo")) {
				int fitDoorTo = source["rooms"][source["path"][i].get<std::string>()]["fitDoorTo"][0].get<int>();
				Vec2 offsetFromSign = Vec2(
					source["rooms"][source["path"][i].get<std::string>()]["fitDoorTo"][1][0],
					source["rooms"][source["path"][i].get<std::string>()]["fitDoorTo"][1][1]
				);
				out[i].doors[out[i].doors.size() - 1].vec = out[i].signs[fitDoorTo].getCenterVec()
					+ offsetFromSign;
			}
		}
	}
	else {
		//procedural, currently out of order
		int nRooms = 30;
		int nForUpgrade = 3;
		vector<int> nRoomsForSpecial = {3, 2};
		int crntLevel = 0;
		int specialCounter = 0;
		int levelCounter = 0;
		for (int i = 0; i < nRooms; i++) {
			int chosenRoomId = rand() % source["roomSets"][crntLevel].size();
			Room &chosenRoom = rooms[source["roomSets"][crntLevel][chosenRoomId]];
			json &chosenRoomJ = source["rooms"][source["roomSets"][crntLevel][chosenRoomId].get<std::string>()];

			if (specialCounter == 0) { //down-counting counter
				specialCounter = nRoomsForSpecial[rand() % nRoomsForSpecial.size()];
				if (i != 0) { //initial reset
					chosenRoomId = rand() % source["specialRooms"].size();
					chosenRoom = rooms[source["specialRooms"][chosenRoomId]];
					chosenRoomJ = source["rooms"][source["specialRooms"][chosenRoomId].get<std::string>()];
				}
			}
			else {
				specialCounter--;
			}

			if (levelCounter == 0) { //down-counting counter
				levelCounter = nForUpgrade;
				if (i != 0) { //initial reset
					crntLevel++;
				}
			}
			else {
				levelCounter--;
			}

			out.push_back(chosenRoom);
			out[i].id = i;
			string nextLoad = "none";
			if (chosenRoomJ.count("nextLoad")) {
				nextLoad = chosenRoomJ["nextLoad"].get<string>();
				out[i].LoadDoors({ Door(0, -1, out[i], nextLoad) });
			}
			else if (i + 1 < nRooms) {
				out[i].LoadDoors({ Door(0, i + 1, out[i], nextLoad) });
			}

			if (crntLevel >= source["roomSets"].size()) {
				break;
			}
		}
	}

	return out;
}

Vec2 RoomParser::GetEnemySpawnLoc(string loc_in)
{
	if (loc_in == "cr1") {
		return {20, 20};
	}
	if (loc_in == "cr2") {
		return { 80, 20 };
	}
	if (loc_in == "cr3") {
		return { 80, 80 };
	}
	if (loc_in == "cr4") {
		return { 20, 80 };
	}

	if (loc_in == "sr1") {
		return { 20, 40 };
	}
	if (loc_in == "sr2") {
		return { 80, 40 };
	}
	if (loc_in == "sr3") {
		return { 80, 60 };
	}
	if (loc_in == "sr4") {
		return { 20, 60 };
	}

	if (loc_in == "l") {
		return { 20, 50 };
	}
	if (loc_in == "r") {
		return { 80, 50 };
	}
	if (loc_in == "c") {
		return { 50, 50 };
	}
	if (loc_in == "t" || loc_in == "top") {
		return { 50, 10 };
	}

	if (loc_in == "tr1") {
		return { 35, 45 };
	}
	if (loc_in == "tr2") {
		return { 65, 45 };
	}
	if (loc_in == "tr3") {
		return { 50, 60 };
	}

	if (loc_in == "ttr1") {
		return { 35, 10 };
	}
	if (loc_in == "ttr2") {
		return { 65, 10 };
	}
	if (loc_in == "ttr3") {
		return { 50, 25 };
	}
}
