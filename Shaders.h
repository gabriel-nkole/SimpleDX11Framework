#pragma once
#include "framework.h"

bool		CreateMatrixBuffer(ID3D11Device* device, ID3D11Buffer** matrxiBuffer);
void		CreateInputLayoutDesc(D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc);


bool		InitShader(ID3D11Device* device, HWND &hWnd, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, int inputElements, 
					   Shader* shader, wchar_t* shaderFilename, char* vertexShaderName, char* pixelShaderName);


bool		DrawColorObject(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* colorShader, Object* object,
							ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P);

bool		DrawTextureObject(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* textureShader, Object* object,
							  ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P, ID3D11ShaderResourceView** textureView);

bool		DrawSkybox(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* skyboxShader, Object* cube,
					   ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P,
					   ID3D11ShaderResourceView** skyboxTextureView, ID3D11DepthStencilState* skyboxDepthStencilState, ID3D11DepthStencilState* depthEnabledStencilState); //requires cube

bool		DrawFloorObject(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* floorShader, Object* plane,
						    ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P);

bool		DrawObject2D(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* textureShader, Object* quad2DDynamic,
					     ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P, ID3D11ShaderResourceView** textureView,
						 bool update = 0, int width = 0, int height = 0, int posX = 0, int posY = 0);  //requires Quad 


void		FreeShaderData(Shader* shader);