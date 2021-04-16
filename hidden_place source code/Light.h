#pragma once
#include "Graphics.h"
#include "Vec2.h"
#include "Colors.h"
#include <map>

class Light{
private:
	float lightLevelTarget = 1;
public:
	//this might get little confusing,
	//because we have 2 things 
	//(flicker, and regular intensity change/t)
	//that lerp, ergo there's a lot of values to keep
	//track of everything over time
	//core light intensity
	float intensity = 500;
	//intensity affected by flicker
	float intensityCrnt = intensity;
	//target of the intensity for flicker
	float intensityTarget;
	float flicker = 0.2;
	float flickerStep = 0.7;
	float maxOpacity = 1;
	float lightLevel = 1;
	//float lightLevelTarget = 1;
	float lightSpeed = 0.3;
	Color c;
	std::map<Graphics::DrawSpecials, float> specials = {
		{Graphics::DrawSpecials::jagged, 3},
		{Graphics::DrawSpecials::squary, 5},
		{Graphics::DrawSpecials::shine, 0},
	};
	Light(float intensity_in, float flicker_in, bool isGrowing,Color c_in);
	void Draw(Vec2 vec_in, Graphics &gfx, int rCap = -1);
	void DrawControlled(Vec2 vec_in, float intensity_in,
		Color c_in, Graphics &gfx);
	void On();
	void Off();
	void Shine();
	void NotShine();
};