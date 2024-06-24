// INCLUDES
// Default
#include "framework.h"
#include "main.h"

// Mine
#include "DirectX.h"
#include "Objects.h"
#include "Textures.h"
#include "Shaders.h"

#define Deg2Rad 0.0174532925f



// WINDOW
const bool FULL_SCREEN = true;
HINSTANCE hInst;
HWND hWnd;
#define MAX_LOADSTRING 100
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

int screenWidth;
int screenHeight;
int halfScreenWidth;
int halfScreenHeight;

bool				InitWindow(HINSTANCE, HINSTANCE, LPWSTR, int); //custom fn
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);



// PROJECTION/ORTHOGRAPHICS VARIABLES
float fieldOfView;
float screenAspect;
float screenDepth;
float screenNear;



// DIRECTX
Direct3D* d3d = 0;
DirectInput* dInput = 0;

bool		InitDirectX();



// CAMERA STUFF
XMFLOAT3 cameraPosition;
XMFLOAT3 cameraRotation; //in degrees
void UpdateCamera(float);

// Calculate view matrix
XMMATRIX	GetViewMatrix();



// 3D OBJECTS
bool		InitObjects();

Object triangle;
Object cube;
Object plane;
Object quad;
Object quad2D;        //for static 2D objects
Object quad2DDynamic; //for moving 2D objects



// TEXTURES
bool		LoadTextures();

Texture2D pattern;
Texture2D stone;
Texture2D backgroundSprite;

// Character Sprite
const int spriteWidth = (int)(182.0f*1.5f), spriteHeight = (int)(84.0f*1.5f);
int spritePosX = 0, spritePosY = 0;
int spritePosXPrev = 0, spritePosYPrev = 0;
int spriteVelX = 500, spriteVelY = 500;

unsigned int spriteIndex = 0;
float spriteTimer = 0.0f;
#define SPRITE_FRAMES 8
Texture2D characterSprite[SPRITE_FRAMES];

bool		UpdateCharacterSprite(float);

// Skybox
ID3D11Resource* skyboxTextureArr[CUBE_FACES];
ID3D11ShaderResourceView* skyboxTextureViewArr[CUBE_FACES];
ID3D11Texture2D* skyboxTexture;
ID3D11ShaderResourceView* skyboxTextureView;



// GENERAL SHADER VARIABLES
ID3D11Buffer* matrixBuffer; 

// Input Layout Description
const unsigned int inputElements = 3;
D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[inputElements];

// SHADERS
Shader colorShader;
Shader textureShader;
Shader skyboxShader;
Shader floorShader;

bool		InitShaders();


// CLEANUP
void		ShutdownWindow();
void		FreeAll();

void		FreeShaders();
void		FreeMatrixBuffer();
void		FreeTextures();
void		FreeObjects();
void		FreeDirectX();



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow){
	// WINDOW
	bool result = InitWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	if (!result) {
		return false;
	}
	halfScreenWidth = screenWidth/2;
	halfScreenHeight = screenHeight/2;
	screenAspect = (float)screenWidth / (float)screenHeight;
	fieldOfView = 3.141592654f / 3.5f;
	screenDepth = 1000.0f;
	screenNear = 0.3f;


	// DIRECTX (D3D & DINPUT)
	result = InitDirectX();
	if (!result) {
		FreeAll();
		return false;
	}
	
	// 3D/2D OBJECTS
	result = InitObjects();
	if (!result) {
		FreeAll();
		return false;
	}
	
	// TEXTURES
	result = LoadTextures();
	if (!result) {
		FreeAll();
		return false;
	}
	
	// SHADERS
	result = InitShaders();
	if (!result) {
		FreeAll();
		return false;
	}
	
	
	// TIMING
	INT64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float freqFloat = (float)freq;
	
	INT64 startTicks;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTicks);
	
	
	// CAMERA
	cameraPosition = XMFLOAT3(0.0f, 0.0f, -1.0f);
	cameraRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX V_2D = GetViewMatrix();
	
	cameraPosition = XMFLOAT3(0.0f, 1.8f, -10.0f);
	cameraRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	
	// CALCULATE TRANSFORMATION MATRICES
	// World
	XMMATRIX M_identity = XMMatrixIdentity();
	XMMATRIX M = M_identity;
		
	// View
	XMMATRIX V = GetViewMatrix();
		
	// Perspective Projection
	XMMATRIX P = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
		
	// Orthographic Projection
	XMMATRIX O = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	
	
	// MISCELLANEOUS
	float cubeRotationAngle = 0.0f;
	bool updateCharacterSpritePos;
	float renderTargetClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


	// MAIN LOOP
	MSG msg;
    while (true){
		// PROCESS MESSAGES
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {
			break;
		}
		
		// READ INPUT
		result = ReadKeyboard(dInput);
		if (!result) {
			break;
		}
		result = ReadMouse(dInput);
		if (!result) {
			break;
		}
		
		// EXIT APPLICATION IF ESCAPE KEY IS PRESSED
		if (dInput->keyboardState[DIK_ESCAPE] & 0x80) {
			break;
		}
		
		
		// CALCULATE FRAME-TIME
		INT64 currentTicks;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);
		
		float elapsedTicks = (float)(currentTicks - startTicks);
		float frameTime = elapsedTicks/freqFloat;
		
		startTicks = currentTicks;
		
		
		// UPDATE CAMERA BASED ON MOVEMENT
		UpdateCamera(frameTime);
		
		
		// CALCULATE TRANSFORMATION MATRICES
		// View
		V = GetViewMatrix(); //based on camera movement
		
		//World
		M = XMMatrixIdentity();
		
		
		
		
		// RENDER OBJECTS
		StartFrame(d3d, renderTargetClearColor);
		

		// REGULAR 3D OBJECTS
		// Skybox
		M = XMMatrixScaling(100.0f, 100.0f, 100.0f);
		result = DrawSkybox(d3d->deviceContext, hWnd, &skyboxShader, &cube, matrixBuffer, M, V, P, &skyboxTextureView, d3d->skyboxDepthStencilState, d3d->depthEnabledStencilState);
		if (!result) {
			break;
		}
		
		// Color Triangle
		//M = XMMatrixIdentity();
		//result = DrawColorObject(d3d->deviceContext, hWnd, &colorShader, &triangle, matrixBuffer, M, V, P);
		//if (!result) {
		//	break;
		//}
		
		// Color Cube
		//cubeRotationAngle += 45.0f * Deg2Rad * frameTime;
		//if (cubeRotationAngle >= 360.0) {
		//	cubeRotationAngle -= 360.0;
		//}
		//M = XMMatrixMultiply(XMMatrixRotationRollPitchYaw(0.0f, cubeRotationAngle, 0.0f), XMMatrixTranslation(0.0f, 1.0001f, 0.0f));
		//result = DrawTextureObject(d3d->deviceContext, hWnd, &textureShader, &cube, matrixBuffer, M, V, P, &pattern.textureView);
		//if (!result) {
		//	break;
		//}
		
		// Plane
		//M = XMMatrixScaling(50.0f, 0.0f, 50.0f);
		//result = DrawFloorObject(d3d->deviceContext, hWnd, &floorShader, &plane, matrixBuffer, M, V, P);
		//if (!result) {
		//	break;
		//}
		
		
		
		// TRANSPARENT OBJECTS
		//EnableAlphaBlending(d3d);
		//
		//M = XMMatrixIdentity();
		//result = DrawTextureObject(d3d->deviceContext, hWnd, &textureShader, &quad, matrixBuffer, M, V, P, &characterSprite[5].textureView);
		//if (!result) {
		//	break;
		//}
		//
		//DisableAlphaBlending(d3d);
		
		
		
		// 2D OBJECTS
		DisableDepthBuffer(d3d);
		EnableAlphaBlending(d3d);
		
		
		// Background Sprite
		result = DrawObject2D(d3d->deviceContext, hWnd, &textureShader, &quad2D, matrixBuffer, M_identity, V_2D, O, &backgroundSprite.textureView);
		if (!result) {
			break;
		}
		
		// Character Sprite
		updateCharacterSpritePos = UpdateCharacterSprite(frameTime);
		result = DrawObject2D(d3d->deviceContext, hWnd, &textureShader, &quad2DDynamic, matrixBuffer, M_identity, V_2D, O, &characterSprite[spriteIndex].textureView,
							  updateCharacterSpritePos, spriteWidth, spriteHeight, spritePosX, spritePosY);
		if (!result) {
			break;
		}
		
		
		DisableAlphaBlending(d3d);
		EnableDepthBuffer(d3d);
		
		
		EndFrame(d3d);
    }

	FreeAll();
	ShutdownWindow();

    return (int) msg.wParam;
}


bool InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TINKERINGWITHWINDOWSANDDX11, szWindowClass, MAX_LOADSTRING);

	// Register Window
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TINKERINGWITHWINDOWSANDDX11));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TINKERINGWITHWINDOWSANDDX11);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);


	//Window Size/Location Properties
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int posX = 0, posY = 0;

	if(FULL_SCREEN){
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else{
		// If windowed then set it to 1280x720 resolution.
		screenWidth  = 1280;
		screenHeight = 720;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	
	//Create Window
    hInst = hInstance;
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        posX, posY, screenWidth, screenHeight, nullptr, nullptr, hInstance, nullptr);


    if(!hWnd){
        return false;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

	ShowCursor(false);

	return true;
}

void ShutdownWindow() {
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN){
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(hWnd);
	hWnd = NULL;

	// Remove the application instance.
	UnregisterClassW(szWindowClass, hInst);
	hInst = NULL;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
        case WM_COMMAND:{
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId){
            default:
              return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool InitDirectX() {
	bool result;

	// Allocate Memory
	d3d = (Direct3D*) malloc(sizeof(Direct3D));
	dInput = (DirectInput*) malloc(sizeof(DirectInput));
	if(d3d && dInput){
		ZeroMemory(d3d, sizeof(Direct3D));
		ZeroMemory(dInput, sizeof(DirectInput));
	}
	else {
		return false;
	}

	
	result = InitDirect3D(d3d, hWnd, FULL_SCREEN, screenWidth, screenHeight);
	if (!result) {
		return false;
	}

	result = InitDirectInput(dInput, hInst, hWnd);
	if (!result) {
		return false;
	}

	return true;
}


bool InitObjects() {
	bool result;

	//3D
	result = InitTriangle(d3d->device, hWnd, &triangle);
	if (!result) {
		return false;
	}

	result = InitCube(d3d->device, hWnd, &cube);
	if (!result) {
		return false;
	}

	result = InitPlane(d3d->device, hWnd, &plane);
	if (!result) {
		return false;
	}

	result = InitQuad(d3d->device, hWnd, &quad);
	if (!result) {
		return false;
	}


	//2D
	result = InitQuad2D(d3d->device, hWnd, &quad2D, screenWidth, screenHeight, -halfScreenWidth, halfScreenHeight);
	if (!result) {
		return false;
	}

	result = InitQuad2D_Dynamic(d3d->device, hWnd, &quad2DDynamic);
	if (!result) {
		return false;
	}
	

	return true;
}


bool LoadTextures() {
	bool result;

	// Pattern Texture
	wchar_t pattern_Filename[128] = L"data/textures/Pattern.png";
	result = LoadTexture2D(d3d->device, d3d->deviceContext, hWnd, pattern_Filename, &pattern);
	if (!result) {
		return false;
	}

	// Stone Texture
	wchar_t stone_Filename[128] = L"data/textures/Stone076.JPG";
	result = LoadTexture2D(d3d->device, d3d->deviceContext, hWnd, stone_Filename, &stone);
	if (!result) {
		return false;
	}

	// Background Sprite
	wchar_t backgroundSprite_Filename[128] = L"data/sprites/Garou-Gato1.png";
	result = LoadTexture2D(d3d->device, d3d->deviceContext, hWnd, backgroundSprite_Filename, &backgroundSprite);
	if (!result) {
		return false;
	}


	// Character Sprite
	wchar_t characterSprite_Filenames[SPRITE_FRAMES][128] = {
		L"data/sprites/chun-li/20906.png",
		L"data/sprites/chun-li/20907.png",
		L"data/sprites/chun-li/20908.png",
		L"data/sprites/chun-li/20909.png",
		L"data/sprites/chun-li/20910.png",
		L"data/sprites/chun-li/20911.png",
		L"data/sprites/chun-li/20912.png",
		L"data/sprites/chun-li/20913.png",
	};

	for(int i = 0; i < SPRITE_FRAMES; i++){
		result = LoadTexture2D(d3d->device, d3d->deviceContext, hWnd, characterSprite_Filenames[i], &characterSprite[i]);
		if (!result) {
			return false;
		}
	}
	

	// Skybox Texture
	wchar_t skybox_Filenames[CUBE_FACES][128] = {
		L"data/textures/skyboxes/ColdSunset/Right+X.png",
		L"data/textures/skyboxes/ColdSunset/Left-X.png",
		L"data/textures/skyboxes/ColdSunset/Up+Y.png",
		L"data/textures/skyboxes/ColdSunset/Down-Y.png",
		L"data/textures/skyboxes/ColdSunset/Front+Z.png",
		L"data/textures/skyboxes/ColdSunset/Back-Z.png"
	};
	result = LoadTextureSkybox(d3d->device, d3d->deviceContext, hWnd, skybox_Filenames, skyboxTextureArr, skyboxTextureViewArr, &skyboxTexture, &skyboxTextureView);
	if (!result) {
		return false;
	}

	return true;
}

bool InitShaders() {
	bool result;

	// Load General Shader Variables
	result = CreateMatrixBuffer(d3d->device, &matrixBuffer);
	if (!result) {
		return false;
	}
	CreateInputLayoutDesc(inputLayoutDesc);
	

	// Load Shaders
	// Color Shader
	wchar_t colorShader_Filename[128] = L"ColorShader.hlsl";
	char  colorVertexShader_Name[128] = "ColorVertexShader";
	char   colorPixelShader_Name[128] = "ColorPixelShader";	

	result = InitShader(d3d->device, hWnd, inputLayoutDesc, inputElements, &colorShader, colorShader_Filename, colorVertexShader_Name, colorPixelShader_Name);
	if (!result) {
		return false;
	}

	// Texture Shader
	wchar_t textureShader_Filename[128] = L"TextureShader.hlsl";
	char textureVertexShader_Name[128] = "TextureVertexShader";
	char  texturePixelShader_Name[128] = "TexturePixelShader";
	
	result = InitShader(d3d->device, hWnd, inputLayoutDesc, inputElements, &textureShader, textureShader_Filename, textureVertexShader_Name, texturePixelShader_Name);
	if (!result) {
		return false;
	}

	// Skybox Shader
	wchar_t skyboxShader_Filename[128] = L"SkyboxShader.hlsl";
	char  skyboxVertexShader_Name[128] = "SkyboxVertexShader";
	char   skyboxPixelShader_Name[128] = "SkyboxPixelShader";

	result = InitShader(d3d->device, hWnd, inputLayoutDesc, inputElements, &skyboxShader, skyboxShader_Filename, skyboxVertexShader_Name, skyboxPixelShader_Name);
	if (!result) {
		return false;
	}
	
	// Floor Shader
	wchar_t floorShader_Filename[128] = L"FloorShader.hlsl";
	char  floorVertexShader_Name[128] = "FloorVertexShader";
	char   floorPixelShader_Name[128] = "FloorPixelShader";

	result = InitShader(d3d->device, hWnd, inputLayoutDesc, inputElements, &floorShader, floorShader_Filename, floorVertexShader_Name, floorPixelShader_Name);
	if (!result) {
		return false;
	}

	return true;
}

bool UpdateCharacterSprite(float frameTime) {
	// Move to next frame
	spriteTimer += frameTime;
	if (spriteTimer > 0.1f) {
		spriteIndex++;
		if (spriteIndex == 8) {
			spriteIndex = 0;
		}

		spriteTimer = 0;
	}

	// Change sprite position and velocity
	spritePosX = spritePosX + (int)((float)spriteVelX * frameTime);
	if(spritePosX < -halfScreenWidth){
		spritePosX = -halfScreenWidth;
		spriteVelX = -spriteVelX;
	}
	if (spritePosX > halfScreenWidth - spriteWidth) {
		spritePosX = halfScreenWidth - spriteWidth;
		spriteVelX = -spriteVelX;
	}

	spritePosY = spritePosY + (int)((float)spriteVelY * frameTime);
	if(spritePosY < -halfScreenHeight + spriteHeight){
		spritePosY = -halfScreenHeight + spriteHeight;
		spriteVelY = -spriteVelY;
	}
	if (spritePosY > halfScreenHeight) {
		spritePosY = halfScreenHeight;
		spriteVelY = -spriteVelY;
	}

	if(spritePosX != spritePosXPrev || spritePosY != spritePosYPrev){
		return true;
	}

	// Change prev position to current position
	spritePosXPrev = spritePosX;
	spritePosYPrev = spritePosY;

	return false;
}


void UpdateCamera(float frameTime) {
	// CAMERA CONTROL PARAMETERS
	float mouseSensitivity = 10.0f;
	float walkSpeed = 4.0f;
	float runSpeed = 8.0f;


	// ROTATION
	cameraRotation.y += dInput->mouseState.lX * mouseSensitivity * frameTime;
	cameraRotation.x += dInput->mouseState.lY * mouseSensitivity * frameTime;

	// Keep Y Rotation in range [0, 360]
	if (cameraRotation.y < 0.0f)
		cameraRotation.y += 360.f;
	
	else if (cameraRotation.y > 360.0f)
		cameraRotation.y -= 360.f;
	

	// Keep X Rotation in range [-80, 90]
	if (cameraRotation.x < -80.0f)
		cameraRotation.x = -80.0f;
	
	else if (cameraRotation.x > 90.0f)
		cameraRotation.x = 90.0f;
	


	// POSITION
	float movementSpeed = walkSpeed;
	if (dInput->keyboardState[DIK_LSHIFT] & 0x80)
		movementSpeed = runSpeed;
	

	XMFLOAT2 moveInputDir = XMFLOAT2(0.0f, 0.0f);

	// Get Inputs
	if (dInput->keyboardState[DIK_W] & 0x80)
		moveInputDir.y = moveInputDir.y + 1.0f;
	
	if (dInput->keyboardState[DIK_S] & 0x80)
		moveInputDir.y = moveInputDir.y - 1.0f;
	
	if (dInput->keyboardState[DIK_D] & 0x80)
		moveInputDir.x = moveInputDir.x + 1.0f;
	
	if (dInput->keyboardState[DIK_A] & 0x80)
		moveInputDir.x = moveInputDir.x - 1.0f;
	

	// Normalize Input Vector
	float mag = sqrtf(moveInputDir.x * moveInputDir.x  +  moveInputDir.y * moveInputDir.y);
	if (mag < 0.000001f)
		return;
	moveInputDir.x = moveInputDir.x / mag;
	moveInputDir.y = moveInputDir.y / mag;


	// Rotate according to camera direction so movement is relative to the camera
	float theta = -cameraRotation.y * Deg2Rad;
	XMFLOAT2 velocity;
	velocity.x = moveInputDir.x * cosf(theta) - moveInputDir.y * sinf(theta);
	velocity.y = moveInputDir.x * sinf(theta) + moveInputDir.y * cosf(theta);

	// Set velocity
	velocity = XMFLOAT2(velocity.x * movementSpeed, velocity.y * movementSpeed);

	// Move
	cameraPosition.x = cameraPosition.x + velocity.x * frameTime;
	cameraPosition.z = cameraPosition.z + velocity.y * frameTime;
}

XMMATRIX GetViewMatrix(){
	XMVECTOR eyePosition, focusPosition, upDirection;
	XMFLOAT3 eyePositionF3, focusPositionF3, upDirectionF3;

	eyePositionF3.x = cameraPosition.x;
	eyePositionF3.y = cameraPosition.y;
	eyePositionF3.z = cameraPosition.z;
	eyePosition = XMLoadFloat3(&eyePositionF3);

	focusPositionF3.x = 0.0;								    // Initialize as focus direction
	focusPositionF3.y = 0.0;
	focusPositionF3.z = 1.0;									
	focusPosition = XMLoadFloat3(&focusPositionF3);

	upDirectionF3.x = 0.0;
	upDirectionF3.y = 1.0;
	upDirectionF3.z = 0.0;
	upDirection = XMLoadFloat3(&upDirectionF3);

	// Rotate focusDirection and UpDirection according to camera rotation
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(cameraRotation.x * Deg2Rad, cameraRotation.y * Deg2Rad, cameraRotation.z * Deg2Rad);
	focusPosition = XMVector3TransformCoord(focusPosition, rotMat);
	upDirection = XMVector3TransformCoord(upDirection, rotMat);


	focusPosition = XMVectorAdd(focusPosition, eyePosition);	// focusDirection -> focusPosition
	return XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
}


void FreeAll(){
	FreeShaders();
	FreeMatrixBuffer();

	FreeTextures();

	FreeObjects();

	FreeDirectX();
}


void FreeShaders() {
	FreeShaderData(&colorShader);
	FreeShaderData(&textureShader);
	FreeShaderData(&skyboxShader);
	FreeShaderData(&floorShader);
}

void FreeMatrixBuffer() {
	if (matrixBuffer) {
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
}


void FreeTextures() {
	FreeTexture2DData(&pattern);
	FreeTexture2DData(&stone);
	FreeTexture2DData(&backgroundSprite);
	for (int i = 0; i < SPRITE_FRAMES; i++) {
		FreeTexture2DData(&characterSprite[i]);
	}

	
	if (skyboxTextureView) {
		skyboxTextureView->Release();
		skyboxTextureView = 0;
	}

	if (skyboxTexture) {
		skyboxTexture->Release();
		skyboxTexture = 0;
	}

	for (int i = 0; i < CUBE_FACES; i++) {
		if (skyboxTextureViewArr[i]) {
			skyboxTextureViewArr[i]->Release();
			skyboxTextureViewArr[i] = 0;
		}

		if (skyboxTextureArr[i]) {
			skyboxTextureArr[i]->Release();
			skyboxTextureArr[i] = 0;
		}
	}
}


void FreeObjects(){
	FreeObjectData(&triangle);
	FreeObjectData(&cube);
	FreeObjectData(&plane);
	FreeObjectData(&quad);
	FreeObjectData(&quad2D);
	FreeObjectData(&quad2DDynamic);
}

void FreeDirectX(){
	if(dInput){
		FreeDirectInputData(dInput);

		free(dInput);
		dInput = 0;
	}

	if(d3d){
		FreeDirect3DData(d3d);

		free(d3d);
		d3d = 0;
	}
}