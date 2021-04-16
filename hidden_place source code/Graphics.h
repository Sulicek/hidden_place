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
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Vec2.h"
#include "Surface.h"
#include <map>
#include "SpriteLoader.h"

class Graphics
{
public:
	enum DrawSpecials {
		rotation, multiplier, jagged, squary, shine, radiusCap
	};
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	SpriteLoader* sl;
	void PutLight(int x, int y, float light, Color c);
	void Illuminate(float illuminationLevel = 1.0f);
	void ApplyLight();
	void ClearScreen(Color c, float opacity = 1);
	void PutPixel(int x, int y, Color c);
	void PutPixel(int x, int y,float alpha, Color c);
	void DrawRect(int x, int y, int width, int height, Color c, float opacity = 1);
	void DrawRectBlend(int x, int y, int width, int height, Color center, Color edge, float opacity = 1);
	void DrawCircle(Vec2 vec_in, int r, Color c);
	//draw a light with intensity, max opacity, color and specials (squary for bigger pixels and jagged for jagged transition
	void DrawLight(Vec2 vec_in, float intensity, float max, Color c, std::map<DrawSpecials, float> specials = {});
	void DrawFlame(Vec2 vec_in, float intensity, float max, Color c, std::map<DrawSpecials, float> specials = {});
	//default preset
	void DrawLight(Vec2 vec_in, Color c);
	//drawing the sprite as is
	void DrawSprite(Vec2 vec_in, std::string str);
	//drawing the sprite while stopping at a certain width and height
	void DrawSprite(Vec2 vec_in, int width_in, int height_in, std::string str);
	//drawing a specific protion of a sprite, based on it's sprite pos coordinates and width and height
	void DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, struct SpritePos sPos, std::string str);
	//drawing a specific protion of a sprite, based on it's sprite pos coordinates and width and height and with a specific chroma color
	void DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, Color chroma, struct SpritePos sPos, std::string str, float opacity,
		std::map<DrawSpecials, float> specials = {
			{Graphics::DrawSpecials::rotation, 0},
			{Graphics::DrawSpecials::multiplier, 1}
		}, Color* overrideC = nullptr);
	//drawing a specific protion of a sprite, based on it's sprite pos coordinates and width and height and with a specific chroma color and with color override
	void DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, Color chroma, SpritePos sPos, 
		Color overrideC, std::string str);
	//drawing a specific protion of a sprite, based on it's sprite pos coordinates and width and height and with a specific chroma color and opacity and with color override
	void DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, Color chroma, SpritePos sPos, 
		Color overrideC, float opacity, std::string,
		std::map<DrawSpecials, float> specials = {
			{Graphics::DrawSpecials::rotation, 0},
			{Graphics::DrawSpecials::multiplier, 1}
		});

	void DrawCone(Vec2 vec_in, Vec2 p_in, int angle_in, Color c);
	void DrawConeOutline(Vec2 vec_in, Vec2 p_in, int angle_in, int scatter_in, Color c);
	void DrawConeEdge(Vec2 vec_in, Vec2 p_in, int angle_in, int scatter_in, int r_in, Color c);
	void DrawVec(Vec2 p1, Vec2 p2, Color c);
	~Graphics();
public:
	static constexpr int ScreenWidth = 820;
	static constexpr int ScreenHeight = 600;
	static class Color HueStep(Color c_in, double hueStep, double maxHueStep);
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
	Color*                                              lightColor = nullptr;
	float*												lightLevel = nullptr;

};