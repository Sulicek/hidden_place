#include "Camera.h"

void Camera::LerpToLocation(Vec2 vec_in)
{
	Vec2 dir = Vec2::GetDeltaVec(vec_in, pos);
	pos += (dir.GetNormalized() * dir.GetLenght() * step);
}

void Camera::Update()
{
	pos += vel;
	vel = vel * resistance;
}

void Camera::AddForce(Vec2 force_in)
{
	vel = force_in;
}

Vec2 Camera::GetPos()
{
	return pos;
}

Camera::Camera(float step_in, float resistance_in)
{
	step = step_in;
	resistance = resistance_in;
}
