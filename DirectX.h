#pragma once
#include "framework.h"

// Direct3D
// Main Functions
bool		InitDirect3D(Direct3D* d3d, HWND &hWnd, bool FULL_SCREEN, int &screenWidth, int &screenHeight);
void		FreeDirect3DData(Direct3D* d3d);


// Helper Functions
void		StartFrame(Direct3D* d3d, float renderTargetClearColor[4]);
void		EndFrame(Direct3D* d3d);

void		EnableAlphaBlending(Direct3D* d3d);
void		DisableAlphaBlending(Direct3D* d3d);
void		EnableDepthBuffer(Direct3D* d3d);
void		DisableDepthBuffer(Direct3D* d3d);



// DirectInput
// Main Functions
bool		InitDirectInput(DirectInput* dInput, HINSTANCE &hInst, HWND &hWnd);
void		FreeDirectInputData(DirectInput* dInput);

// Helper Functions
bool		ReadKeyboard(DirectInput* dInput);
bool		ReadMouse(DirectInput* dInput);