#pragma once
#include "Graphics.h"
#include <string>
#include <vector>
#include <map>

struct SpritePos {
	int x, y;
};

enum AnimationType {
	RunUp, RunDown, RunLeft, RunRight, Idle, Casting, Striking, WindUp, Death
};

class Animation {
public:
	struct SoundStoredInAnim {
		std::string name;
		int frame = 0;
	};
	SoundStoredInAnim sound;
	int width = 1;
	int height = 1;
	bool isPlaying = false; //for static anims only
	bool isShownStatic = false;
	bool noAttackAnim = false;
	Vec2 vecStatic = Vec2(100, 100);
	std::map<AnimationType, std::vector<SpritePos>> animations;
	AnimationType aTypeCrnt = AnimationType::Idle;
	std::string surf;
	std::map<std::string, std::string> masks;
	//rotation in 90 angles clockwise
	int rotation = 0;
	//the frame of the animation that is being drawn
	int progress = 0;
	//variable keeping tract of the ammount of frames that we show the animation to the user
	int tempMax = 10;
	bool isBoosted = false;
	int tempMaxBoost;
	//variable keeping tract of the duration of the frame
	int temp = tempMax;
	//ammounts of frames in one animation
	int nFrames;
	Animation(std::string surf_in);
	//load the surf_in into the animation object
	void Load(std::string surf_in, int nFrames_in, std::vector<AnimationType> aTypes_in);
	void AddWH(int width_in, int height_in);
	void AddMask(std::string name, std::string maskSurf_in);
	//set the animation type to the aType_in, returns true if suceeded
	bool SetAnimationType(AnimationType aType_in);
	//raw drawing of the animation
	void DrawRaw(Vec2 vec_in, class Rect drawRect_in, AnimationType aType_in, Graphics & gfx);
	//draw the animation frame in the drawRect + vec_in with gfx
	void Draw(Vec2 vec_in, class Rect drawRect_in, Vec2 pos, Graphics & gfx);
	//draw the animation frame in the drawRect + vec_in with gfx, overriding everything with color overrideC
	void Draw(Vec2 vec_in, class Rect drawRect_in, Vec2 pos, Color overrideC, Graphics & gfx);
	void Draw(Vec2 vec_in, class Rect drawRect_in, Vec2 pos, Color overrideC, float opacity, Graphics & gfx);
	void DrawMask(std::string name, Vec2 vec_in, class Rect drawRect_in, Vec2 pos, Color overrideC, float opacity, Graphics & gfx);
	//update the animation object
	void Update();
	//sets the rotation (in 90 degrees clockwise)
	void SetRotation(int rotation_in);
	//returns wether the animation has the animation type
	bool Has(AnimationType aType_in);
	void Refresh();
	void Boost(int framesPerFrameBoost);
	
	//static anims
	static std::map<std::string, std::vector<void*>> staticAnims;
	static void UpdateStatic();
	static void AddStaticAnim(std::string animName, std::string surf_in, 
		int nFrames_in, int width_in, int height_in, int index = 0);
	static void DrawStaticGeneral(Vec2 pos, Graphics & gfx);
	static void Play(std::string animName);
	static void Show(std::string animName, Vec2 vec_in);
	static void SetFps(std::string animName, int framesPerFrame, int index = 0);
	static void DrawStatic(std::string animName,
		Vec2 vec_in, Vec2 pos, Graphics & gfx, int index = 0);
};