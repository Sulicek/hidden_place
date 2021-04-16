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
#include "MainWindow.h"
#include "Game.h"
#include "Colors.h"
#include "Entity.h"
#include "Enemy.h"
#include "Door.h"
#include <random>
#include "Spell.h"
#include "ProgressBar.h"
#include <assert.h>
#include "Font.h"
#include "Pet.h"
#include <time.h>
#include <filesystem>
#include "Parameters.h"
#include <chrono>
#include "RoomParser.h"

json OpenJson(std::string filename_in) {

	fstream input;
	input.open(filename_in);
	json output = json::parse(input);
	input.close();
	return output;
}

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	hero(),
	font("Resources\\Sprites\\Fixedsys8x14.bmp")
	//,fontBig("Resources\\Sprites\\Monospace.bmp")
{
	gfx.sl = &sl;
	sl.Load(spriteMap);
	//starting the theme
	srand(time(0));
	if (rand() % 2 == 0) {
		//forestTheme.Play();
	}
	else {
		//magicTheme.Play();
	}
	//forestTheme.Play();
	//magicTheme.Play();

	//parsing dialog files
	fstream dialogInput;
	dialogInput.open("Resources\\texts.json");
	jTexts = json::parse(dialogInput);

	//parsing transition files
	fstream tranInput;
	tranInput.open("Resources\\loadScreens.json");
	json jTran = json::parse(tranInput);
	for (auto &item : jTran) {
		string textSource = item["text"];
		tranScreens[item["name"]] = TranScreen(item, jTexts[textSource]);
	}
	string creditSmile = "..............Made by Jan Sulicek.....Press \'E\'";
	json creditJson = { {"image","none"} };
	tranScreens["introScreen"] = TranScreen(creditJson, { ".............................Press \'E\'",creditSmile,
		"..........hidden_place.exe.........."});

	//parsing the sounds files
	fstream soundInput;
	soundInput.open("Resources\\sounds.json");
	json jSound = json::parse(soundInput);
	SoundManager::LoadSounds(jSound["sounds"]);

	//parsing enemy files
	Enemy::enemyJ = OpenJson("Resources\\enemies.json");

	//parsing the pathing file
	fstream pathingInput;
	pathingInput.open("Resources\\pathing.json");
	jPathing = json::parse(pathingInput);
	//creating a button for each file with .json
	std::string path = "Resources\\Levels\\";
	for (const auto & entry : std::experimental::filesystem::directory_iterator(path)) {
		if (entry.path().string().find(".json") != string::npos) {
			dungeons.push_back(entry.path().string());
		}
	}

	//creating interface
	//intf += Interface::Label("asdfghjkl", { 50, 5 }, 150, 50, Colors::BackgroundGreen, 5);
	for (int i = 0; i < dungeons.size(); i++) {
		intf += Interface::Button(dungeons[i], { 50, float((i + 1) * 10) }, 300, 50, Colors::BackgroundGreen, 5);
	}

	intf += Interface::Button("Crypt", { 10, 30 }, 150, 50, Colors::BackgroundGreen, 5);
	intf += Interface::Button("Forest", { 10, 20 }, 150, 50, Colors::BackgroundGreen, 5);
	intf += Interface::Button("Mute", { 10, 10 }, 150, 50, Colors::BackgroundGreen, 5);
	intf += Interface::LightButtonGroup(Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2), 100);

	intf.lbg[0].UpdateButtonStatus(hero);

	isProduction = true;

	if (isProduction) {
		//start the game straingt away
		isStarted = true;
		//load limbo into a different room set
		LoadModel("Resources\\Levels\\limbo.json", 1);
		bool useSave = true;
		if (useSave) {
			json savedData = saver.GetSavedData();
			cheatEnable = bool(savedData["debug"].get<int>());
			hero.deathCount = int(savedData["deathCount"].get<int>());
			LoadModel(savedData["level"]);
			SpawnHero(savedData["rn"], savedData["direction"]);
			//load inventory from save file
			for (auto item : savedData["inv"]) {
				hero.inv.AddItem(MinorItem(ItemName(item)));
			}
			hero.statPointsToAllocate = savedData["pointsLeft"];
			for (auto stat : savedData["stats"]) {
				hero.stats[ItemParameters(stat[0])] = stat[1];
			}
		}
		else {
			LoadModel("Resources\\Levels\\intro.json");
			SpawnHero(0);
		}
		ShowTranScreen("introScreen");
		//BlackOut();
	}
	else {
		BlackOut();
	}

	for (int i = 0; i < 16; i++)
	{
		//	testLights.push_back(Light(-1, -1,true, Color::rgbToColor(Color::hsv2rgb({ double( rand() % 360), 1.0, 1.0 }))));
	}

	//loading anims
	Animation::AddStaticAnim("strike_left", "strike_left", 8, 150, 150);
	Animation::AddStaticAnim("strike_right", "strike_right", 8, 150, 150);
	Animation::AddStaticAnim("strike_top", "strike_top", 8, 150, 150);
	Animation::AddStaticAnim("strike_bottom", "strike_bottom", 8, 150, 150);
	Animation::AddStaticAnim("explosion", "explosion", 8, 200, 200);
	Animation::AddStaticAnim("smol", "smol", 8, 100, 100);
	Animation::AddStaticAnim("disappear", "disappear", 8, 60, 60);
	Animation::SetFps("strike_left", 3);
	Animation::SetFps("strike_right", 3);
	Animation::SetFps("strike_top", 3);
	Animation::SetFps("strike_bottom", 3);
	Animation::SetFps("explosion", 5);
	Animation::SetFps("smol", 5);
	Animation::SetFps("disappear", 1);
}


void Game::LoadModel(string filename_in, int roomSet)
{
	crntLevelName = filename_in;
	roomSetSelected = roomSet;
	if (GetRooms().size() > 0) {
		if (GetRooms()[0].biom == "tutorial") {
			SoundManager::StopAll();
		}
	}
	BlackOut();
	srand(time(0));
	//we reset everything
	rn = 0;
	//hero = Player();
	GetRooms().erase(GetRooms().begin(), GetRooms().end());
	std::string filename = filename_in;
	//our input streem
	fstream input;
	//we open the file in it
	input.open(filename);
	//we parse the input stream into the json object
	j = json::parse(input);

	GetRooms() = RoomParser::LoadModel(j, jTexts);
	if (j["config"].count("toll")) {
		crntToll = j["config"]["toll"];
	}
	else {
		crntToll = 0;
	}
	SpawnHero(0);
	hero.isMoving = false;
	BlackOut();
}

float Game::GetLightLevel() {
	if (GetRooms().size() > rn) {
		return GetRooms()[rn].lightLevel;
	}
	return 1;
}

std::vector<Room> &Game::GetRooms()
{
	return rooms[roomSetSelected];
}

void Game::BlackOut(float speed)
{
	/*if (length == -1) {
		length = blackOut;
	}
	blackOutCrnt = length;*/
	lightLevelTarget = GetLightLevel();
	lightLevel = 0;
	lightSpeedExtra = speed;
}

void Game::Darken(float speed)
{
	lightLevelTarget = 0;
	lightLevel = GetLightLevel();
	lightSpeedExtra = speed;
}

void Game::ShowBigText(std::string text_in)
{
	bigText = text_in;
	bigTextTimer = 100;
}

void Game::OnHeroDeath()
{
	//hero dead
	gfx.ClearScreen(Colors::Black);

	gfx.DrawSpritePortion(Vec2(Graphics::ScreenWidth / 2 - gfx.sl->operator[](deathSurf)->GetWidth() / 2,
		Graphics::ScreenHeight / 2 - gfx.sl->operator[](deathSurf)->GetHeight() / 2), 
		gfx.sl->operator[](deathSurf)->GetWidth(), 
		gfx.sl->operator[](deathSurf)->GetHeight(), Colors::Magenta, SpritePos({ 0, 0 }), deathSurf, 1, {});
	
	//if (deathFadeOut == 50) {
	//	//window is kill (:
	//	wnd.Kill();
	//}
	//we catch the blackout right before it manages to end the game
	lightLevelTarget = 0;



	if (lightLevel <= 0.01f) {
		hero.deathCount++;
		//we move the player into the previous room,
		//please can you not die in the first room? ((((:
		/*if (savePoint < GetRooms().size()) {
			rn = savePoint;
		}
		else {
			rn = 0;
		}*/
		//going to limbo
		roomSetSelected = 1;
		int savePointSave = savePoint;
		SpawnHero(0);
		savePoint = savePointSave;
		hero.isActive = true;
		hero.health = hero.healthMax;
		lightLevelTarget = GetLightLevel();
		BlackOut();
		textCommands.push("return");
	}
}

void Game::SpawnHero(int rn_in, int direction)
{
	if (direction != 4) {
		lastSpawnPos = direction;
	}
	//spawning the hero properly in the room 
	int formerRoom = rn;
	int doorSize = 30;
	rn = rn_in;
	int padding = 5;
	switch (direction) {
	case 0:
		hero.vec.x = GetRooms()[rn].width / 2 - hero.width / 2;
		hero.vec.y = GetRooms()[rn].height - hero.height - doorSize;
		break;
	case 1:
		hero.vec.x = GetRooms()[rn].width / 2 - hero.width / 2;
		hero.vec.y = doorSize;
		break;
	case 2:
		hero.vec.x = GetRooms()[rn].width - hero.width - doorSize;
		hero.vec.y = GetRooms()[rn].height / 2 - hero.height / 2;
		break;
	case 3:
		hero.vec.x = doorSize;
		hero.vec.y = GetRooms()[rn].height / 2 - hero.height / 2;
		break;
	case 4:
		hero.vec.x = GetRooms()[rn].width / 2 - hero.width / 2;
		hero.vec.y = GetRooms()[rn].height / 2 - hero.height / 2 + 100;
		break;
	}
	//set the camera 
	hero.cam.pos = Vec2(GetRooms()[rn].width / 2, GetRooms()[rn].height / 2);
	hero.Respawn(crntToll);
	//set the lightLevel
	lightLevelTarget = GetLightLevel();
	textCommands = std::queue<std::string>();
	GetRooms()[rn].OnEnter(textCommands, timePassed.count(), DoorType(direction));
	//remove the dash cuz it be glitching
	if (hero.e.Has(EffectType::dash))
		hero.e.Remove(EffectType::dash);
}


void Game::ProcessCommand(std::string command_in)
{
	if (command_in == "test") {
		
	}
	else if (command_in == "incrementrn") {
		textCommands.push("asdfasdf"); //spawn hero removes one command from the top basically
		SpawnHero(rn + 1, 0);
	}
	else if (command_in == "nodamage") {
		GetRooms()[rn].noDamage = true;
	}
	else if (command_in == "yesdamage") {
		GetRooms()[rn].noDamage = false;
	}
	else if (command_in == "yesdamageenemy") {
		GetRooms()[rn].EnableDamageForAllEnemies();
	}
	else if (command_in == "forcesignupdate") {
		GetRooms()[rn].forceSignUpdate = true;
	}
	else if (command_in == "unlocksigns") {
		GetRooms()[rn].UnlockAllSigns();
	}
	else if (command_in == "givecommandreference") {
		GetRooms()[rn].GiveCommandRef(&textCommands);
	}
	else if (command_in == "start counter") {
		isTimed = true;
	}
	else if (command_in == "schedulefwd") {
		fwdIn = fwdInMax;
	}
	else if (command_in == "fwd") {
		heroActionStoreForDialogScreen = true;
	}
	else if (command_in == "loff") {
		for (auto &sign : GetRooms()[rn].signs) {
			sign.light.Off();
		}
	}
	else if (command_in == "spawn dummy") {
		GetRooms()[rn].enemies[GetRooms()[rn].enemies.size()] =
			new Enemy(Vec2(GetRooms()[rn].width / 2, GetRooms()[rn].height / 2),
				json(json::parse(string("{\"id\":11}"))), "forest");
		for (auto sign : GetRooms()[rn].signs) {
			Animation::Show("explosion", sign.getCenterVec());
		}
	}
	else if (command_in == "lock") {
		GetRooms()[rn].isLocked = true;
	}
	else if (command_in == "unlock") {
		GetRooms()[rn].isLocked = false;
	}
	else if (command_in == "stop counter") {
		isTimed = false;
	}
	else if (command_in == "blackout") {
		BlackOut();
	}
	else if (command_in == "darken") {
		lightLevelTarget = 0;
	}
	else if (command_in == "lights off") {
		lightLevelTarget = 0.01;
	}
	else if (command_in == "lights on") {
		lightLevelTarget = 1;
	}
	else if (command_in == "save") {
		savePoint = rn;
		saver.Save(hero, rn, crntLevelName, lastSpawnPos, cheatEnable);
		//ShowBigText("LOCATION SAVED");
	}
	else if (command_in == "return") {
		roomSetSelected = 0;
		GetRooms()[rn].Respawn();
		saver.Save(hero, savePoint, crntLevelName, lastSpawnPos, cheatEnable);
		SpawnHero(savePoint, lastSpawnPos);
	}
	else if (command_in.find("show") != string::npos) {
		ShowBigText(command_in.substr(command_in.find("show") + 5));
	}
	else if (command_in.find("playse") != string::npos) {
		SoundManager::Play(command_in.substr(command_in.find("playse") + 7));
	}
	else if (command_in.find("play") != string::npos) {
		SoundManager::StopAll();
		SoundManager::Play(command_in.substr(command_in.find("play") + 5));
	}
	else if (command_in.find("sts") != string::npos) {
		ShowTranScreen(command_in.substr(command_in.find("sts") + 4));
	}
	else if (command_in.find("heal") != string::npos) {
		hero.health += stoi(command_in.substr(command_in.find("heal") + 5));
		//ShowBigText(command_in.substr(command_in.find("heal") + 5));
	}
	else if (command_in.find("item") != string::npos) {
		string item = command_in.substr(command_in.find("item") + 5);
		hero.inv.items = {};
		if (item == "boomerang") {
			hero.inv.AddItem(MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_return },
		{ SpellParameters::addAmmoOnHeroHit, 1 },
		{SpellParameters::notDeleteOnHit, 3}
				}, {
					{SpellParameters::castSpeed, 0.8}
				}));
		}
		if (item == "grenade") {
			/*hero.inv.AddItem(MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_explode },
		{SpellParameters::notDeleteOnHit, 3}
				}, {
					{SpellParameters::castSpeed, 0.7},
					{SpellParameters::damage, 0}
				}));*/
			hero.inv.AddItem(MinorItem(ItemName::item_grenade));
		}
		if (item == "explosiveDash") {
			/*hero.inv.AddItem(MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_explode },
		{SpellParameters::notDeleteOnHit, 3}
				}, {
					{SpellParameters::castSpeed, 0.7},
					{SpellParameters::damage, 0}
				}));*/
			//
			hero.inv.AddItem(MinorItem(ItemName::item_grenade));
			hero.inv.AddItem(MinorItem(ItemName::item_explosiveDash));
		}
		if (item == "claw") {
			hero.inv.AddItem(MinorItem({
		{ SpellParameters::storedSpellExpire, SpellName::stored_claw },
		{ SpellParameters::rangeCap, 0.1}
				}, { }));
		}
		if (item == "scatter") {
			/*hero.inv.AddItem(MinorItem({
				{ SpellParameters::storedSpellExpire, SpellName::stored_scatter },
				{ SpellParameters::rangeCap, 100 },
				}, {
					{SpellParameters::castSpeed, 0.7},
					{SpellParameters::damage, 0} }));*/
			hero.inv.AddItem(MinorItem(ItemName::item_scatter));
		}
		if (item == "dash") {
			if (!hero.inv.GetData(0, SpellParameters::canPlayerDash)) {
				hero.inv.AddItem(MinorItem(ItemName::item_dashEnable));
			}
		}
	}
	else if (command_in.substr(0, 3) == "set") {
		//command structure
		//command_in: set someCondition someValue
		string condition = command_in.substr(4, command_in.substr(4).find(" "));
		//condition: someCondition
		string value = command_in.substr(command_in.substr(4).find(" ") + 1 + 4);
		//value: someValue
		if (value == "true" || value == "1") {
			Room::roomFlags[condition] = true;
		}
		else if (value == "false" || value == "0") {
			Room::roomFlags[condition] = false;
		}
	}
}

void Game::ShowTranScreen(std::string screenName)
{
	isLoadScreen = true;
	if (tranScreens[screenName].isJustDialogLol) {
		isLoadScreenJustDialogTextLol = true;
	}
	else {
		isLoadScreenJustDialogTextLol = false;
	}
	tScreen = &tranScreens[screenName];
	tScreen->Start();
}

bool Game::IsLoadScreen()
{
	return isLoadScreen && !isLoadScreenJustDialogTextLol;
}

void Game::TranScreen::Start()
{
	fadeOutCrnt = fadeOut;
	page = 0;
	prg = 0;
	isPaused = false;
	isPlaying = true;
}

void Game::TranScreen::Update(float speed, Game &game)
{
	if (!game.isLoadScreenJustDialogTextLol) {
		fadeOutCrnt -= speed;
		if (fadeOutCrnt <= 0) {
			fadeOutCrnt = fadeOut;
		}
		game.gfx.ClearScreen(Colors::Black);
		//Surface *ts = game.spriteMap[surf];
		Surface *ts;
		if (surf != "none") {
			ts = game.gfx.sl->operator[](surf);
		}
		float visibility;
		if (fadeOutCrnt / fadeOut <= 0.5) {
			//visibility = (fadeOutCrnt / fadeOut)*2;
		}
		else {
			visibility = (-(fadeOutCrnt / fadeOut) + 1) * 2;
		}
		float fadeOutTest = abs(fadeOutCrnt - (fadeOut / 2.0f));
		//test
		visibility = (fadeOutTest + (fadeOut / 8.0f)) / fadeOut;
		//game.font.Draw(to_string(visibility), Vec2(100, 100), Colors::White, Colors::White, game.gfx);
		if (surf != "none") {
			game.gfx.DrawSpritePortion(
				Vec2(Graphics::ScreenWidth / 2 - ts->GetWidth() / 2,
					Graphics::ScreenHeight / 2 - ts->GetHeight() / 2),
				ts->GetWidth(), ts->GetHeight(), Colors::Magenta, { 0, 0 }, surf, visibility);
		}
	}
	//text processing

	bool shouldScreenProgress;
	if (game.isLoadScreenJustDialogTextLol) {
		shouldScreenProgress = game.heroActionStoreForDialogScreen;
	}
	else {
		shouldScreenProgress = game.hero.IsAction();
	}
	if (shouldScreenProgress) {
		if (isPlaying) {
			isPaused = false;
			page++;
			prg = 0;
		}
		else {
			isPlaying = true;
		}
	}

	if (page == text.size()) {
		page = 0;
		isPlaying = false;
		//exit the transition screen
		if (!game.isLoadScreenJustDialogTextLol) {
			game.BlackOut();
		}
		game.isLoadScreen = false;
		game.isLoadScreenJustDialogTextLol = false;
	}
	if (text[page].substr(0, 1) != "!") {
		//setting the text
		if (isPlaying) {
			tooltip = text[page].substr(0, prg + 1);
			int spacesMissing = text[page].size() - tooltip.size();
			for (int i = 0; i < spacesMissing; i++) {
				tooltip += " ";
			}
			if (!isPaused) {
				float prgSpeed = 0.5;
				prg += prgSpeed;
				if (prg == text[page].size()) {
					isPaused = true;
				}
			}
		}
		//game.font.Draw(tooltip, Vec2(100, Graphics::ScreenHeight - 50), Colors::White, Colors::White, game.gfx);

		game.font.Draw(tooltip,
			Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight - 100) -
			Vec2(tooltip.size() * game.font.getGlyphWidth() * 2 / 2,
				game.font.getGlyphHeight() * 2),
			Color(4, 145, 134), Colors::Magenta, 2, game.gfx);
		if (!game.isLoadScreenJustDialogTextLol) {
			game.hero.a.Update();
			game.hero.a.DrawRaw({ 30, Graphics::ScreenHeight - 110 }, Rect({ 0, 0 }, 90, 90), AnimationType::RunRight, game.gfx);
		}
	}
	else {
		game.textCommands.push(text[page].substr(1));
		//found a command and we skip all of this stuff :>
		isPaused = false;
		page++;
		prg = 0;
	}
}

Game::TranScreen::TranScreen(json j_in,
	std::vector<std::string> text_in) {
	surf = j_in["image"].get<std::string>();
	text = text_in;
	if (j_in.count("dialog")) {
		isJustDialogLol = true;
	}
}

void DoDrawing(Game* game) {
	try {
		//while (true) {
		game->gfx.BeginFrame();
		game->ComposeFrame();
		game->gfx.EndFrame();
		//}
	}
	catch (int e) {

	}
}

void Game::Go() {
	std::chrono::time_point<std::chrono::steady_clock> newFrameTime = std::chrono::steady_clock::now();
	frameTimeDiff = newFrameTime - lastFrameTime;
	lastFrameTime = std::chrono::steady_clock::now();
	newUpdate -= frameTimeDiff;
	if (newUpdate.count() <= 0) {
		UpdateModel();
		newUpdate += std::chrono::milliseconds(16);
		//draw = rand()%2;
	}
	if (draw) {
		//draw = false;
		//drawThread = std::thread(DoDrawing, this);
		DoDrawing(this);
		/*gfx.BeginFrame();
		ComposeFrame();
		gfx.EndFrame();*/
	}
}


void Game::UpdateModel() {
	Animation::UpdateStatic();
	//isProduction = false;
	isSpriteLoaded = gfx.sl->LoadAll(1);
	//timing
	if (isTimed) {
		timePassed += frameTimeDiff;
	}
	if (fwdIn > 0) {
		fwdIn--;
		if (fwdIn <= 0) {
			textCommands.push("fwd");
		}
	}
	if (!IsLoadScreen() && isSpriteLoaded) {
		if (isStarted) {
			//keyboard events outside of the main loop
			while (!wnd.kbd.KeyIsEmpty()) {
				//get an event from the queue
				const Keyboard::Event e = wnd.kbd.ReadKey();
				//check if it is a release event
				if (e.GetCode() == 'E') {
					if (e.IsPress()) {
						hero.isAction = true;
					}
				}
				if (e.GetCode() == 'L')
				{
					if (e.IsPress()) {
						//test debug
						if (cheatEnable) {
							SpawnHero(rn + 1);
						}
					}
					/*if (e.IsPress()) {
						if (hero.heroLight.lightLevel > 0.1) {
							hero.heroLight.Off();
						}
						else {
							hero.heroLight.On();
						}
					}*/
				}
				if (e.GetCode() == VK_ESCAPE)
				{
					if (cheatEnable) {
						if (e.IsPress()) {
							if (state == Status::st_game) {
								state = Status::st_menu;
							}
							else {
								state = Status::st_game;
							}
						}
					}
				}
				if (e.GetCode() == 'M')
				{
					if (e.IsPress()) {
						if (state == Status::st_map) {
							state = Status::st_game;
						}
						else {
							state = Status::st_map;
						}
						mapDrawRecLimit = 5;
					}
				}
				
				if (state == Status::st_map) {
					if (e.GetCode() == VK_UP)
					{
						if (e.IsPress()) {
							mapDrawRecLimit++;

						}
					}
					if (e.GetCode() == VK_DOWN)
					{
						if (e.IsPress()) {
							mapDrawRecLimit--;
						}
					}
				}
				if (state == Status::st_armory) {
					if (e.GetCode() == VK_LEFT)
					{
						if (e.IsPress()) {
							armoryStatSelected++;
						}
					}
					if (e.GetCode() == VK_RIGHT)
					{
						if (e.IsPress()) {
							armoryStatSelected--;
						}
					}
					if (e.GetCode() == VK_UP) {
						if (e.IsPress()) {
							int statPointsRequired = hero.GetStatCost(hero.stats[ItemParameters(statForUpgrade)]);
							if (hero.statPointsToAllocate >= statPointsRequired) {
								if (hero.stats[ItemParameters(statForUpgrade)] + 1 <= hero.statMax) {
									hero.statPointsToAllocate -= statPointsRequired;
									hero.stats[ItemParameters(statForUpgrade)] += 1;
								}
							}
						}
					}
					if (e.GetCode() == VK_DOWN) {
						if (e.IsPress()) {
							int costOfCurrentStatLevel = hero.GetStatCost(hero.stats[ItemParameters(statForUpgrade)]-1);
							if (hero.stats[ItemParameters(statForUpgrade)] - 1 >= 0) {
								hero.statPointsToAllocate+= costOfCurrentStatLevel;
								hero.stats[ItemParameters(statForUpgrade)] -= 1;
							}
						}
					}
				}
				if (e.GetCode() == 'C')
				{
					if (e.IsPress()) {
						if (state == Status::st_armory) {
							state = Status::st_game;
						}
						else {
							if (GetRooms()[rn].IsCleared()) {
								state = Status::st_armory;
								intf.lbg[0].UpdateButtonStatus(hero);
								armoryStatSelected = 0;
								statForUpgrade = 0;
							}
						}
					}
				}
				if (e.GetCode() == 'I') {
					if (e.IsPress()) {
						if (state != Status::st_inventory) {
							state = Status::st_inventory;
						}
						else {
							state = Status::st_game;
						}
					}
				}
				if (e.GetCode() == 'P') {
					if (e.IsPress()) {
						isProduction = !isProduction;
					}
				}
			}

			heroActionStoreForDialogScreen = false;
			while (textCommands.size() > 0) {
				ProcessCommand(textCommands.front());
				textCommands.pop();
			}
			if (state == Status::st_game) {
				//keyboard processing
				hero.MoveControls(wnd);
				//dash
				if (wnd.kbd.KeyIsPressed(VK_SPACE)) {
					hero.AttemptToDash();
				}
				if (wnd.kbd.KeyIsPressed(VK_CONTROL)) {
					hero.StartCasting(2);
				}
				//mouse events
				while (!wnd.mouse.IsEmpty()) {
					//get an event from the queue
					const Mouse::Event e = wnd.mouse.Read();
					//check if it is a release event
					/*if (e.GetType() == 0) {
					}*/
				}

				//only update them if all enemies are dead
				//if (!GetRooms()[rn].enemies.size()) {
				if (GetRooms()[rn].IsCleared()) {
					//updating doors
					if (!GetRooms()[rn].IsLocked()) {
						for (int i = 0; i < GetRooms()[rn].doors.size(); i++) {
							int doorUpdateResult = GetRooms()[rn].doors[i].Update(hero);
							if (doorUpdateResult >= 0) {
								SpawnHero(doorUpdateResult, GetRooms()[rn].doors[i].type);
							}
							else if (doorUpdateResult == -2) {
								LoadModel("Resources\\Levels\\" + GetRooms()[rn].doors[i].nextLoad + ".json");
							}
						}
					}
				}
				if (GetRooms()[rn].DoSignUpdate()) {
					//updating signs
					for (auto &sign : GetRooms()[rn].signs) {
						sign.Update(hero, textCommands);
					}
				}
				//updating enemies
				for (int i = 0; i < GetRooms()[rn].enemies.size(); i++) {
					GetRooms()[rn].enemies[i]->Update(hero, GetRooms()[rn].enemies, GetRooms()[rn]);
				}

				//updating hero
				hero.Update(GetRooms()[rn].enemies, GetRooms()[rn], wnd);
				//for hero
				//projectiles
				for (int i = 0; i < hero.projectiles.size(); i++) {
					if (!hero.projectiles[i].isMoving || !hero.projectiles[i].isActive) {
						hero.projectiles[i].OnDeath();
						hero.projectiles.erase(hero.projectiles.begin() + i);
						i--;
					}
				}
				//cones
				for (int i = 0; i < hero.cones.size(); i++) {
					if (!hero.cones[i].isActive) {
						hero.cones.erase(hero.cones.begin() + i);
					}
				}
				//for enemies
				for (int o = 0; o < GetRooms()[rn].enemies.size(); o++) {
					for (int i = 0; i < GetRooms()[rn].enemies[o]->projectiles.size(); i++) {
						if (!GetRooms()[rn].enemies[o]->projectiles[i].isActive || !GetRooms()[rn].enemies[o]->projectiles[i].isMoving) {
							GetRooms()[rn].enemies[o]->projectiles[i].OnDeath();
							GetRooms()[rn].enemies[o]->projectiles.erase(GetRooms()[rn].enemies[o]->projectiles.begin() + i);
							i--;
						}
					}
				}
				//deleting enemies
				for (int o = 0; o < GetRooms()[rn].enemies.size(); o++) {
					if (GetRooms()[rn].enemies[o]->isDead) {
						//	GetRooms()[rn].enemies.erase(o);
					}
				}
			}
			else if (state == Status::st_menu) {//menu is being shown
				intf.Update(wnd);
				while (intf) {
					auto evnt = intf.Get();
					for (auto &dung : dungeons) {
						if (evnt.parent == "  " + dung) {
							isStarted = true;
							LoadModel(dung);
						}
					}
					if (evnt.parent == "  Crypt") {
						SoundManager::StopAll();
						SoundManager::Play("crypt");
						break;
					}
					if (evnt.parent == "  Forest") {
						SoundManager::StopAll();
						SoundManager::Play("forest");
						break;
					}
					if (evnt.parent == "  Mute") {
						SoundManager::StopAll();
						break;
					}
				}
			}
		}
		else {
			intf.Update(wnd);
			while (intf) {
				auto evnt = intf.Get();
				for (auto &dung : dungeons) {
					if (evnt.parent == "  " + dung) {
						isStarted = true;
						LoadModel(dung);
					}
				}
				if (evnt.parent == "  Crypt") {
					SoundManager::StopAll();
					SoundManager::Play("crypt");
					break;
				}
				if (evnt.parent == "  Forest") {
					SoundManager::StopAll();
					SoundManager::Play("forest");
					break;
				}
				if (evnt.parent == "  Mute") {
					SoundManager::StopAll();
					break;
				}
			}
		}
	}
	else {
		//keyboard events for the load screen (kinda mess, sry)
		while (!wnd.kbd.KeyIsEmpty()) {
			//get an event from the queue
			const Keyboard::Event e = wnd.kbd.ReadKey();
			//check if it is a release event
			if (e.GetCode() == 'E') {
				if (e.IsPress()) {
					hero.isAction = true;
				}
			}
		}
	}
}



void Game::ComposeFrame() {
	std::chrono::time_point<std::chrono::steady_clock> fpsNew = std::chrono::steady_clock::now();
	fpsDiff = fpsNew - fpsLast;
	fpsLast = std::chrono::steady_clock::now();
	if (lightLevel < 0.99) {
		gfx.Illuminate(lightLevel);
	}
	bool drawShit = true;
	if (drawShit) {
		if (!IsLoadScreen()) {
			if (isStarted) {
				GetRooms()[rn].Draw(font, gfx, hero);
				if (hero.isActive) {
					hero.Draw(gfx, GetRooms()[rn].pos, font, wnd);
				}
				Animation::DrawStaticGeneral(GetRooms()[rn].pos, gfx);

				if (state == Status::st_menu) {
					gfx.Illuminate();
					gfx.DrawRect(0, 0, Graphics::ScreenWidth, Graphics::ScreenHeight, Colors::Black);
					gfx.DrawSprite(Vec2(Graphics::ScreenWidth / 2 - gfx.sl->operator[](backgroundSurf)->GetWidth() / 2, 0), backgroundSurf);
					intf.Draw(wnd, font, gfx);
				}
				if (state == Status::st_map) {
					gfx.ClearScreen(Colors::Black);
					GetRooms()[rn].DrawAtMap(mapDrawRecLimit, 0, Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2), GetRooms()[rn], -1, Vec2(0, 0), GetRooms(), gfx, font);
				}
				if (state == Status::st_armory) {
					gfx.ClearScreen(Colors::Black, 1.0f - lightLevel);
					lightLevelTarget = 0.2;
					gfx.Illuminate(1);
					int chosenStat = intf.lbg[0].Draw(armoryStatSelected, hero, font, gfx);
					statForUpgrade = chosenStat; 
					Vec2 descDrawVec = Vec2::GeneratePoints(Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2), 4, 100)[0] +
						 Vec2(0, 100);
					string lvlCrntStr = (to_string(hero.GetStats(ItemParameters(chosenStat))) + "/" + to_string(hero.statMax));
					if (hero.GetStats(ItemParameters(chosenStat)) < 10) {
						lvlCrntStr = lvlCrntStr;
					}
					int lvlCrntSize = 1;
					Vec2 lvlCrntOffsetVec = Vec2(float(lvlCrntStr.size())/2.0f * float(font.getGlyphWidth()) * float(lvlCrntSize) 
							- float(font.getGlyphWidth())/2.0f,
						font.getGlyphHeight()/2 * lvlCrntSize);
					font.Draw(lvlCrntStr, 
						descDrawVec + Vec2(0, -80) - lvlCrntOffsetVec + bottomArmoryCam.GetPos(),
						Colors::White, Colors::Magenta, lvlCrntSize, gfx);
					font.Draw("Bank: " + to_string(hero.statPointsToAllocate),
						descDrawVec + Vec2(-400, -250) + sideArmoryCam.GetPos(), 
						Colors::White, Colors::Magenta, 2, gfx);
					if (hero.GetStats(ItemParameters(chosenStat)) < hero.statMax) {
						font.Draw("Required: " + to_string(hero.GetStatCost(hero.GetStats(ItemParameters(chosenStat)))),
							descDrawVec + Vec2(-400, -250 + font.getGlyphHeight() * 2) + sideArmoryCam.GetPos(),
							Colors::White, Colors::Magenta, 2, gfx);
					}
					for (auto line : MinorItem::GetParameterText(ItemParameters(chosenStat))) {
						int centerOffset = (line.size() * font.getGlyphWidth() * 2) / 2;
						font.Draw(line, descDrawVec - Vec2(centerOffset,0) + bottomArmoryCam.GetPos(), Colors::White,Colors::Magenta,2,gfx);
						descDrawVec += Vec2(0, font.getGlyphHeight() * 2 + 5);
					}
					sideArmoryCam.LerpToLocation(Vec2(0, 0));
					bottomArmoryCam.LerpToLocation(Vec2(0, 0));
				}
				else {
					sideArmoryCam.LerpToLocation(Vec2(-Graphics::ScreenWidth, 0));
					bottomArmoryCam.LerpToLocation(Vec2(0, Graphics::ScreenHeight));
				}
				if (state == Status::st_inventory) {
					gfx.ClearScreen(Colors::Black, 1.0f - lightLevel);
					lightLevelTarget = 0.2;
					gfx.Illuminate(1);
					invCam.LerpToLocation(Vec2(0, 0));
					hero.inv.Draw(invCam.GetPos(), 3, 5, font, gfx);
				}
				else {
					invCam.LerpToLocation(Vec2(0, Graphics::ScreenHeight));
				}
				if (state == Status::st_game) {
					lightLevelTarget = GetLightLevel();
				}
			}
			else {
				gfx.DrawSprite(Vec2(Graphics::ScreenWidth / 2 - gfx.sl->operator[](backgroundSurf)->GetWidth() / 2, 0), backgroundSurf);
				intf.Draw(wnd, font, gfx);
				//string introText = "Press \"Enter\" to start this game";
				//font.Draw(introText, Vec2(gfx.ScreenWidth/2 - introText.length()/2, gfx.ScreenHeight/2), Colors::White, Colors::White, gfx);
			}
			if (!hero.isActive) {
				//hero is ded
				OnHeroDeath();
			}
			if (isLoadScreenJustDialogTextLol) {
				if (tScreen != nullptr) {
					tScreen->Update(0.1, *this);
				}
			}
		}
		else {
			if (tScreen != nullptr) {
				tScreen->Update(0.1, *this);
			}
		}
	}
	lightLevel += (lightLevelTarget - lightLevel) * lightSpeed * lightSpeedExtra;

	if (!isProduction) {
		/*font.Draw("FPS: " + to_string(int(1 / frameTimeDiff.count())),
		{ 0, 0 }, Colors::Black, Colors::Magenta, 2, gfx);
		font.Draw("nProjectiles: " + to_string(hero.projectiles.size()),
			{ 0, (float)font.getGlyphHeight() * 2 }, Colors::Black, Colors::Magenta, 2, gfx);
		font.Draw("Elapsed time: " + to_string(timePassed.count()),
			{ 0, (float)font.getGlyphHeight() * 4 }, Colors::Black, Colors::Magenta, 2, gfx);*/
		vector<std::string> debugDraw = {
			"FPS: " + to_string(int(1 / fpsDiff.count())),
			"nProjectiles: " + to_string(hero.projectiles.size()),
			"Elapsed time: " + to_string(timePassed.count()),
			"Save point: " + to_string(savePoint),
			"Toll: " + to_string(crntToll),
			"flr text:" + GetRooms()[rn].floorText,
			"death count:" + to_string(hero.deathCount),
			"force sign show: " + to_string(GetRooms()[rn].forceSignUpdate)
			//"Effects: "
		};
		/*for (auto flag : Room::roomFlags) {
			debugDraw.push_back(flag.first + ": " + to_string(flag.second));
		}*/
		/*debugDraw.push_back("Stats:");
		for (auto stat : hero.stats) {
			debugDraw.push_back(" " + to_string(stat.first) + ": " + to_string(stat.second));
		}*/
		int i = 0;
		for (auto item : debugDraw) {
			font.Draw(item,
				{ 0, (float)font.getGlyphHeight() * 2 * i }, Colors::Black, Colors::Magenta, 2, gfx);
			if (item == "Effects: ") {
				hero.e.DrawTable(Vec2(0, (float)font.getGlyphHeight() * 2 * i), font, gfx);
			}
			i++;
		}
	}

	if (bigTextTimer > 0) {
		bigTextTimer--;
	}
	if (bigTextTimer > 0) {
		bigTextCam.LerpToLocation(Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2) - Vec2(0, 200));
	}
	else {
		bigTextCam.LerpToLocation(Vec2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight + 100));
	}
	int btScale = 3;
	font.Draw(bigText, bigTextCam.GetPos() - Vec2((bigText.size() * font.getGlyphWidth() * btScale) / 2, 0),
		Colors::Black, Colors::Magenta, btScale, gfx);

	//for(int i = 0; i < 4; i ++){
	//	gfx.DrawSpritePortion(Vec2( (wnd.mouse.GetPosX() - 500) * 5 + 105 * i, 100), 100, 100, Colors::Black, { 0, 0 }, Colors::Cyan, 1, "rocks",
	//		{ { Graphics::DrawSpecials::rotation, i } });
	//}

	//for (int i = 0; i < 4; i++) {
	//	gfx.DrawSpritePortion(Vec2( (wnd.mouse.GetPosX()-500) * 5 + 105 * i, 300), 100, 100, Colors::White, { 0, 0 }, Colors::Cyan, 1, "rocks",
	//		{ { Graphics::DrawSpecials::rotation, i } });
	//}
	if (lightLevel < 0.99) {
		gfx.ApplyLight();
	}
}