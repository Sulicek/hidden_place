#include "SoundManager.h"
#include "Sound.h"
#include <map>
#include <string>
#include "json.hpp"

using namespace std;

map<string, vector<Sound*>> SoundManager::sounds = {};
map<string, bool> SoundManager::isPlaying = {};
float SoundManager::volume = 1.0f;

wstring StringToWString(const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());
	return wsTmp;
}

void SoundManager::LoadSounds(nlohmann::json j) {
	for (auto &item : j) {
		string name = item["name"];
		vector<string> filenames = {};
		if(item.count("filename")){
			filenames.push_back(item["filename"].get<string>());
		}
		else {
			filenames = item["filenames"].get<vector<string>>();
		}
		for (auto &filename : filenames) {
			if (item.count("loopStart") > 0) {
				float loopStart = item["loopStart"];
				float loopEnd = item["loopEnd"];
				string fileLocation = "Resources\\Sounds\\" + filename;
				sounds[name].push_back(new	 Sound(StringToWString(fileLocation), loopStart, loopEnd));
			}
			else {
				string fileLocation = "Resources\\Sounds\\" + filename;
				sounds[name].push_back(new Sound(StringToWString(fileLocation)));
			}
		}
	}
}

Sound * SoundManager::GetRandEl(string name) {
	return sounds[name][rand() % sounds[name].size()];
}

void SoundManager::Play(string name)
{
	if (sounds.count(name) > 0) {
		GetRandEl(name)->Play(1, volume);
		isPlaying[name] = true;
	}
	else if (name.find("_") != string::npos){
		Play(name.substr(0, name.find("_")));
	}
}

void SoundManager::PlayOne(string name)
{
	if (sounds.count(name) > 0) {
		if (!isPlaying[name]) {
			GetRandEl(name)->Play(1, volume);
			isPlaying[name] = true;
		}
	}
}

void SoundManager::Stop(string name)
{
	if (sounds.count(name) > 0) {
		GetRandEl(name)->StopAll();
		isPlaying[name] = false;
	}
}

void SoundManager::StopAll()
{
	for (auto &sound : sounds) {
		for (auto &variant : sound.second) {
			variant->StopAll();
		}
		isPlaying[sound.first] = false;
	}
}
