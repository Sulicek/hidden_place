/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Entity.h"
#include "Player.h"
#include <vector>
#include "Enemy.h"
#include "Door.h"
#include "Room.h"
#include "ProgressBar.h"
#include "RoomParser.h"
#include "Inventory.h"
#include "Sound.h"
#include "Font.h"
#include <map>
#include <deque>
#include <queue>
#include "Interface.h"
#include "SoundManager.h"
#include "Light.h"
#include "Saver.h"

enum Status {
	st_menu, st_map, st_inventory, st_armory, st_game
};

class Game
{
public:
	Game(class MainWindow& wnd);
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	void ProcessCommand(std::string command_in);
	void Go();
	class TranScreen {
	private:
		std::string tooltip = "";
		std::vector<std::string> text;
		std::string surf;
		float fadeOut = 100;
		float fadeOutCrnt = 0;
		int page = 0;
		float prg = 0;
		bool isPaused = false;
		bool isPlaying = false;
	public:
		bool isJustDialogLol = false;
		TranScreen(json j_in,
			std::vector<std::string> text_in);
		TranScreen() = default;
		void Start();
		void Update(float speed, Game &game);
	};
public:
	void ComposeFrame();
	void UpdateModel();
	void LoadModel(std::string filename_in, int roomSet = 0);
	void ShowTranScreen(std::string screenName);
	bool IsLoadScreen();
	void BlackOut(float speed = 1);
	void Darken(float speed = 1);
	void ShowBigText(std::string text_in);
	void OnHeroDeath();
	void SpawnHero(int rn_in, int direction = 4);
	float GetLightLevel();
	std::vector<Room>& GetRooms();
public:
	std::thread drawThread;
	bool isProduction = false;
	bool cheatEnable = false;
	int minFrames = 55;
	//UNUSED
	int maxFrames = 50;

	std::chrono::time_point<std::chrono::steady_clock> fpsLast = std::chrono::steady_clock::now();
	std::chrono::duration<double> fpsDiff;

	std::chrono::time_point<std::chrono::steady_clock> lastFrameTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> frameTimeDiff;

	bool draw = true;
	float desFPS = 60;
	bool isTimed = false;
	std::chrono::duration<double> timePassed;
	std::chrono::duration<double>  newUpdate = std::chrono::milliseconds(16);

	//drawing stuff
	class Font font;
	std::vector<std::string> dungeons;
	map<string, TranScreen> tranScreens;
	queue<string> textCommands;
	//SoundManager sdm;
	MainWindow& wnd;
	Graphics gfx;
	Player hero;
	Interface intf;
	Vec2 mouseVec;
	Saver saver;
	SpriteLoader sl;
	std::vector<Light> testLights;
	std::string backgroundSurf = "introScreen";
	std::string deathSurf = "deathScreen";
	std::map<int,std::vector<Room>> rooms;
	int roomSetSelected = 0;
	bool isLoaded = true;
	int rn = 0;
	string crntLevelName = "";
	int lastSpawnPos = 4;
	int savePoint = 0;
	int roomTemp = 0;
	int crntToll = 0;
	int deathCount = 0;

	int armoryStatSelected = 0;
	int statForUpgrade = 0;

	float asdf;

	int fwdIn = 0;
	int fwdInMax = 200;

	bool isStarted = false;
	bool isLoadScreen = false;
	bool isLoadScreenJustDialogTextLol = false;
	bool isSpriteLoaded = false;
	bool heroActionStoreForDialogScreen = false;

	std::string bigText = "";
	Camera bigTextCam;
	Camera invCam;
	Camera sideArmoryCam;
	Camera bottomArmoryCam;
	bool isBigText = false;
	int bigTextTimer = 0;

	Status state = Status::st_game;
	int mapDrawRecLimit = 5;

	TranScreen * tScreen = nullptr;
	
	float lightLevel = 1;
	float lightLevelTarget = lightLevel;
	float lightSpeed = 0.05;
	float lightSpeedExtra = 1;

	//for parsing json
	std::vector <std::vector <Door>> doors;
	std::map <string, Room::RoomType> typeMap;
	std::map <string, enum DoorType> doorTypeMap;
	int nRooms;
	json j;
	json jTexts;
	json jPathing;
	map<string, string> spriteMap = {
	//ground
		{ "outskirts medium","Resources\\Sprites\\Ground\\Ground_empty_base_02.bmp"},
		{ "deep medium","Resources\\Sprites\\Ground\\Ground_empty_base_02.bmp"},
		{ "altered medium","Resources\\Sprites\\Ground\\Ground_empty_altered_03.bmp"},
		{ "path medium","Resources\\Sprites\\Ground\\Ground_empty_path_02.bmp"},
		{ "forest medium","Resources\\Sprites\\Ground\\test.bmp"},
		{ "kingdom medium","Resources\\Sprites\\Ground\\Ground_empty_kingdom_01.bmp"},
		{ "hut_inside","Resources\\Sprites\\Ground\\Ground_hut_inside_01.bmp"},

		{ "caves medium","Resources\\Sprites\\Ground\\DungeonPath_texture_medium_01.bmp"},
		{ "tutorial move", "Resources\\Sprites\\Ground\\Tutorial_texture_move_01.bmp" },
		{ "tutorial attack", "Resources\\Sprites\\Ground\\Tutorial_texture_attack_01.bmp" },

	//enemies
		{ "leaves", "Resources\\Sprites\\Enemies\\Leaves_spriteSheet_02.bmp"},
		{ "vomit", "Resources\\Sprites\\Enemies\\Vomit_spriteSheet_01.bmp"},
		{ "nade", "Resources\\Sprites\\Enemies\\Nade_spriteSheet_01.bmp"},
		{ "leavesMask", "Resources\\Sprites\\Enemies\\Leaves_mask_01.bmp"},
		{ "souls", "Resources\\Sprites\\Enemies\\Souls_spriteSheet_01.bmp"},
		{ "hammer", "Resources\\Sprites\\Enemies\\Hammer_spriteSheet_02.bmp"},
		{ "crystal", "Resources\\Sprites\\Enemies\\Crystal_spriteSheet_02.bmp"},
		{ "goblet", "Resources\\Sprites\\Enemies\\Goblet_spriteSheet_02.bmp"},
		{ "dummy", "Resources\\Sprites\\Enemies\\Dummy_spriteSheet_01.bmp"},
		{ "lock", "Resources\\Sprites\\Enemies\\Lock_spriteSheet_01.bmp"},
		{ "soulsMask", "Resources\\Sprites\\Enemies\\Souls_mask_01.bmp"},
		{ "wolf", "Resources\\Sprites\\Enemies\\WerewolfBoss_spriteSheet_03.bmp"},
		{ "tree", "Resources\\Sprites\\Enemies\\TreeBoss_spriteSheet_02.bmp" },
		{ "fountain", "Resources\\Sprites\\Enemies\\FountainBoss_spriteSheet_01.bmp" },
		{ "hammers", "Resources\\Sprites\\Enemies\\ForgeBoss_spriteSheet_02.bmp" },
		{ "heart", "Resources\\Sprites\\Enemies\\Heart_spriteSheet_01.bmp" },
		{ "heart rooted", "Resources\\Sprites\\Enemies\\HeartRooted_spriteSheet_01.bmp" },
		{ "reaper", "Resources\\Sprites\\Enemies\\ReaperBoss_spriteSheet_01.bmp"},
	//other
		{ "hero", "Resources\\Sprites\\Other\\UndeadAdventurer_spriteSheet_04.bmp"},

		{ "strike_left", "Resources\\Sprites\\Other\\Strike_left.bmp"},
		{ "strike_right", "Resources\\Sprites\\Other\\Strike_right.bmp"},
		{ "strike_top", "Resources\\Sprites\\Other\\Strike_top.bmp"},
		{ "strike_bottom", "Resources\\Sprites\\Other\\Strike_bottom.bmp"},
		{ "explosion", "Resources\\Sprites\\Other\\Explosion_01.bmp"},
		{ "smol", "Resources\\Sprites\\Other\\ExplosionSmol_01.bmp"},
		{ "disappear", "Resources\\Sprites\\Other\\Disappear_02.bmp"},
		{ "root", "Resources\\Sprites\\Other\\Root_01.bmp"},

		{ "heroMask", "Resources\\Sprites\\Other\\UndeadAdventurer_mask_04.bmp"},
		{"projectileMask", "Resources\\Sprites\\Other\\Shadowball_mask_01.bmp"},
		{"projectile", "Resources\\Sprites\\Other\\Shadowball_Spell_01.bmp"},

		{ "introScreen","Resources\\Sprites\\Thematic\\Intro_background_01.bmp"},
		{ "deathScreen","Resources\\Sprites\\Thematic\\Skull_background_02.bmp"},
		{ "bcg","Resources\\Sprites\\Thematic\\background_texture_02.bmp"},
		{ "toll","Resources\\Sprites\\Thematic\\toll.bmp"},

		{ "armor", "Resources\\Sprites\\Items\\Armor_Items_01.bmp"},
		{ "Resources\\Sprites\\Wheapons_Items_02.bmp", "Resources\\Sprites\\Items\\Wheapons_Items_02.bmp"},
		{ "Resources\\Sprites\\Wheapons_Items_02.bmp", "Resources\\Sprites\\Items\\Wheapons_Items_02.bmp"},
	//objects
		{ "doorMaskLeft","Resources\\Sprites\\Objects\\DoorLeft_mask_01.bmp"},
		{ "doorMaskRight","Resources\\Sprites\\Objects\\DoorRight_mask_01.bmp"},
		{ "doorMaskTop","Resources\\Sprites\\Objects\\DoorTop_mask_01.bmp"},
		{ "doorMaskBottom","Resources\\Sprites\\Objects\\DoorBottom_mask_01.bmp"},
	//objects
		{ "rocks","Resources\\Sprites\\Objects\\Rocks_object_01.bmp"},
		{ "friend","Resources\\Sprites\\Objects\\Friend_spriteSheet_01.bmp"},
		{ "witch","Resources\\Sprites\\Objects\\witch_01.bmp"},
		{ "witchAltered","Resources\\Sprites\\Objects\\witchAltered_01.bmp"},
		{ "hut","Resources\\Sprites\\Objects\\hut_02.bmp"},
		{ "entrance","Resources\\Sprites\\Objects\\Entrance.bmp"},
		{ "hutAltered","Resources\\Sprites\\Objects\\hutAltered_01.bmp"},
		{ "chestHead","Resources\\Sprites\\Objects\\SoulHead_spriteSheet_01.bmp"},
		{ "chestBase","Resources\\Sprites\\Objects\\SoulBase_spriteSheet_01.bmp"},
		{ "chestMask","Resources\\Sprites\\Objects\\SoulHead_mask_01.bmp"},
		{	"Resources\\Sprites\\Fixedsys8x14.bmp","Resources\\Sprites\\Fixedsys8x14.bmp"}
		//ForestPath_texture_medium_01.bmp"},
	};
};