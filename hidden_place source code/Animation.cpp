#include "Animation.h"
#include "Rect.h"
#include "SoundManager.h"

using namespace std;

void Animation::Draw(Vec2 vec_in, Rect drawRect_in, Vec2 pos, Graphics & gfx){
	gfx.DrawSpritePortion(drawRect_in.offsetVec + pos + vec_in, drawRect_in.width, drawRect_in.height, animations.find(aTypeCrnt)->second[progress], surf);
}

void Animation::Draw(Vec2 vec_in, Rect drawRect_in, Vec2 pos, Color overrideC, Graphics & gfx) {
	gfx.DrawSpritePortion(drawRect_in.offsetVec + pos + vec_in, drawRect_in.width, drawRect_in.height, Colors::Magenta, 
		animations.find(aTypeCrnt)->second[progress], overrideC, surf);
}

void Animation::Draw(Vec2 vec_in, Rect drawRect_in, Vec2 pos, Color overrideC,float opacity, Graphics & gfx) {
	gfx.DrawSpritePortion(drawRect_in.offsetVec + pos + vec_in, drawRect_in.width, drawRect_in.height, 
		Colors::Magenta,animations.find(aTypeCrnt)->second[progress], overrideC,opacity, surf);
}

void Animation::DrawMask(std::string name, Vec2 vec_in, Rect drawRect_in, Vec2 pos, Color overrideC,
	float opacity, Graphics & gfx)
{
	if (masks.count(name) > 0) {
		gfx.DrawSpritePortion(drawRect_in.offsetVec + pos + vec_in, drawRect_in.width, drawRect_in.height, Colors::Magenta,
			animations.find(aTypeCrnt)->second[progress], overrideC, opacity, masks[name], { {Graphics::DrawSpecials::rotation, rotation} });
	}
}

void Animation::DrawRaw(Vec2 vec_in, Rect drawRect_in, AnimationType aType_in, Graphics & gfx) {
	gfx.DrawSpritePortion(vec_in, drawRect_in.width, drawRect_in.height, animations.find(aType_in)->second[progress], surf);


}

void Animation::Update(){
	temp--;
	if (temp == 0) {
		if (progress == sound.frame) {
			SoundManager::Play(sound.name);
		}
		progress++;
		if (progress >= nFrames) {
			if (aTypeCrnt == AnimationType::WindUp || isShownStatic) {
				progress = nFrames - 1;
			}
			else {
				progress = 0;
				isBoosted = false;
				if (aTypeCrnt == AnimationType::Striking || aTypeCrnt == AnimationType::Death) {
					SetAnimationType(AnimationType::Idle);
				}
			}
		}
		if (isBoosted) {//faster frame changes if boosted
			temp = tempMaxBoost;
		}
		else {
			temp = tempMax;
		}
	}
}

std::map<std::string, std::vector<void*>> Animation::staticAnims = { };

void Animation::UpdateStatic() {
	for (auto &as : staticAnims) {
		for (auto &a : as.second) {
			((Animation*)a)->Update();
		}
	}
}

void Animation::AddStaticAnim(std::string animName, std::string surf_in, int nFrames_in, int width_in, int height_in, int index)
{
	staticAnims[animName].push_back(new Animation(surf_in));
	((Animation*)staticAnims[animName][index])->Load(surf_in, nFrames_in, { AnimationType::Idle});
	((Animation*)staticAnims[animName][index])->AddWH(width_in, height_in);
}

void Animation::DrawStaticGeneral(Vec2 pos, Graphics & gfx)
{
	for (auto &anims : staticAnims) {
		int i = 0;
		std::vector<int> eraseLater = {};
		for (auto &anim : anims.second) {
			if (((Animation*)anim)->isShownStatic) {
				DrawStatic(anims.first, ((Animation*)anim)->vecStatic, pos, gfx, i);
			}
			if(!(((Animation*)anim)->isShownStatic) && 
				!(((Animation*)anim)->isPlaying &&
					i != 0)){
				eraseLater.push_back(i);
			}
			i++;
		}
		for (auto i : eraseLater) {
		//	anims.second.erase(anims.second.begin() + i);
		}
	}
}

void Animation::Play(std::string animName)
{
	Animation* a = ((Animation*)staticAnims[animName][0]);
	a->isPlaying = true;
	a->progress = 0;
}

void Animation::Show(std::string animName, Vec2 vec_in)
{
	Animation * a;
	a = (Animation*)staticAnims[animName][0];
	//if the animation is already running
	if (a->isPlaying) {
		//we add another one
		AddStaticAnim(animName, a->surf, a->nFrames, a->width, a->height, staticAnims[animName].size());
		a = (Animation*)staticAnims[animName][staticAnims[animName].size() - 1];
		//set fps from the original animation
		SetFps(animName, ((Animation*)staticAnims[animName][0])->tempMax, staticAnims[animName].size() - 1);
	}
	a->isPlaying = true;
	a->progress = 0;
	a->temp = a->tempMax;
	a->isShownStatic = true;
	a->vecStatic = vec_in;
}

void Animation::SetFps(std::string animName, int framesPerFrame, int index)
{
	((Animation*)staticAnims[animName][index])->tempMax = framesPerFrame;
}

void Animation::DrawStatic(std::string animName, Vec2 vec_in, Vec2 pos, Graphics & gfx, int index)
{
	Animation* a = ((Animation*)staticAnims[animName][index]);
	if (a->isPlaying) {
		a->Draw(vec_in - Vec2(a->width/2, a->height/2), Rect(Vec2(0, 0),
			a->width, a->height), pos, gfx);
		if (a->progress >= a->nFrames - 1) {
			a->isPlaying = false;
			a->isShownStatic = false;
		}
	}
}

void Animation::SetRotation(int rotation_in)
{
	rotation = rotation_in;
}

Animation::Animation(std::string surf_in):surf(surf_in){
}

void Animation::Load(std::string surf_in, int nFrames_in, std::vector<AnimationType> aTypes_in) {
	surf = surf_in;
	nFrames = nFrames_in;
	for (int o = 0; o < aTypes_in.size(); o++) {
		std::vector<SpritePos> temp;
		for (int i = 0; i < nFrames_in; i++) {
			temp.push_back({ i, o });
		}
		animations[aTypes_in[o]] = temp;
		temp.clear();
	}
}

void Animation::AddWH(int width_in, int height_in)
{
	width = width_in;
	height = height_in;
}

void Animation::AddMask(std::string name, std::string maskSurf_in)
{
	masks[name] = maskSurf_in;
}

bool Animation::SetAnimationType(AnimationType aType_in){
	//we change the animation type only if it is different than the current one to avoid bad
	//stuff 
	if (aType_in != aTypeCrnt && animations.count(aType_in) > 0 || noAttackAnim) {
		//we dont set attack anim if we don't have it
		if (aType_in == AnimationType::WindUp && noAttackAnim) {
			return true;
		}
		//we reset progress to avoid errors
		progress = 0;
		//we set the type accordingly
		aTypeCrnt = aType_in;
		
		return true;
	}
	return false;
}

bool Animation::Has(AnimationType aType_in) {
	return animations.count(aType_in) > 0;
}

void Animation::Refresh()
{
	if(isBoosted){
		temp = tempMaxBoost;
	}
	else {
		temp = tempMax;
	}
	progress = 0;
}

void Animation::Boost(int framesPerFrameBoost)
{
	tempMaxBoost = framesPerFrameBoost;
	isBoosted = true;
}
