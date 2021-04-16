#include "Light.h"

Light::Light(float intensity_in, float flicker_in, bool isGrowing, Color c_in)
{	
	if (intensity_in != -1) {
		intensity = intensity_in;
	}
	if (isGrowing) {
		lightLevel = 0;
	}
	c = c_in;
	if (flicker_in != -1) {
		flicker = flicker_in;
	}
	intensityTarget = intensity;
}

void Light::Draw(Vec2 vec_in,Graphics & gfx, int rCap)
{
	lightLevel += (lightLevelTarget - lightLevel) * lightSpeed;
	float tolerance = 1;
	if ((intensityCrnt - intensityTarget) < tolerance) {
		float maxIntChange = intensity * flicker;
		float deltaInt = 0;
		if (maxIntChange > 0) {
			deltaInt = rand() % (int)maxIntChange;
		}
		intensityTarget = intensity + deltaInt;
	}
	intensityCrnt += (intensityTarget-intensityCrnt) * flickerStep;
	specials[Graphics::DrawSpecials::radiusCap] = rCap;
	if (intensity + intensityCrnt > 0) {
		gfx.DrawFlame(vec_in, intensityCrnt * lightLevel, maxOpacity, c, 
			specials);
	}
}

void Light::DrawControlled(Vec2 vec_in, float intensity_in,
	Color c_in, Graphics &gfx)
{
	lightLevel += (lightLevelTarget - lightLevel) * lightSpeed;
	float tolerance = 1;
	if ((intensityCrnt - intensityTarget) < tolerance) {
		float maxIntChange = intensity * flicker;
		float deltaInt = 0;
		if (maxIntChange > 0) {
			deltaInt = rand() % (int)maxIntChange;
		}
		intensityTarget = intensity + deltaInt;
	}
	intensityCrnt += (intensityTarget - intensityCrnt) * flickerStep;
	if (intensity + intensityCrnt > 0) {
		gfx.DrawFlame(vec_in, intensityCrnt * lightLevel  * intensity_in, maxOpacity, c_in, specials);
	}
}


void Light::On()
{
	lightLevelTarget = 1;
}

void Light::Off()
{
	lightLevelTarget = 0;
}

void Light::Shine()
{
	specials[Graphics::DrawSpecials::shine] = 1;
}

void Light::NotShine()
{
	specials[Graphics::DrawSpecials::shine] = 0;
}
