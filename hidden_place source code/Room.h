#pragma once
#include <vector>
#include "Entity.h"
#include "Enemy.h"
#include "Graphics.h"
#include "Door.h"
#include "RoomID.h"
#include "Surface.h"
#include "json.hpp"
#include "Shrine.h"
#include "Capsule.h"
#include "Chest.h"
#include "Sign.h"
#include "Rect.h"
#include <queue>

using namespace std;

using json = nlohmann::json;

class Room {
public:
	static map<string, bool> roomFlags;
public:
	enum RoomType {
		bigH,
		bigV,
		smallH,
		smallV,
		medium
	};
	Room();
	std::map <int, Enemy*> enemies;
	std::map <int, Enemy*> enemiesNative;
	std::vector <Door> doors;
	std::vector <RoomID> ids;
	std::vector <Shrine> shrines;
	std::vector <Capsule> walls;
	std::vector <Chest> chests;
	std::vector <Sign> signs;
	Vec2 pos;
	RoomType roomType;
	string surf;
	string biom;
	string stepSound;
	int rotation = 0;
	string floorText = "";
	bool isLocked = false;

	Rect drawRect;
	std::vector<std::string> text; //for commands
	float lightLevel = 1;
	void LoadRoom(RoomType roomType_in, string gfxType_in, json roomJ, int level = 1);
	void LoadDoors(vector <Door> doors_in);
	void Draw(class Font font, 
		Graphics &gfx, class Player &hero);
	void DrawAt(Room &ogRoom, int doorType_in, Vec2 pos_in, Graphics &gfx,
		class Font font, Player *hero_in = nullptr);
	void DrawAtMap(int recLimit, int crntRec, Vec2 vec, Room &ogRoom, int doorType_in, Vec2 pos_in,
		vector<class Room> rooms_in, Graphics &gfx, class Font font,
		Player *hero_in = nullptr);
	void LoadEnemies(nlohmann::json j);
	void LoadWalls(nlohmann::json j);
	void LoadSigns(nlohmann::json j, nlohmann::json jTexts);
	void LoadChests(nlohmann::json j, int level = 1);
	void AddChest(Vec2 vec, std::vector<Item> items_in, std::string chestSurf);
	Sign& AddSign(Vec2 vec_in, std::vector<std::string> text_in, std::string surf_in);
	void Respawn(DoorType enterFrom = DoorType::top);
	void AddShrine();
	void OnEnter(std::queue<string>&textCommands_in, double timePassed, DoorType enterFrom = DoorType::top);
	void ProcessCommand(std::string command_in, double timePassed, std::queue<string>&textCommands_in);
	bool IsCleared();
	bool DoSignUpdate();
	void UnlockAllSigns();
	void EnableDamageForAllEnemies();
	void GiveCommandRef(queue<string>* textCommands_in);
	bool IsLocked();
	int width;
	int height;
	int nEnemies;
	int nIds;
	int id;
	bool isMirrored = false;
	bool noDamage = false;
	bool forceSignUpdate = false;
};