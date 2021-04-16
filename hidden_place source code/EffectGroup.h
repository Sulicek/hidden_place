#pragma once
#include <vector>
#include "Effect.h"
#include "Graphics.h"
#include "Vec2.h"
#include <map>
#include "Font.h"

class EffectGroup {
public:
	std::map<EffectType, Effect> effects;
	class Inventory *inv;
public:
	EffectGroup(class Inventory* inv_in = nullptr);
	Effect* Get(EffectType eType);
	//does the group have the certain effect?
	bool Has(EffectType eType);
	//adding effect into the group 
	void Add(EffectType eType);
	void Add(EffectType eType, int timeLeft);
	void Add(EffectType eType, int timeLeft, float damage);
	void RefreshTime(EffectType eType);
	//removing the effect from the group one stack by one
	void Remove(EffectType eType);
	int GetCount(EffectType eType);
	int GetStacks(EffectType eType);
	float GetData(EffectType eType_in, EffectParameters eParam_in);
	void Update();
	void DrawEffect(Vec2 vec_in, EffectType eType_in, bool isReverse, Graphics &gfx);
	void DrawTable(Vec2 vec_in, Font &font, Graphics &gfx);
};