#pragma once
#include "Sound.h"
#include <map>
#include <string>
#include "json.hpp"

using namespace std;

class SoundManager {
private:
	static map<string, vector<Sound*>> sounds;
	static map<string, bool> isPlaying;
	static float volume;
public:
	static void LoadSounds(nlohmann::json j);
	static void Play(string name);
	static void PlayOne(string name);
	static void Stop(string name);
	static void StopAll();
	static Sound * GetRandEl(string name);
};