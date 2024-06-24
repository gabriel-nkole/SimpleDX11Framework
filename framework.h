// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// MINE
// Includes
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#pragma comment(lib, "dxgi.lib")

#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

#include <directxmath.h>
using namespace DirectX;

#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#define CUBE_FACES 6


//Structs
struct Direct3D {
	int videoCardMemory;
	char videoCardDescription[128];

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	ID3D11Texture2D* depthBuffer;
	ID3D11DepthStencilState* depthEnabledStencilState;
	ID3D11DepthStencilState* depthDisabledStencilState;
	ID3D11DepthStencilState* skyboxDepthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11RasterizerState* rasterState;
	ID3D11BlendState* alphaBlendEnabledState;
	ID3D11BlendState* alphaBlendDisabledState;
};

struct DirectInput {
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char keyboardState[256];
	DIMOUSESTATE mouseState;
};


struct VertexData {
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

struct Object {
	int vertexCount, indexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
};

struct Texture2D {
	ID3D11Resource* textureResource;
	ID3D11ShaderResourceView* textureView;
};

struct MatrixBufferData {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct Shader {
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
};