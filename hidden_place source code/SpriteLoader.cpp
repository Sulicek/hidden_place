#include "SpriteLoader.h"
#include <thread>

void LoadSprite(std::string name, Surface** result) {
	*result = new Surface(name);
}

Surface * SpriteLoader::operator[](std::string s)
{
	Surface *out;
	if (!(spriteMap.count(s) > 0)) {
		if (threadMap.count(s) == 0) {
			//threadMap[s] = new std::thread(LoadSprite, stringMap[s], &(spriteMap[s]));
			StartThread(s, &(spriteMap[s]));
		}
	}
	if (threadMap.count(s) > 0) {
		threadMap[s]->join();
		threadMap.erase(s);
	}
	out = spriteMap[s];
	return out;
}

void SpriteLoader::StartThread(std::string stringKey, Surface ** result)
{
	threadMap[stringKey] = new std::thread(LoadSprite, stringMap[stringKey], result);
}

void SpriteLoader::Load(std::map<std::string, std::string> in)
{
	stringMap = in;
}

bool SpriteLoader::LoadAll(int limit, std::vector<std::string> surfs)
{
		int counter = 0;
		if (surfs.size() <= 0) {
			for (auto item : stringMap) {
				if (spriteMap.count(item.first) <= 0) {
					Surface* surf;
					//threadMap[item.first] = new std::thread(LoadSprite, item.second, &(spriteMap[item.first]));
					StartThread(item.first, &(spriteMap[item.first]));
					counter++;
				}
				if (counter >= limit) {
					break;
				}
			}
		}
		return counter == 0;
}


