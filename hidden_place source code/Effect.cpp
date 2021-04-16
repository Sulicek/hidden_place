#include "Effect.h"

void Effect::Draw(Vec2 vec_in, Graphics &gfx) {
	gfx.DrawRect(vec_in.x, vec_in.y, size, size, c);
}

float* Effect::GetDataDirect(EffectParameters effectP_in)
{
	float pr_OMEGALUL_grammingSkillz = 0;
	return (data.count(effectP_in) > 0 ? &data[effectP_in] :  &pr_OMEGALUL_grammingSkillz);
}

float Effect::GetData(EffectParameters effectP_in)
{
	return (data.count(effectP_in) > 0 ? data[effectP_in] : 0);
}

Effect::Effect(EffectType eType_in, 
			int timeLeft_in, float damage_in)
{
	isPermanent = false;  
	switch (eType_in) {
	case EffectType::root:
		desc = "Rooted!";
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		isRemovable = false;
		break;
	case EffectType::ignite:
		desc = "Ignite";
		data[EffectParameters::timeLeft] = timeLeft_in;
		data[EffectParameters::maxStacks] = 5;
		data[EffectParameters::defaultMaxTime] = 500;
		//data[EffectParameters::maxStacks] = 0;
		isPermanent = false;
		isStackable = true;
		isRemovable = false;
		break;
	case EffectType::burning:
		desc = "On fire";
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		data[EffectParameters::damageP] = damage_in;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::soul:
		desc = "Souls";
		c = Color(80, 181, 161);
		data[EffectParameters::maxStacks] = 1;
		isPermanent = true;
		isStackable = true;
		isRemovable = false;
		break;
	case EffectType::reaping:
		desc = "Reaping";
		isPermanent = true;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::demon:
		desc = "Demon form";
		data[EffectParameters::timeLeft] = timeLeft_in;
		data[EffectParameters::progress] = 0;
		data[EffectParameters::maxProgress] = 100;
		isPermanent = false;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::manaShield:
		desc = "Mana shield";
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::dash:
		desc = "Dash";
		data[EffectParameters::timeLeft] = timeLeft_in;
		data[EffectParameters::targetX] = 0;
		data[EffectParameters::targetY] = 0;
		data[EffectParameters::targetDist] = 0;
		isPermanent = false;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::invulnerable:
		desc = "Invulnerability";
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::canChangeSpells:
		desc = "Shrine pressence";
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::healing:
		desc = "Heal over time";
		data[EffectParameters::healingOverTime] = 1;
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		isStackable = false;
		isRemovable = false;
		break;
	case EffectType::attackBoost:
		desc = "Attack boost";
		data[EffectParameters::maxStacks] = 0;
		data[EffectParameters::defaultMaxTime] = 50;
		data[EffectParameters::timeLeft] = timeLeft_in;
		isPermanent = false;
		isStackable = true;
		isRemovable = false;
		break;
	}
}
