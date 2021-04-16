#include <string>
#include <queue>
class SoundLib {
public:
	std::queue<std::string> soundQueue;
	void PlaySoundEffect(std::string sound)
	{
		soundQueue.push(sound);
	}
	void ProcessSounds();
};