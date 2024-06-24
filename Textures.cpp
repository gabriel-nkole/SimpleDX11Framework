#include "WICTextureLoader.h"
#include "Textures.h"

//Load Texture
bool LoadTexture2D(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND &hWnd, wchar_t *filename, Texture2D* texture) {
	HRESULT result = CreateWICTextureFromFile(device, deviceContext, filename, &texture->textureResource, &texture->textureView);

	if (FAILED(result)) {
		MessageBoxW(hWnd, filename, L"Failed to Load Texture", MB_OK);
		return false;
	}

	return true;
}

//Load Skybox Texture
bool LoadTextureSkybox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND &hWnd, wchar_t filenames[CUBE_FACES][128],
					   ID3D11Resource** skyboxTextureArr, ID3D11ShaderResourceView** skyboxTextureViewArr,
					   ID3D11Texture2D** skyboxTexture, ID3D11ShaderResourceView** skyboxTextureView) {
	HRESULT result;


	//Get skybox texture properties
	UINT twidth;
	UINT theight;
	size_t rowPitch;
	size_t imageSize;
	GetWICTexturePropertiesFromFile(device, filenames[0], twidth, theight, rowPitch, imageSize);

	//Allocate memory for the raw image data of all 6 textures
	uint8_t **imgs = new uint8_t*[CUBE_FACES];
	for (size_t i = 0; i < CUBE_FACES; i++){
		imgs[i] = new uint8_t[imageSize];
	}

	//Load the 6 textures
	D3D11_SUBRESOURCE_DATA skyboxTextureDataArr[CUBE_FACES];
	for(int i = 0; i < CUBE_FACES; i++){
		result = CreateWICTextureFromFile(device, deviceContext, filenames[i], &skyboxTextureArr[i], &skyboxTextureViewArr[i], &imgs[i], skyboxTextureDataArr, i);
		if(FAILED(result)){
			MessageBoxW(hWnd, filenames[i], L"Failed to Load Skybox Texture", MB_OK);
			return false;
		}
	}

	//Create Cubemap Texture
	D3D11_TEXTURE2D_DESC skyboxTextureDesc;
	ZeroMemory(&skyboxTextureDesc, sizeof(skyboxTextureDesc));
	
	skyboxTextureDesc.Width = twidth;
	skyboxTextureDesc.Height = theight;
	skyboxTextureDesc.MipLevels = 1;
	skyboxTextureDesc.ArraySize = 6;
	skyboxTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	skyboxTextureDesc.SampleDesc.Count = 1;
	skyboxTextureDesc.SampleDesc.Quality = 0;
	skyboxTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	skyboxTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	skyboxTextureDesc.CPUAccessFlags = 0;
	skyboxTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	result = device->CreateTexture2D(&skyboxTextureDesc, &skyboxTextureDataArr[0], skyboxTexture);
	if (FAILED(result)) {
		return false;
	}
	
	
	//Create Cubemap Texture View
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = skyboxTextureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	result = device->CreateShaderResourceView(*skyboxTexture, &SRVDesc, skyboxTextureView);
	if (FAILED(result)) {
		return false;
	}


	for (size_t i = 0; i < CUBE_FACES; i++) {
		delete [] imgs[i];
		imgs[i] = 0;
	}
	delete [] imgs;
	imgs = 0;

	return true;
}


void FreeTexture2DData(Texture2D* texture) {
	if (texture->textureView) {
		(texture->textureView)->Release();
		texture->textureView = 0;
	}

	if (texture->textureResource) {
		(texture->textureResource)->Release();
		texture->textureResource = 0;
	}
}