#pragma once
#include "framework.h"

bool		LoadTexture2D(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND &hWnd, wchar_t* filename, Texture2D* texture);
bool		LoadTextureSkybox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND &hWnd, wchar_t filenames[CUBE_FACES][128],
							  ID3D11Resource** skyboxTextureArr, ID3D11ShaderResourceView** skyboxTextureViewArr,
							  ID3D11Texture2D** skyboxTexture, ID3D11ShaderResourceView** skyboxTextureView);

void		FreeTexture2DData(Texture2D*);