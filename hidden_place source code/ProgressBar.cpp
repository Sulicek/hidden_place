#pragma once
#include "ProgressBar.h"
#include <iostream>
#include "Room.h"
void ProgressBar::Draw(Graphics &gfx, int padding, Vec2 pos, Parameters params) {

	
	auto tempVec = params.Get<Vec2*>();
	auto tempWH = params.Get<std::pair<int, int>*>();
	int widthCrnt = width;
	int heightCrnt = height;
	Vec2 vecCrnt = vec;
	if (tempVec != nullptr) {
		vecCrnt = *tempVec;
	}
	if (tempWH != nullptr) {
		width = tempWH->first;
		height = tempWH->second;
	}

	int scale = 1;//temp, maybe use later
	float progressCrntF = prgCrnt;
	float progressMaxF = prgMax;
	float crntToMaxRatio = (float)prgCrnt / (float)prgMax;
	float targToMaxRatio = (float)prgTarg / (float)prgMax;
	float actualWidthCrnt = widthCrnt * crntToMaxRatio * (float)scale;
	float actualWidthTarg = widthCrnt * targToMaxRatio * (float)scale;
	
	if (actualWidthCrnt > widthCrnt * (float)scale) {
		actualWidthCrnt = widthCrnt * (float)scale;
	}


	actualWidthTarg = min(actualWidthTarg, widthCrnt * (float)scale);
	if (widthCrnt  * (float)scale - actualWidthCrnt - padding * 2 > 1) {
		gfx.DrawRectBlend(vecCrnt.x + pos.x, vecCrnt.y + pos.y, widthCrnt * (float)scale,
			heightCrnt * (float)scale, bg, Colors::Black);

		gfx.DrawRectBlend(vecCrnt.x + padding + pos.x,
			vecCrnt.y + padding + pos.y,
			widthCrnt  * (float)scale - actualWidthCrnt - padding * 2,
			heightCrnt - padding * 2, highlightC, Color(100, 100, 100));

		auto newColor = Color::rgb2hsv(
			{ (float)c.GetR() / 255.0, (float)c.GetG() / 255.0, (float)c.GetB() / 255.0 });
		newColor.v /= 2;

		gfx.DrawRectBlend(vecCrnt.x + padding + pos.x,
			vecCrnt.y + padding + pos.y,
			widthCrnt - actualWidthTarg - padding * 2,
			heightCrnt - padding * 2, c, Color::rgbToColor(Color::hsv2rgb(newColor)));
	}
}

void ProgressBar::Draw(Graphics &gfx, int padding) {
	Draw(gfx, padding, Vec2(0, 0));
}
void ProgressBar::Draw(Vec2 vec_in, int width_in, int height_in, int prgCrnt_in, int prgMax_in,
	Color c_in, Color bg_in, Graphics &gfx, int padding) {


	float progressCrntF = prgCrnt_in;
	float progressMaxF = prgMax_in;
	float crntToMaxRatio = progressCrntF / progressMaxF;
	float actualWidth = width_in * crntToMaxRatio;

		gfx.DrawRect(vec_in.x, vec_in.y, width_in, height_in, bg_in);
		gfx.DrawRect(	vec_in.x + padding,
						vec_in.y + padding,
						actualWidth - padding * 2,
			height_in - padding * 2, c_in);

	
}



void ProgressBar::Update(Vec2 clip, int prgCrnt_in, int prgMax_in, Room & room){
	//clipping
	vec.x = clip.x;
	vec.y = clip.y;
	Update(prgCrnt_in, prgMax_in);
	//prgTarg += float(((float)prgCrnt_in - (float)prgTarg) * step);
	//prgCrnt += float(((float)prgCrnt_in - (float)prgCrnt) * stepHighlight);
	//prgMax = prgMax_in;
}

void ProgressBar::Update(int prgCrnt_in, int prgMax_in) {

	prgTarg += float(((float)prgCrnt_in - (float)prgTarg) * step);

	prgCrnt += float(((float)prgCrnt_in - (float)prgCrnt) * stepHighlight);
	if (prgCrnt < prgCrnt_in) {
		prgCrnt += stepHighlight;
	}
	else if (prgCrnt > prgCrnt_in) {
		prgCrnt -= stepHighlight;
	}
	
	prgMax = prgMax_in;
}

void ProgressBar::DrawManual(float prgCrnt_in, float prgMax_in, int orientation, bool isReverse, Vec2 vec_in, Color c_in, Color bg, int width_in, int height_in, Graphics &gfx)
{
	float crntToMaxRatio;
	if (!isReverse) {
		crntToMaxRatio = prgCrnt_in / prgMax_in;
	}
	else {
		crntToMaxRatio = (prgMax_in - prgCrnt_in)/ prgMax_in;
	}
	gfx.DrawRectBlend(vec_in.x, vec_in.y, width_in, height_in, bg, Colors::Black);
	Color edge = Colors::BackgroundGrey;
	switch (orientation) {
	case 0:
		gfx.DrawRectBlend(vec_in.x + ( width_in - float(width_in) * crntToMaxRatio), vec_in.y, int(float(width_in) * crntToMaxRatio), height_in, c_in, edge);
		break;
	case 1:
		gfx.DrawRectBlend(vec_in.x, vec_in.y, float(width_in) * crntToMaxRatio, height_in, c_in, edge);
		break;
	case 2:
		gfx.DrawRectBlend(vec_in.x, vec_in.y + (height_in - float(height_in) * crntToMaxRatio), width_in, float (height_in) * crntToMaxRatio, c_in, edge);
		break;
	case 3:
		gfx.DrawRectBlend(vec_in.x, vec_in.y, width_in, float(height_in) * crntToMaxRatio, c_in, edge);
		break;
	}
}


ProgressBar::ProgressBar(Vec2 &vec_in, Color c_in,  Color bg_in, int width_in, int height_in){
	vec = vec_in;
	c = c_in;
	width = width_in;
	height = height_in;
	bg = bg_in;
}