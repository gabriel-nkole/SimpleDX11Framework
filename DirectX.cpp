#include "DirectX.h"

// D3D - Main Functions
bool InitDirect3D(Direct3D* d3d, HWND &hWnd, bool FULL_SCREEN, int &screenWidth, int &screenHeight) {
	HRESULT result;
	UINT multiSampleCount = 1;
	UINT multiSampleQuality = 0;


	// Create a DirectX graphics interface factory.
	IDXGIFactory* factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Graphics Interface Factory", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Adapter", L"D3D Initialization Error", MB_OK);
		return false;
	}


	// Enumerate the primary adapter output (monitor).
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Adapter Output", L"D3D Initialization Error", MB_OK);
		return false;
	}


	unsigned int numModes = 0;

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Retrieve Number of Display Modes", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		MessageBoxW(hWnd, L"Failed to Initialize Display Mode List", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Display Mode List", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	unsigned int numerator = 0, denominator = 1;
	for (int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (UINT)GetSystemMetrics(SM_CXSCREEN)) {
			if (displayModeList[i].Height == (UINT)GetSystemMetrics(SM_CYSCREEN)) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}


	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Retrieve Adapter Description", L"D3D Initialization Error", MB_OK);
		return false;
	}


	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;



	// Store the dedicated video card memory in megabytes.
	d3d->videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	unsigned long long videoCardDescription_length;
	int error = wcstombs_s(&videoCardDescription_length, d3d->videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		MessageBoxW(hWnd, L"Failed to Retrieve Video Card Description", L"D3D Initialization Error", MB_OK);
		return false;
	}



	// Initialize the swap chain description.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set the refresh rate of the back buffer.										(VSYNC - ON | OFF)
	swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;

	
	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = multiSampleCount;
	swapChainDesc.SampleDesc.Quality = multiSampleQuality;

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hWnd;

	// Set to full screen or windowed mode.											(WINDOWED - ON | OFF)
	if (FULL_SCREEN) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}
	
	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;


	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;


	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &d3d->swapChain, &d3d->device, NULL, &d3d->deviceContext);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Swap Chain And Device", L"D3D Initialization Error", MB_OK);
		return false;
	}



	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = multiSampleCount;
	depthBufferDesc.SampleDesc.Quality = multiSampleQuality;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = (d3d->device)->CreateTexture2D(&depthBufferDesc, NULL, &d3d->depthBuffer);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Depth Buffer", L"D3D Initialization Error", MB_OK);
		return false;
	}


	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = (d3d->device)->CreateDepthStencilState(&depthStencilDesc, &d3d->depthEnabledStencilState);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Depth Buffer Enabled Stencil State", L"D3D Initialization Error", MB_OK);
		return false;
	}
	(d3d->deviceContext)->OMSetDepthStencilState(d3d->depthEnabledStencilState, 1);

	depthStencilDesc.DepthEnable = false;
	result = (d3d->device)->CreateDepthStencilState(&depthStencilDesc, &d3d->depthDisabledStencilState);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Depth Buffer Disabled Stencil State", L"D3D Initialization Error", MB_OK);
		return false;
	}


	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	result = (d3d->device)->CreateDepthStencilState(&depthStencilDesc, &d3d->skyboxDepthStencilState);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Skybox Depth Stencil State", L"D3D Initialization Error", MB_OK);
		return false;
	}


	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (multiSampleCount > 1) {
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}
	else {
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = (d3d->device)->CreateDepthStencilView(d3d->depthBuffer, &depthStencilViewDesc, &d3d->depthStencilView);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Depth Stencil View", L"D3D Initialization Error", MB_OK);
		return false;
	}


	ID3D11Texture2D* backBufferPtr;
	
	// Get the pointer to the back buffer.
	result = (d3d->swapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Find Back Buffer", L"D3D Initialization Error", MB_OK);
		return false;
	}
	
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (multiSampleCount > 1) {
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}
	else {
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	}
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	//renderTargetViewDesc.
	
	// Create the render target view with the back buffer pointer.
	result = (d3d->device)->CreateRenderTargetView(backBufferPtr, &renderTargetViewDesc, &d3d->renderTargetView);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Render Target View", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;


	// Bind the render target view and depth stencil buffer to the output render pipeline.
	(d3d->deviceContext)->OMSetRenderTargets(1, &d3d->renderTargetView, d3d->depthStencilView);


	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	if (multiSampleCount > 1) {
		rasterDesc.MultisampleEnable = true;
	}
	else {
		rasterDesc.MultisampleEnable = false;
	}
	rasterDesc.AntialiasedLineEnable = false;

	// Create the rasterizer state from the description we just filled out.
	result = (d3d->device)->CreateRasterizerState(&rasterDesc, &d3d->rasterState);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Rasterizer State", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Now set the rasterizer state.
	(d3d->deviceContext)->RSSetState(d3d->rasterState);


	// Setup the viewport for rendering.
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	(d3d->deviceContext)->RSSetViewports(1, &viewport);



	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = (d3d->device)->CreateBlendState(&blendDesc, &d3d->alphaBlendEnabledState);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Alpha-Enabled Blend State", L"D3D Initialization Error", MB_OK);
		return false;
	}

	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	result = (d3d->device)->CreateBlendState(&blendDesc, &d3d->alphaBlendDisabledState);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"Failed to Create Alpha-Disabled Blend State", L"D3D Initialization Error", MB_OK);
		return false;
	}

	float blendFactor[4] = { 0.0, 0.0, 0.0, 0.0 };
	(d3d->deviceContext)->OMSetBlendState(d3d->alphaBlendDisabledState, blendFactor, D3D11_DEFAULT_SAMPLE_MASK);

	return true;
}

void FreeDirect3DData(Direct3D* d3d) {
	if(d3d->swapChain){
		(d3d->swapChain)->SetFullscreenState(false, NULL);
	}

	if (d3d->alphaBlendDisabledState) {
		(d3d->alphaBlendDisabledState)->Release();
		 d3d->alphaBlendDisabledState = 0;
	}

	if (d3d->alphaBlendEnabledState) {
		(d3d->alphaBlendEnabledState)->Release();
		 d3d->alphaBlendEnabledState = 0;
	}

	if (d3d->rasterState) {
		(d3d->rasterState)->Release();
		 d3d->rasterState = 0;
	}

	if (d3d->renderTargetView) {
		(d3d->renderTargetView)->Release();
		 d3d->renderTargetView = 0;
	}
	
	if (d3d->depthStencilView) {
		(d3d->depthStencilView)->Release();
		 d3d->depthStencilView = 0;
	}

	if (d3d->skyboxDepthStencilState) {
		(d3d->skyboxDepthStencilState)->Release();
		 d3d->skyboxDepthStencilState = 0;
	}
	
	if (d3d->depthDisabledStencilState) {
		(d3d->depthDisabledStencilState)->Release();
		 d3d->depthDisabledStencilState = 0;
	}

	if (d3d->depthEnabledStencilState) {
		(d3d->depthEnabledStencilState)->Release();
		 d3d->depthEnabledStencilState = 0;
	}

	if (d3d->depthBuffer) {
		(d3d->depthBuffer)->Release();
		 d3d->depthBuffer = 0;
	}

	if (d3d->deviceContext) {
		(d3d->deviceContext)->Release();
		 d3d->deviceContext = 0;
	}

	if (d3d->device) {
		(d3d->device)->Release();
		 d3d->device = 0;
	}

	if(d3d->swapChain){
		(d3d->swapChain)->Release();
		 d3d->swapChain = 0;
	}
}


// D3D - Helper Functions
void StartFrame(Direct3D* d3d, float renderTargetClearColor[4]) {
	//float renderTargetClearColor[4] = { 0.2f, 0.4f, 1.0f, 1.0f };
	//float renderTargetClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//float renderTargetClearColor[4] = { r, g, b, a };
	(d3d->deviceContext)->ClearRenderTargetView(d3d->renderTargetView, renderTargetClearColor);
	(d3d->deviceContext)->ClearDepthStencilView(d3d->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void EndFrame(Direct3D* d3d) {
	(d3d->swapChain)->Present(1, 0);
}

void EnableAlphaBlending(Direct3D* d3d) {
	float blendFactor[4] = { 0.0, 0.0, 0.0, 0.0 };
	(d3d->deviceContext)->OMSetBlendState(d3d->alphaBlendEnabledState, blendFactor, D3D11_DEFAULT_SAMPLE_MASK);
}

void DisableAlphaBlending(Direct3D* d3d) {
	float blendFactor[4] = { 0.0, 0.0, 0.0, 0.0 };
	(d3d->deviceContext)->OMSetBlendState(d3d->alphaBlendDisabledState, blendFactor, D3D11_DEFAULT_SAMPLE_MASK);
}

void EnableDepthBuffer(Direct3D* d3d) {
	(d3d->deviceContext)->OMSetDepthStencilState(d3d->depthEnabledStencilState, 1);
}

void DisableDepthBuffer(Direct3D* d3d) {
	(d3d->deviceContext)->OMSetDepthStencilState(d3d->depthDisabledStencilState, 1);
}




// DirectInput - Main Functions
bool InitDirectInput(DirectInput* dInput, HINSTANCE &hInst, HWND &hWnd) {
	HRESULT result;

	result = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput->directInput, NULL);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Create Direct Input", L"D3D Initialization Error", MB_OK);
		return false;
	}
	// Initialize the direct input interface for the keyboard.
	result = (dInput->directInput)->CreateDevice(GUID_SysKeyboard, &dInput->keyboard, NULL);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Create Keyboard Interface", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = (dInput->keyboard)->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Set Keyboard Data Format", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = (dInput->keyboard)->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Set Keyboard Cooperative Level", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Now acquire the keyboard.
	result = (dInput->keyboard)->Acquire();
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Acquire Keyboard", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = (dInput->directInput)->CreateDevice(GUID_SysMouse, &dInput->mouse, NULL);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Create Mouse Interface", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = (dInput->mouse)->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Set Mouse Data Format", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = (dInput->mouse)->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Set Mouse Cooperative Level", L"D3D Initialization Error", MB_OK);
		return false;
	}

	// Acquire the mouse.
	result = (dInput->mouse)->Acquire();
	if (FAILED(result)){
		MessageBoxW(hWnd, L"Failed to Acquire Mouse", L"D3D Initialization Error", MB_OK);
		return false;
	}

	return true;
}

void FreeDirectInputData(DirectInput* dInput) {
	if (dInput->mouse) {
		(dInput->mouse)->Unacquire();
		(dInput->mouse)->Release();
		 dInput->mouse = 0;
	}

	if (dInput->keyboard) {
		(dInput->keyboard)->Unacquire();
		(dInput->keyboard)->Release();
		 dInput->keyboard = 0;
	}

	if (dInput->directInput) {
	    (dInput->directInput)->Release();
		dInput->directInput = 0;
	}
}


// DirectInput - Helper Functions
bool ReadKeyboard(DirectInput* dInput){
	// Read the keyboard device.
	HRESULT result = (dInput->keyboard)->GetDeviceState(sizeof(dInput->keyboardState), (LPVOID)&dInput->keyboardState);
	if (FAILED(result)){
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)){
			(dInput->keyboard)->Acquire();
		}
		else{
			return false;
		}
	}

	return true;
}

bool ReadMouse(DirectInput* dInput){
	// Read the mouse device.
	HRESULT result = (dInput->mouse)->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&dInput->mouseState);
	if (FAILED(result)){
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)){
			(dInput->mouse)->Acquire();
		}
		else{
			return false;
		}
	}

	return true;
}