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
#include <vector>

class Vec2 {
public:
	float x, y;
public:
	Vec2(float x_in, float y_in);
	Vec2();
public:
	bool operator!=(Vec2 &in);
	bool operator==(Vec2 &in);
	void operator+=(Vec2 &in);
	void operator-=(Vec2 &in);
	Vec2 operator-(Vec2 &in);
	Vec2 operator+(Vec2 &in);
	float operator*(Vec2  in);
	Vec2 operator-=(float in);
	Vec2 operator+=(float in);
	Vec2 operator*(float  in);
	Vec2 GetNormalized();
	float GetLenght();
	float GetLenghtSqrt();
	Vec2 GetNormal();
	//returns the vector, which has the inputet angle with the positive x axis hopefully
	static Vec2 GetVecByAngle(float angle);
	//returns the angle to the positive x axis. Might not be working entirely perfectly, but it is good enough for now lol xD ((I hope))
	float GetAngle();
	static float FixAngle(float angle_in);
	static Vec2 GetDeltaVec(Vec2 vec_in1, Vec2 vec_in2);
	static Vec2 GetDeltaVecNorm(Vec2 vec_in1, Vec2 vec_in2);
	//is colliding function for 2 squares
	static bool IsColliding(Vec2 &vec1_in, Vec2 &vec2_in, int width1, int height1,
		int width2, int height2);
	static bool IsColliding(class Rect rect1, class Rect rect2);
	//is colliding function for a square and a cone
	static bool IsColliding(Vec2 &vec_in, int width, int height, class Cone cone_in);
	//is colliding function for 2 circles
	static bool IsColliding(Vec2 &vec1_in,Vec2 &vec2_in, int r1_in, int r2_in);
	static std::vector<Vec2>GeneratePoints(Vec2 vec_in, int nPoints, int distance);

};