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
#include "Font.h"
#include "Animation.h"
#include <cassert>

Font::Font( const std::string& filename,Color chroma )
	:
	surf( filename ),
	// calculate glyph dimensions from bitmap dimensions
	glyphWidth( 8 ),
	glyphHeight( 14),
	chroma( chroma )
{
	// verify that bitmap had valid dimensions
	//assert( glyphWidth * nColumns == surf.GetWidth() );
	//assert( glyphHeight * nRows == surf.GetHeight() );
}

void Font::Draw( const std::string& text,const Vec2& vec_in, 
	Color color, Color bg, int scale, Graphics& gfx )
{
	// curPos is the pos that we are drawing to on the screen
	auto curPos = vec_in;
	for( auto &c : text )
	{
		// on a newline character, reset x position and move down by 1 glyph height
		if( c == '\n' )
		{
			// carriage return
			curPos.x = vec_in.x;
			// line feed
			curPos.y += glyphHeight * scale;
			// we don't want to advance the character position right for a newline
			continue;
		}
		// only draw characters that are on the font sheet
		// start at firstChar + 1 because might as well skip ' ' as well
		else if (c >= firstChar && c <= lastChar)
		{
			// use  so that we can choose the color of the font rendered
			if (bg.dword != Colors::Magenta.dword) {
				gfx.DrawRect(curPos.x, curPos.y, glyphWidth * scale, glyphHeight * scale, bg);
			}
			gfx.DrawSpritePortion(curPos, glyphWidth, glyphHeight, chroma,
				GetGlyphCoordinates(c), surf, 1,
				{ { Graphics::DrawSpecials::multiplier, scale }},
				&color);
			
		}
		// advance screen pos for next character
		curPos.x += glyphWidth * scale;
	}
}

void Font::DrawBg(const std::string& text, const Vec2& vec_in, Graphics& gfx)
{
	Draw(text, vec_in, Colors::Black, Colors::White, 1, gfx);
}

void Font::DrawNoBg(const std::string& text, const Vec2& vec_in, Graphics& gfx)
{
	Draw(text, vec_in, Colors::Black, Colors::Magenta, 1, gfx);
}

int Font::getGlyphHeight()
{
	return glyphHeight;
}

int Font::getGlyphWidth()
{
	return glyphWidth;
}

SpritePos Font::GetGlyphCoordinates(char c) const
{
	assert(c >= firstChar && c <= lastChar);
	// font sheet glyphs start at ' ', calculate index into sheet
	const int glyphIndex = c - ' ';
	// map 1d glyphIndex to 2D coordinates
	const int yGlyph = glyphIndex / nColumns;
	const int xGlyph = glyphIndex % nColumns;
	// convert the sheet grid coords to pixel coords in sheet
	return {xGlyph, yGlyph};
	
}