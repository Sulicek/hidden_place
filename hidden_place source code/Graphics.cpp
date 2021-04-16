/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>
#include "Surface.h"
#include "Animation.h"
#include <random>
#include <cmath>
#include <algorithm>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )
#define M_PI 3.14159265358979323846 
using Microsoft::WRL::ComPtr;
using namespace std;

Graphics::Graphics(HWNDKey& key)
{
	assert(key.hWnd != nullptr);

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

	// create device and front/back buffers
	if (FAILED(hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating device and swap chain");
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if (FAILED(hr = pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBuffer)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Getting back buffer");
	}

	// create a view on backbuffer that we can render to
	if (FAILED(hr = pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating render target view on backbuffer");
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float(Graphics::ScreenWidth);
	vp.Height = float(Graphics::ScreenHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports(1, &vp);


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if (FAILED(hr = pDevice->CreateTexture2D(&sysTexDesc, nullptr, &pSysBufferTexture)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating sysbuffer texture");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if (FAILED(hr = pDevice->CreateShaderResourceView(pSysBufferTexture.Get(),
		&srvDesc, &pSysBufferTextureView)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating view on sysBuffer texture");
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if (FAILED(hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof(FramebufferShaders::FramebufferPSBytecode),
		nullptr,
		&pPixelShader)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating pixel shader");
	}


	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if (FAILED(hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof(FramebufferShaders::FramebufferVSBytecode),
		nullptr,
		&pVertexShader)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating vertex shader");
	}


	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FSQVertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if (FAILED(hr = pDevice->CreateBuffer(&bd, &initData, &pVertexBuffer)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating vertex buffer");
	}


	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if (FAILED(hr = pDevice->CreateInputLayout(ied, 2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof(FramebufferShaders::FramebufferVSBytecode),
		&pInputLayout)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating input layout");
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(hr = pDevice->CreateSamplerState(&sampDesc, &pSamplerState)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Creating sampler state");
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>(
		_aligned_malloc(sizeof(Color) * Graphics::ScreenWidth * Graphics::ScreenHeight, 16u));

	lightColor = reinterpret_cast<Color*>(
		_aligned_malloc(sizeof(Color) * Graphics::ScreenWidth * Graphics::ScreenHeight, 16u));
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if (pSysBuffer)
	{
		_aligned_free(pSysBuffer);
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if (pImmediateContext) pImmediateContext->ClearState();
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if (FAILED(hr = pImmediateContext->Map(pSysBufferTexture.Get(), 0u,
		D3D11_MAP_WRITE_DISCARD, 0u, &mappedSysBufferTexture)))
	{
		throw CHILI_GFX_EXCEPTION(hr, L"Mapping sysbuffer");
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData);
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof(Color);
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof(Color);
	// perform the copy line-by-line
	for (size_t y = 0u; y < Graphics::ScreenHeight; y++)
	{
		memcpy(&pDst[y * dstPitch], &pSysBuffer[y * srcPitch], rowBytes);
	}
	// release the adapter memory
	pImmediateContext->Unmap(pSysBufferTexture.Get(), 0u);

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout(pInputLayout.Get());
	pImmediateContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	pImmediateContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const UINT stride = sizeof(FSQVertex);
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	pImmediateContext->PSSetShaderResources(0u, 1u, pSysBufferTextureView.GetAddressOf());
	pImmediateContext->PSSetSamplers(0u, 1u, pSamplerState.GetAddressOf());
	pImmediateContext->Draw(6u, 0u);

	// flip back/front buffers
	if (FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw CHILI_GFX_EXCEPTION(pDevice->GetDeviceRemovedReason(), L"Presenting back buffer [device removed]");
		}
		else
		{
			throw CHILI_GFX_EXCEPTION(hr, L"Presenting back buffer");
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset(pSysBuffer, 0u, sizeof(Color) * Graphics::ScreenHeight * Graphics::ScreenWidth);
	memset(lightColor, 0u, sizeof(Color) * Graphics::ScreenHeight * Graphics::ScreenWidth);
}

void Graphics::PutPixel(int x, int y, float alpha, Color c)
{
	assert(x >= 0);
	assert(x < int(Graphics::ScreenWidth));
	assert(y >= 0);
	assert(y < int(Graphics::ScreenHeight));
	pSysBuffer[Graphics::ScreenWidth * y + x] =
		(pSysBuffer[Graphics::ScreenWidth * y + x] * (float(1) - alpha) + c * alpha);
}

void Graphics::PutPixel(int x, int y, Color c)
{
	PutPixel(x, y, 1, c);
}

float Jag(float input, int jag_in) {
	int jag = 10 * jag_in;
	input *= jag;
	input = std::floor(input);
	input /= (float)jag;
	return input;
}

float JagSimple(float input_in, float jag_in) {
	if (jag_in == 0) {
		return input_in;
	}
	float factor = 1;
	float jag = jag_in;
	float input = input_in;
	while (jag < 1) {
		jag *= 10.0;
		factor *= 10.0;
		input *= 10.0;
	}
	float output = (int)input % (int)jag;
	return input_in - (output / factor);
}

Vec2 Jag(Vec2 &input, int jag_in) {
	return { input.x - (int)input.x % jag_in, input.y - (int)input.y % jag_in };
}

void Graphics::PutLight(int x, int y, float light, Color c)
{
	Color source = lightColor[Graphics::ScreenWidth * y + x];
	c = c * light;
	lightColor[Graphics::ScreenWidth * y + x] = Color(
		max(source.GetR(), c.GetR()),
		max(source.GetG(), c.GetG()),
		max(source.GetB(), c.GetB()));
}

void Graphics::Illuminate(float illuminationLevel)
{
	for (int i = 0; i < Graphics::ScreenWidth*Graphics::ScreenHeight; i++) {
		Color source = lightColor[i];
		Color c = Color(255, 255, 255) * illuminationLevel;
		lightColor[i] = c;
	}
}

void Graphics::ApplyLight()
{
	for (int i = 0; i < Graphics::ScreenWidth*Graphics::ScreenHeight; i++) {
		//applying light color
		Color c = pSysBuffer[i];
		c = c * lightColor[i];
		pSysBuffer[i] = c;//* lightLevel[i];
	}
}

void Graphics::ClearScreen(Color c, float opacity) {
	for (int i = 0; i < Graphics::ScreenWidth*Graphics::ScreenHeight; i++) {
		pSysBuffer[i] = pSysBuffer[i] * (float(1) - opacity) + c * opacity;
	}
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception(HRESULT hr, const std::wstring& note, const wchar_t* file, unsigned int line)
	:
	ChiliException(file, line, note),
	hr(hr)
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring(L"Error: ") + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring(L"Description: ") + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring(L"Note: ") + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring(L"Location: ") + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString(hr);
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t, 512> wideDescription;
	DXGetErrorDescription(hr, wideDescription.data(), wideDescription.size());
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}

void Graphics::DrawRect(int x, int y, int width, int height, Color c, float opacity) {
	//adjusting the width and the height to not go over screen
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	if (x + width > xBound) {
		width -= x + width - xBound;
	}
	if (y + height > yBound) {
		height -= y + height - yBound;
	}
	if (x < 0) {
		width -= -x;
		x = 0;
	}
	if (y < 0) {
		height -= -y;
		y = 0;
	}

	for (int i = 0; i < width; i++) {
		for (int o = 0; o < height; o++) {
			Graphics::PutPixel(x + i, y + o, opacity, c);
		}
	}
}

void Graphics::DrawRectBlend(int x, int y, int width, int height, Color center, Color edge, float opacity)
{
	//adjusting the width and the height to not go over screen
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	if (x + width > xBound) {
		width -= x + width - xBound;
	}
	if (y + height > yBound) {
		height -= y + height - yBound;
	}
	if (x < 0) {
		width -= -x;
		x = 0;
	}
	if (y < 0) {
		height -= -y;
		y = 0;
	}
	int maxPossibleDistance = min(width, height);
	for (int i = 0; i < width; i++) {
		for (int o = 0; o < height; o++) {
			int distanceX = min(i, width - i);
			int distanceY = min(o, height - o);
			int distance = min(distanceY, distanceX);

			//distance = JagSimple(distance, 2 * maxPossibleDistance/10);
			float ratio = float(distance) / (height / 2);
			float pattern = 0.4;
			
			ratio = JagSimple(ratio, pattern);
			Color blendedC = edge * (1.0f - ratio) + center * ratio;
			Graphics::PutPixel(x + i, y + o, opacity, blendedC);
		}
	}
}

void Graphics::DrawSprite(Vec2 vec_in, std::string str)
{
	Surface& s = *((*(sl))[str]);
	DrawSprite(vec_in, s.GetWidth(), s.GetHeight(), str);
}

void Graphics::DrawSprite(Vec2 vec_in, int width_in, int height_in, std::string str) {
	DrawSpritePortion(vec_in, width_in, height_in, { 0, 0 }, str);
}

void Graphics::DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, SpritePos sPos, std::string str) {
	DrawSpritePortion(vec_in, width_in, height_in, Colors::Magenta, sPos, str, 1);
}

Vec2 RotateRight(Vec2 source, int width, int height, int count) {
	switch (count) {
	case 1:
		return Vec2(height - source.y - 1, source.x);
		break;
	case 2:
		return Vec2(width - source.x - 1, height - source.y - 1);
		break;
	case 3:
		return Vec2(source.y , width-source.x - 1);
		break;
	}
}

void Graphics::DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, Color chroma, SpritePos sPos,
	std::string str, float opacity,
	std::map<DrawSpecials, float> specials, Color *overrideC)
{
	Surface& s = *((*(sl))[str]);
	int multiplier = specials[DrawSpecials::multiplier];
	if (multiplier == 0) {
		multiplier = 1;
	}
	int rotation = specials[DrawSpecials::rotation];
	rotation = rotation % 4;
	int width = width_in;
	int height = height_in;
	//set the boundaries
	const int xBound = ScreenWidth-1;
	const int yBound = ScreenHeight-1;
	int xOffset = 0;
	int yOffset = 0;
	int x = vec_in.x;
	int y = vec_in.y;
	//adjust the width & height & both offsets
	if (x + width * multiplier > xBound) {
		width -= ceil(float(x + width * multiplier - xBound) / float(multiplier));
	}
	if (y + height * multiplier > yBound) {
		height -= ceil(float(y + height * multiplier - yBound) / float(multiplier));
	}
	if (x < 0) {
		width -= -x;
		xOffset = -x;
	}
	if (y < 0) {
		height -= -y;
		yOffset = -y;
	}
	if (overrideC != nullptr) { //i know this looks really dumb but it is good for performance apparently???
		for (int sy = 0; sy < height; sy++)
		{
			for (int sx = 0; sx < width; sx++)
			{
				Color sourcePixel;
				if (rotation == 0) {
					sourcePixel = s.GetPixel(sx + xOffset + sPos.x * width_in, sy + yOffset + sPos.y * height_in);
				}
				else {
					sourcePixel =
						s.GetPixel(RotateRight(Vec2(sx + xOffset, sy + yOffset), width_in, height_in, 4 - rotation)
							+ Vec2(sPos.x*width_in, sPos.y*height_in));
				}
				if (sourcePixel.GetR() != chroma.GetR() ||
					sourcePixel.GetG() != chroma.GetG() ||
					sourcePixel.GetB() != chroma.GetB()
					) {
					//basic multipliers
					for (int mx = 0; mx < multiplier; mx++)
					{
						for (int my = 0; my < multiplier; my++)
						{
							PutPixel(x + sx * multiplier + xOffset + mx, y + sy * multiplier + yOffset + my, opacity, *overrideC);
						}
					}
				}
			}
		}
	}
	else {
		for (int sy = 0; sy < height; sy++)
		{
			for (int sx = 0; sx < width; sx++)
			{
				Color sourcePixel;
				if (rotation == 0) {
					sourcePixel = s.GetPixel(sx + xOffset + sPos.x * width_in, sy + yOffset + sPos.y * height_in);
				}
				else {
					sourcePixel =
						s.GetPixel(RotateRight(Vec2(sx + xOffset, sy + yOffset), width_in, height_in, 4 - rotation)
							+ Vec2(sPos.x*width_in, sPos.y*height_in));
				}
				if (sourcePixel.GetR() != chroma.GetR() ||
					sourcePixel.GetG() != chroma.GetG() ||
					sourcePixel.GetB() != chroma.GetB()
					) {
					//basic multipliers
					for (int mx = 0; mx < multiplier; mx++)
					{
						for (int my = 0; my < multiplier; my++)
						{

							PutPixel(x + sx * multiplier + xOffset + mx, y + sy * multiplier + yOffset + my, opacity,
								sourcePixel);
						}
					}
				}
			}
		}
	}
}

void Graphics::DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, Color chroma, SpritePos sPos, Color overrideC, 
	std::string str) {
	DrawSpritePortion(vec_in, width_in, height_in, chroma, sPos, overrideC, 1, str);
}


void Graphics::DrawSpritePortion(Vec2 vec_in, int width_in, int height_in, Color chroma, SpritePos sPos,
	Color overrideC, float opacity, std::string str, std::map<DrawSpecials, float> specials)
{
	Surface& s = *((*(sl))[str]);
	int multiplier = (specials.count(DrawSpecials::multiplier) > 0 ? specials[DrawSpecials::multiplier] : 1);
	int rotation = (specials.count(DrawSpecials::rotation) > 0 ? specials[DrawSpecials::rotation] : 0);
	int width = width_in;
	int height = height_in;
	//set the boundaries
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	int xOffset = 0;
	int yOffset = 0;
	int x = vec_in.x;
	int y = vec_in.y;
	//adjust the width & height & both offsets
	if (x + width * multiplier > xBound) {
		width -= ceil(float(x + width * multiplier - xBound) / float(multiplier));
	}
	if (y + height * multiplier > yBound) {
		height -= ceil(float(y + height * multiplier - yBound) / float(multiplier));
	}
	if (x < 0) {
		width -= -x;
		xOffset = -x;
	}
	if (y < 0) {
		height -= -y;
		yOffset = -y;
	}
	for (int sy = 0; sy < height; sy++)
	{
		for (int sx = 0; sx < width; sx++)
		{
			Color sourcePixel;
			if (rotation == 0) {
				sourcePixel = s.GetPixel(sx + xOffset + sPos.x * width_in, sy + yOffset + sPos.y * height_in);
			}
			else {
				sourcePixel =
					s.GetPixel( RotateRight(Vec2(sx + xOffset, sy + yOffset),width_in, height_in, 4-rotation) 
						+ Vec2(sPos.x*width_in, sPos.y*height_in));
			}
			if (sourcePixel.GetR() != chroma.GetR() ||
				sourcePixel.GetG() != chroma.GetG() ||
				sourcePixel.GetB() != chroma.GetB()
				) {
				//basic multipliers
				for (int mx = 0; mx < multiplier; mx++)
				{
					for (int my = 0; my < multiplier; my++)
					{
						PutPixel(x + sx * multiplier + xOffset + mx, y + sy * multiplier + yOffset + my, opacity, overrideC);
					}
				}
			}
		}
	}
}



void Graphics::DrawCircle(Vec2 vec_in, int r, Color c) {
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	for (int ix = 0; ix < r * 2 + 1; ix++) {
		for (int iy = 0; iy < r * 2 + 1; iy++) {
			if (int(abs(Vec2::GetDeltaVec(Vec2(ix, iy), Vec2(r, r)).GetLenght())) == r) {
				if (vec_in.x - r + ix > 0 && vec_in.x - r + ix < xBound &&
					vec_in.y - r + iy > 0 && vec_in.y - r + iy < yBound) {
					Graphics::PutPixel(vec_in.x - r + ix, vec_in.y - r + iy, c);
				}
			}
		}
	}
}



Color LightenColor(Color c_in) {
	int r = c_in.GetR();
	int g = c_in.GetG();
	int	b = c_in.GetB();
	vector<int*> values = { &r, &g, &b };
	int *maxEl = &r;
	for (auto &item : values) {
		if (*maxEl < *item) {
			maxEl = item;
		}
	}
	//auto maxEl = std::max_element(values.begin(), values.end());
	*maxEl = 255;
	return Color(r, g, b);
}

float sqrt1(const float &n)
{
	static union { int i; float f; } u;
	u.i = 0x5F375A86 - (*(int*)&n >> 1);
	return (int(3) - n * u.f * u.f) * n * u.f * 0.5f;
}

void Graphics::DrawLight(Vec2 vec_in, Color c)
{
	DrawLight(vec_in,
		100, 0.3, c,
		{
		{Graphics::DrawSpecials::jagged, 3},
		{Graphics::DrawSpecials::squary, 5}
		});
}

bool ValidatePixelPlacement(Vec2 placement, int xBound, int yBound) {
	return placement.x > 0 && placement.x < xBound &&
		placement.y > 0 && placement.y < yBound;
}

void Graphics::DrawLight(Vec2 vec_in, float intensity, float max, Color c, std::map<DrawSpecials, float> specials)
{
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;

	float minLight = 0.05f;
	int r = sqrt1(intensity / minLight);
	int minR = 2;
	if (r <= 2) {
		return;
	}
	for (int ix = 0; ix < r + 1; ix++) {
		for (int iy = 0; iy < r + 1; iy++) {
			Vec2 distanceVector = Vec2::GetDeltaVec(Vec2(ix, iy), Vec2(r, r));
			float distanceFromCenter = abs(distanceVector.GetLenghtSqrt());
			if (specials.count(DrawSpecials::squary) > 0) {
				distanceFromCenter = abs(Vec2::GetDeltaVec(Jag(Vec2(ix, iy), (int)specials[DrawSpecials::squary]), Vec2(r, r)).GetLenghtSqrt());
			}
			if (int(distanceFromCenter) <= r * r) {

				float currentOpacity = std::min(1 / distanceFromCenter * intensity, max);
				//jaggedness
				if (specials.count(DrawSpecials::jagged) > 0) {
					//how many digits do we save
					currentOpacity = Jag(currentOpacity, (int)specials[DrawSpecials::jagged]);
				}
				for (int i = 0; i < 2; i++) {
					for (int o = 0; o < 2; o++) {
						Vec2 offset = distanceVector;
						Vec2 pixelPlacement = Vec2(
							vec_in.x - r + ix - distanceVector.x * i * 2 + i * 1,
							vec_in.y - r + iy - distanceVector.y * o * 2 + o * 1);
						if (ValidatePixelPlacement(pixelPlacement, xBound, yBound)) {
							Graphics::PutPixel(pixelPlacement.x, pixelPlacement.y, currentOpacity, c);
							//Graphics::PutLight(pixelPlacement.x, pixelPlacement.y, currentOpacity, c);
						}
					}
				}

			}
		}
	}
}

Color Graphics::HueStep(Color c_in, double hueStep, double maxHueStep) {
	Color::hsv hsvColor = Color::rgb2hsv(
		{ (float)c_in.GetR() / 255.0, (float)c_in.GetG() / 255.0, (float)c_in.GetB() / 255.0 });
	hsvColor.h -= min(hueStep, maxHueStep);
	return Color::rgbToColor(Color::hsv2rgb(hsvColor));
}



void Graphics::DrawFlame(Vec2 vec_in, float intensity, float max, Color c, std::map<DrawSpecials, float> specials)
{
	vec_in = Vec2((int)vec_in.x, (int)vec_in.y);
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;

	//minimum opacity that is going to be drawn
	float minLight = 0.05f;
	int r = sqrt1(intensity / minLight);
	int drawRadius = r;
	if(specials.count(DrawSpecials::radiusCap) > 0){
		if ((int)specials[DrawSpecials::radiusCap] > 0) {
			drawRadius = std::min(r, (int)specials[DrawSpecials::radiusCap]);
		}
	}

	//setting the minimum radius that we're going to draw
	int minR = 2;
	if (r <= 2) {
		return;
	}

	int squaryResult = -1;
	if (specials.count(DrawSpecials::squary) > 0) {
		squaryResult = (int)specials[DrawSpecials::squary];
	}

	int jaggedResult = -1;
	if (specials.count(DrawSpecials::jagged) > 0) {
		jaggedResult = (int)specials[DrawSpecials::jagged];
	}

	int radDrawDiff = r - drawRadius;
	for (int ix = radDrawDiff; ix < r + 1; ix++) {
		for (int iy = radDrawDiff; iy < r + 1; iy++) {

			Vec2 distanceVector = Vec2::GetDeltaVec(Vec2(ix, iy), Vec2(r, r));
			double distanceFromCenter = abs(distanceVector.GetLenghtSqrt());
			//jagging the distance from center
			if (squaryResult > 0) {
				distanceFromCenter = abs(Vec2::GetDeltaVec(Jag(Vec2(ix, iy), squaryResult), Vec2(r, r)).GetLenghtSqrt());
			}

			if (int(distanceFromCenter) <= drawRadius * drawRadius) {
				//screen bound checks

				float currentOpacity = std::min(1 / (float)distanceFromCenter * intensity, max);
				//jagging the opacity
				if (jaggedResult > 0) {
					//how many digits do we save
					currentOpacity = Jag(currentOpacity, jaggedResult);
				}
				//converting the color
				Color convertedColor = c;
				convertedColor = HueStep(c, distanceFromCenter / double(r)*0.12 * 50, 60);
				for (int i = 0; i < 2; i++) {
					for (int o = 0; o < 2; o++) {
						Vec2 offset = distanceVector;
						Vec2 pixelPlacement = Vec2(
							vec_in.x - r + ix - distanceVector.x * i * 2 + i * 1,
							vec_in.y - r + iy - distanceVector.y * o * 2 + o * 1);
						//pixelPlacement += Vec2(r - drawRadius, drawRadius);
						if (ValidatePixelPlacement(pixelPlacement, xBound, yBound)) {
							Graphics::PutPixel(pixelPlacement.x, pixelPlacement.y, currentOpacity, convertedColor);
							if (specials.count(DrawSpecials::shine) > 0) {
								if (specials[DrawSpecials::shine] == 1) {
									Graphics::PutLight(pixelPlacement.x, pixelPlacement.y, 1, convertedColor);
								}
							}
						}
					}
				}
			}
		}

	}
}


void Graphics::DrawCone(Vec2 vec_in, Vec2 p_in, int angle_in, Color c) {
	//we put the angle into radians for the math and cuz inputing it in radians sucks
	float angle = float(angle_in) * M_PI / float(180);
	//i did it!
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	//so ehm. this is kinda hard to explain. Basically (ironic use of a word, right?) this angle is 
	//how much is the 'top' point of the cone offset of a line perpendicular to
	//the x axis. Why the x axis? because we count the angles from it. that's really it.
	float deltaAngle = (float(2) * M_PI) - Vec2::GetDeltaVec(p_in, vec_in).GetAngle();
	//just setting the radius based on the inputs
	int r = Vec2::GetDeltaVec(p_in, vec_in).GetLenght();
	//the other point
	Vec2 p2 = Vec2::GetVecByAngle(Vec2::GetDeltaVec(p_in, vec_in).GetAngle() + angle).GetNormalized() * r + vec_in;
	//the "spike"  point
	Vec2 spike = p_in + Vec2::GetDeltaVec(p2, vec_in);
	//finding the maxs and the mins
	int maxX = max(max(p_in.x, p2.x), max(spike.x, vec_in.x));
	int maxY = max(max(p_in.y, p2.y), max(spike.y, vec_in.y));
	int minX = min(min(p_in.x, p2.x), min(spike.x, vec_in.x));
	int minY = min(min(p_in.y, p2.y), min(spike.y, vec_in.y));
	if (angle_in > 90) {
		maxX = vec_in.x + r;
		maxY = vec_in.y + r;
		minX = vec_in.x - r;
		minY = vec_in.y - r;
	}
	//checking every single possible pixel to draw
	for (int ix = 0; ix < maxX - minX + 1; ix++) {
		for (int iy = 0; iy < maxY - minY + 1; iy++) {
			if (int(abs(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetLenght())) <= r &&
				//checking wether the delta vector of the point and the radius has the appropriate angle. 
				//This one was a bitch to figure out
				Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetAngle() + deltaAngle) <= angle &&
				Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetAngle()) + deltaAngle > 0
				) {
				//not drawing off the screen
				if (minX + ix > 0 && minX + ix < xBound &&
					minY + iy > 0 && minY + iy < yBound) {
					Graphics::PutPixel(minX + ix, minY + iy, c);
				}
			}
		}
	}
	/*
	testing the edges
	PutPixel(p_in.x, p_in.y, Colors::Magenta);
	PutPixel(p2.x, p2.y, Colors::Magenta);
	PutPixel(spike.x, spike.y, Colors::Magenta);
	PutPixel(vec_in.x, vec_in.y, Colors::Magenta);*/
	//testing the basic bitch hit box
	/*
	DrawVec(Vec2(minX, minY), Vec2(maxX, minY), Colors::Magenta);
	DrawVec(Vec2(maxX, minY), Vec2(maxX, maxY), Colors::Magenta);
	DrawVec(Vec2(maxX, maxY), Vec2(minX, maxY), Colors::Magenta);
	DrawVec(Vec2(minX, maxY), Vec2(minX, minY), Colors::Magenta);
	*/
}

void Graphics::DrawConeOutline(Vec2 vec_in, Vec2 p_in, int angle_in, int scatter_in, Color c) {
	//we put the angle into radians for the math cuz inputing it in radians sucks
	float angle = float(angle_in) * M_PI / float(180);
	//i did it!
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	//so ehm. this is kinda hard to explain. Basically (ironic use of a word, right?) this angle is 
	//how much is the 'top' point of the cone offset of a line perpendicular to
	//the x axis. Why the x axis? because we count the angles from it. that's really it.
	float deltaAngle = (float(2) * M_PI) - Vec2::GetDeltaVec(p_in, vec_in).GetAngle();
	//just setting the radius based on the inputs
	int r = Vec2::GetDeltaVec(p_in, vec_in).GetLenght();
	//the other point
	Vec2 p2 = Vec2::GetVecByAngle(Vec2::GetDeltaVec(p_in, vec_in).GetAngle() + angle).GetNormalized() * r + vec_in;
	//the "spike"  point
	Vec2 spike = p_in + Vec2::GetDeltaVec(p2, vec_in);
	//finding the maxs and the mins
	int maxX = max(max(p_in.x, p2.x), max(spike.x, vec_in.x));
	int maxY = max(max(p_in.y, p2.y), max(spike.y, vec_in.y));
	int minX = min(min(p_in.x, p2.x), min(spike.x, vec_in.x));
	int minY = min(min(p_in.y, p2.y), min(spike.y, vec_in.y));
	if (angle_in > 90) {
		maxX = vec_in.x + r;
		maxY = vec_in.y + r;
		minX = vec_in.x - r;
		minY = vec_in.y - r;
	}
	//checking every single possible pixel to draw
	for (int ix = 0; ix < maxX - minX + 1; ix++) {
		for (int iy = 0; iy < maxY - minY + 1; iy++) {
			if (int(abs(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetLenght())) <= r &&
				//checking wether the delta vector of the point and the radius has the appropriate angle. 
				//This one was a bitch to figure out
				Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetAngle() + deltaAngle) <= angle &&
				Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetAngle() + deltaAngle) > 0
				) {
				//not drawing off the screen
				if (minX + ix > 0 && minX + ix < xBound &&
					minY + iy > 0 && minY + iy < yBound) {
					if (rand() % scatter_in == 0) {
						Graphics::PutPixel(minX + ix, minY + iy, c);
					}
				}
			}
		}
	}
}

void Graphics::DrawConeEdge(Vec2 vec_in, Vec2 p_in, int angle_in, int scatter_in, int r_in, Color c) {
	//we put the angle into radians for the math and cuz inputing it in radians sucks 
	float angle = float(angle_in) * M_PI / float(180);
	//i did it!
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	//so ehm. this is kinda hard to explain. Basically (ironic use of a word, right?) this angle is 
	//how much is the 'top' point of the cone offset of a line perpendicular to
	//the x axis. Why the x axis? because we count the angles from it. that's really it.
	float deltaAngle = (float(2) * M_PI) - Vec2::GetDeltaVec(p_in, vec_in).GetAngle();
	//just setting the radius based on the inputs
	int r = r_in;
	//the other point
	Vec2 p2 = Vec2::GetVecByAngle(Vec2::GetDeltaVec(p_in, vec_in).GetAngle() + angle).GetNormalized() * r + vec_in;
	//the "spike"  point
	Vec2 spike = p_in + Vec2::GetDeltaVec(p2, vec_in);
	//finding the maxs and the mins
	int maxX = max(max(p_in.x, p2.x), max(spike.x, vec_in.x));
	int maxY = max(max(p_in.y, p2.y), max(spike.y, vec_in.y));
	int minX = min(min(p_in.x, p2.x), min(spike.x, vec_in.x));
	int minY = min(min(p_in.y, p2.y), min(spike.y, vec_in.y));
	if (angle_in > 90) {
		maxX = vec_in.x + r;
		maxY = vec_in.y + r;
		minX = vec_in.x - r;
		minY = vec_in.y - r;
	}
	//checking every single possible pixel to draw
	for (int ix = 0; ix < maxX - minX + 1; ix++) {
		for (int iy = 0; iy < maxY - minY + 1; iy++) {
			if (int(abs(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetLenght())) == r &&
				//checking wether the delta vector of the point and the radius has the appropriate angle. 
				//This one was a bitch to figure out
				Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetAngle() + deltaAngle) <= angle &&
				Vec2::FixAngle(Vec2::GetDeltaVec(Vec2(minX + ix, minY + iy), vec_in).GetAngle() + deltaAngle) > 0
				) {
				//not drawing off the screen
				if (minX + ix > 0 && minX + ix < xBound &&
					minY + iy > 0 && minY + iy < yBound) {
					Graphics::PutPixel(minX + ix, minY + iy, c);

				}
			}
		}
	}
}

void Graphics::DrawVec(Vec2 p1, Vec2 p2, Color c) {
	const int xBound = ScreenWidth;
	const int yBound = ScreenHeight;
	Vec2 dir = Vec2::GetDeltaVecNorm(p2, p1);
	for (int i = 0; i < Vec2::GetDeltaVec(p1, p2).GetLenght(); i++) {
		if (p1.x + dir.x*i > 0 && p1.x + dir.x*i < xBound &&
			p1.y + dir.y*i > 0 && p1.y + dir.y*i < yBound) {
			PutPixel(p1.x + dir.x*i, p1.y + dir.y*i, c);
		}
	}
}
