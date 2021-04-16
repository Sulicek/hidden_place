#pragma once
#include "Graphics.h"
#include "Vec2.h"
#include "Colors.h"
#include "Parameters.h"

class ProgressBar {
public:
	Vec2 vec;
	Color c, bg;
	Color highlightC = Colors::White;
	float width, height;
	bool doesClip = false;
	float prgCrnt = 0;
	float prgMax = 0;
	float prgTarg = 0;
	float step = 0.3;
	float stepHighlight = 0.03;
public:
	ProgressBar(Vec2 &vec_in, Color c_in, Color bg,  int width_in, int height_in);
	void Draw(Graphics &gfx, int padding, Vec2 pos, Parameters params = Parameters());
	void Draw(Graphics &gfx, int padding);
	void Draw(Vec2 vec_in, int width_in, int height_in,int prgCrnt_in, int prgMax_in,
				Color c_in, Color bg_in, Graphics &gfx, int padding);
	void Update(Vec2 clip, int prgCrnt_in, int prgMax_in, class Room & room);
	void Update(int prgCrnt_in, int prgMax_in);
	static void DrawManual(float prgCrnt_in, float prgMax_in, int orientation, bool isReverse, Vec2 vec_in, Color c_in, Color bg, int width_in, int height_in, Graphics &gfx);
};