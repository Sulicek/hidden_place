/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "Vec2.h"
#include <cmath>
#include <windows.h>
#include "Rect.h"
#include <string>
#include "Cone.h"

using namespace std;

/*
float GetNormalizedSrd();
float GetNormalized();
*/

//ignore this
#define M_PI 3.14159265358979323846 

Vec2::Vec2(float x_in, float y_in) {
	x = x_in;
	y = y_in;
}

Vec2::Vec2() {}

bool Vec2::operator!=(Vec2 & in)
{
	return (x != in.x) || (y != in.y);
}

bool Vec2::operator==(Vec2 & in)
{
	return x == in.x && y == in.y;
}

void Vec2::operator+=(Vec2 & in)
{
	x += in.x;
	y += in.y;
}

void Vec2::operator-=(Vec2 & in)
{
	x -= in.x;
	y -= in.y;
}


float Vec2::GetLenght() {
	return sqrt(x*x + y*y);
}

float Vec2::GetLenghtSqrt() {
	return x * x + y * y;
}

Vec2 Vec2::GetNormal()
{
	return Vec2(y, -x);
}

//returns in what direction is vec1 to the vec2
Vec2 Vec2::GetDeltaVecNorm(Vec2 vec_in1, Vec2 vec_in2)
{
	return Vec2(vec_in1.x - vec_in2.x, vec_in1.y - vec_in2.y).GetNormalized();
}

bool Vec2::IsColliding(Rect rect1, Rect rect2)
{
	return IsColliding(rect1.offsetVec, rect2.offsetVec, rect1.width, rect1.height, rect2.width, rect2.height);
}

bool Vec2::IsColliding(Vec2 & vec1_in, Vec2 & vec2_in, int width1, int height1, int width2, int height2)
{
	return
		vec1_in.x < vec2_in.x + width2 &&
		vec1_in.y < vec2_in.y + height2 &&
		vec2_in.x < vec1_in.x + width1 &&
		vec2_in.y < vec1_in.y + height1;
}

bool Vec2::IsColliding(Vec2 & vec_in, int width, int height, Cone cone_in)
{
	int maxX = max(max(cone_in.p1.x, cone_in.p2.x), max(cone_in.spike.x, cone_in.vec.x));
	int maxY = max(max(cone_in.p1.y, cone_in.p2.y), max(cone_in.spike.y, cone_in.vec.y));
	int minX = min(min(cone_in.p1.x, cone_in.p2.x), min(cone_in.spike.x, cone_in.vec.x));
	int minY = min(min(cone_in.p1.y, cone_in.p2.y), min(cone_in.spike.y, cone_in.vec.y));

	float deltaAngle = (float(2) * M_PI) - Vec2::GetDeltaVec(cone_in.p1, cone_in.GetVec()).GetAngle();
	float angle = float(cone_in.spell->GetData(SpellParameters::coneAngle)) * M_PI / float(180);

	if (Vec2::IsColliding(Vec2(minX, minY), vec_in, maxX - minX, maxY - minY, width, height)) {
		for (int ix = 0; ix < width; ix++) {
			for (int iy = 0; iy < height; iy++) {
				if (
					int(abs(Vec2::GetDeltaVec(Vec2(vec_in.x + ix, vec_in.y + iy), cone_in.GetVec()).GetLenght())) <= cone_in.GetRadius() &&
					Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(vec_in.x + ix, vec_in.y + iy), cone_in.GetVec()).GetAngle() + deltaAngle) <= angle &&
					Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(vec_in.x + ix, vec_in.y + iy), cone_in.GetVec()).GetAngle() + deltaAngle) > 0
					) {
					return true;
				}
			}
		}
	}
		return false;
	
}

bool Vec2::IsColliding(Vec2 & vec1_in, Vec2 & vec2_in, int r1_in, int r2_in)
{
	return GetDeltaVec(vec1_in, vec2_in).GetLenght() <= r1_in + r2_in;
}

std::vector<Vec2> Vec2::GeneratePoints(Vec2 vec_in, int nPoints, int distance)
{
	std::vector<Vec2> out;
	switch (nPoints) {
	case 1:
		out.push_back(Vec2(vec_in.x, vec_in.y - distance));
		break;
	case 2:
		out.push_back(Vec2(vec_in.x, vec_in.y + distance));
		out.push_back(Vec2(vec_in.x, vec_in.y));
		break;
	case 3:
		out.push_back(Vec2(vec_in.x, vec_in.y + distance));
		out.push_back(Vec2(vec_in.x - distance, vec_in.y - distance));
		out.push_back(Vec2(vec_in.x + distance, vec_in.y - distance));
		break;
	case 4:
		out.push_back(Vec2(vec_in.x, vec_in.y + distance));
		out.push_back(Vec2(vec_in.x, vec_in.y + distance) + Vec2( 150-60, -60));
		out.push_back(Vec2(vec_in.x, vec_in.y + distance) + Vec2( 220-60, -130));
		out.push_back(Vec2(vec_in.x, vec_in.y + distance) + Vec2( 250-60, -220));
		/*out.push_back(Vec2(vec_in.x - distance, vec_in.y - distance));
		out.push_back(Vec2(vec_in.x, vec_in.y - float(distance) * 1.3));
		out.push_back(Vec2(vec_in.x + distance, vec_in.y - distance));*/
		break;
	default:
		for (int i = 0; i < nPoints; i++) {
			out.push_back(Vec2(vec_in.x - (nPoints * distance) / 2 + i * distance, vec_in.y));
		}
	}
	return out;
}

//returns in what direction and in what distance is object 1 to the other object
Vec2 Vec2::GetDeltaVec(Vec2 vec_in1, Vec2 vec_in2)
{
	return { vec_in1.x - vec_in2.x, vec_in1.y - vec_in2.y };
}

Vec2 Vec2::GetVecByAngle(float angle_in) {
	float angle = angle_in;


	if (angle > (float(2) * M_PI)) {
		for (int i = 0; angle > (float(2) * M_PI); i++) {
			angle -= float(2)* M_PI;
		}
	}
	if (angle < 0) {
		for (int i = 0; angle < 0; i++) {
			angle += float(2)* M_PI;
		}
	}

	if (0 <= angle && angle < (M_PI / float(2))) {
		return Vec2(1, tan(angle)).GetNormalized();
	}
	else if((M_PI / float(2)) <= angle && angle < (M_PI)){
		return Vec2(-1, tan( M_PI - angle )).GetNormalized();
	}
	else if (M_PI <= angle && angle < ((M_PI / float(2)) * 3)) {
		return Vec2(-1, -tan(angle - M_PI)).GetNormalized();
	}else if (((M_PI / float(2)) * 3) <= angle && angle < (M_PI *2)) {
		return Vec2(1, -tan(float(2) * M_PI - angle)).GetNormalized();
	}
	else {
		return Vec2(1, 1);
	}
}

float Vec2::GetAngle() {
	if (x == 0 && y == 0) {
		return 0;
	}
	/*if (x >= 0 && y >= 0) {
		return atan(y / x);
	}
	if (x < 0 && y >= 0) {
		return -abs(atan(y / x)) + M_PI;
	}
	if (x < 0 && y < 0) {
		return atan(y / x) + M_PI;
	}
	if (x >= 0 && y < 0) {
		return atan(y / x) + float(2) * M_PI;
	}*/
	return atan2(y, x);
}
Vec2 Vec2::operator-(Vec2 & in)
{
	return Vec2(this->x-in.x, this->y-in.y);
}

Vec2 Vec2::operator-=(float in)
{
	return Vec2(this->x - in, this->y - in);
}
Vec2 Vec2::operator+=(float in)
{
	return Vec2(this->x + in, this->y + in);
}
Vec2 Vec2::operator+(Vec2 & in)
{
	return Vec2(this->x + in.x, this->y + in.y);
}
Vec2 Vec2::operator*(float  in) {
	return Vec2(this->x * in, this->y * in);
}

float Vec2::operator*(Vec2 in)
{
	return in.x * x + in.y * y;
}

Vec2 Vec2::GetNormalized() {
	if(Vec2::GetLenghtSqrt() == 0){
		return Vec2(0, 1);
	}
	return Vec2(x / Vec2::GetLenght(), y / Vec2::Vec2::GetLenght());
}

float Vec2::FixAngle(float angle_in) {
	float angle = angle_in;
	if (angle > (float(2) * M_PI)) {
		while(angle > (float(2) * M_PI)){
			angle -= float(2)* M_PI;
		}
	}
	return angle;
}