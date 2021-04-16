#pragma once
#include "Vec2.h"

class Camera {
public:
	Vec2 pos = Vec2(0, 0);
	Vec2 vel = Vec2(0, 0);
	float step = 0.1;
	float resistance = 0.7;
	void LerpToLocation(Vec2 vec_in);
	void Update();
	void AddForce(Vec2 force_in);
	Vec2 GetPos();
	Camera() = default;
	Camera(float step_in, float resistance_in);
};