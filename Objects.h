#pragma once
#include "framework.h"

bool		InitTriangle(ID3D11Device* device, HWND &hWnd, Object* object);
bool		InitCube(ID3D11Device* device, HWND &hWnd, Object* object);
bool		InitPlane(ID3D11Device* device, HWND &hWnd, Object* object);
bool		InitQuad(ID3D11Device* device, HWND &hWnd, Object* object);
bool		InitQuad2D(ID3D11Device* device, HWND &hWnd, Object* object, int width, int height, int posX, int posY);
bool		InitQuad2D_Dynamic(ID3D11Device* device, HWND &hWnd, Object* object);

bool		InitObject(ID3D11Device* device, HWND &hWnd, bool isDynamic, wchar_t* objectName, Object* object, VertexData*& vertexArr, unsigned long*& indexArr);

void		FreeObjectData(Object* object);
