#pragma once
#include "Surface.h"
#include <string>
#include <map>
#include <thread>

class SpriteLoader{
public:
	std::map<std::string, std::string> stringMap = {};
	std::map<std::string, Surface*> spriteMap;
	std::map<std::string, std::thread*> threadMap;
	Surface* operator[](std::string s);
	//bool isLoaded = false;
	//std::thread LoadChecker;
	void StartThread(std::string name, Surface** result);
	void Load(std::map<std::string, std::string> in);
	bool LoadAll(int limit, std::vector<std::string> surfs = {});
	int timePerSprite = 100;
	float timePerSpriteCrnt = timePerSprite;
	SpriteLoader() = default;
};