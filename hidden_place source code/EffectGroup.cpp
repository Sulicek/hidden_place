#include "EffectGroup.h"
#include "Inventory.h"

EffectGroup::EffectGroup(Inventory * inv_in)
{
	inv = inv_in;
}

Effect* EffectGroup::Get(EffectType eType) {
	if (effects.count(eType) > 0) {
		return &effects[eType];
	}
	else {
		return nullptr;
	}
}

bool EffectGroup::Has(EffectType eType) {
	return (effects.count(eType) > 0);
}

void EffectGroup::Add(EffectType eType) {
	Add(eType, 0, 0);
	*(Get(eType)->GetDataDirect(EffectParameters::timeLeft)) = GetData(eType, EffectParameters::defaultMaxTime);
}

void EffectGroup::Add(EffectType eType, int timeLeft) {
	Add(eType, timeLeft, 0);
}

void EffectGroup::Add(EffectType eType, int timeLeft, float damage) {
	if (GetCount(eType) > 0) {
		if (Get(eType)->isStackable && Get(eType)->stacks < GetData(eType, EffectParameters::maxStacks)) {
			Get(eType)->stacks++;
		}
		*Get(eType)->GetDataDirect(EffectParameters::timeLeft) = timeLeft;
	}
	else {
		effects[eType] = Effect(eType, timeLeft, damage);
	}
}

void EffectGroup::RefreshTime(EffectType eType)
{
	if (Has(eType)) {
		*(Get(eType)->GetDataDirect(EffectParameters::timeLeft)) = GetData(eType, EffectParameters::defaultMaxTime);
	}
}

void EffectGroup::Remove(EffectType eType) {
	Get(eType)->stacks--;
	if (Get(eType)->stacks <= 0) {
		effects.erase(eType);
	}
}

int EffectGroup::GetCount(EffectType eType) {
	return effects.count(eType);
}

int EffectGroup::GetStacks(EffectType eType) {
	if (Has(eType)) {
		return effects[eType].stacks;
	}
	else {
		return 0;
	}
}

float EffectGroup::GetData(EffectType eType_in, EffectParameters eParam_in){
	if (inv != nullptr) {
		if (effects.count(eType_in)) {
			return Get(eType_in)->GetData(eParam_in);//+ inv->GetEffectData(eType_in, eParam_in);
		}
	}
	else {
		if (effects.count(eType_in)) {
			return Get(eType_in)->GetData(eParam_in);
		}
	}
}

void EffectGroup::Update() {
	for (std::map<EffectType, Effect>::iterator it = effects.begin(); it != effects.end(); it++) {
		//checking max stacks
		if (Get(it->first)->stacks > GetData(it->first, EffectParameters::maxStacks)) {
			Get(it->first)->stacks = GetData(it->first, EffectParameters::maxStacks);
		}
		if (!it->second.isPermanent) {
			if (*it->second.GetDataDirect(EffectParameters::timeLeft) > 0) {
				*(it->second.GetDataDirect(EffectParameters::timeLeft)) = *(it->second.GetDataDirect(EffectParameters::timeLeft)) - 1;
			}
			else {
				effects.erase(it->first);
			}
		}
	}

}

void EffectGroup::DrawEffect(Vec2 vec_in, EffectType eType_in, bool isReverse, Graphics &gfx) {
	if (GetCount(eType_in) > 0) {
		//setting padding
		int padding = 5;

		if (isReverse) {
			vec_in.x -= effects[eType_in].stacks * padding + effects[eType_in].stacks * effects[eType_in].size;
		}

		//drawing it
		for (int i = 0; i < effects[eType_in].stacks; i++) {
			effects[eType_in].Draw(Vec2(vec_in.x + i * padding + i * effects[eType_in].size, vec_in.y), gfx);
		}
	}
}

void EffectGroup::DrawTable(Vec2 vec_in, Font &font, Graphics & gfx)
{
	int i = 0;
	for (auto &e : effects) {
		//font.Draw(std::to_string(e.second.stacks) + "_" +
			//std::to_string(int(e.second.GetData(EffectParameters::timeLeft))) + "_" +
			//e.second.desc, vec_in +
			//Vec2(0, i * font.getGlyphHeight()), Colors::White, e.second.c, gfx);
		font.DrawNoBg(std::to_string(e.second.stacks) + "_" +
			std::to_string(int(e.second.GetData(EffectParameters::timeLeft))) + "_" +
			e.second.desc,
			{ vec_in.x , vec_in.y + font.getGlyphHeight() * i }, gfx);
		i++;
	}
}
