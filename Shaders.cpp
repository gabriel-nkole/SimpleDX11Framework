#include "Shaders.h"

bool CreateMatrixBuffer(ID3D11Device* device, ID3D11Buffer** matrixBuffer) {
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferData);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void CreateInputLayoutDesc(D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc) {
	inputLayoutDesc[0].SemanticName = "POSITION";
	inputLayoutDesc[0].SemanticIndex = 0;
	inputLayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayoutDesc[0].InputSlot = 0;
	inputLayoutDesc[0].AlignedByteOffset = 0;
	inputLayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayoutDesc[0].InstanceDataStepRate = 0;

	inputLayoutDesc[1].SemanticName = "COLOR";
	inputLayoutDesc[1].SemanticIndex = 0;
	inputLayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputLayoutDesc[1].InputSlot = 0;
	inputLayoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayoutDesc[1].InstanceDataStepRate = 0;

	inputLayoutDesc[2].SemanticName = "TEXCOORD";
	inputLayoutDesc[2].SemanticIndex = 0;
	inputLayoutDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayoutDesc[2].InputSlot = 0;
	inputLayoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayoutDesc[2].InstanceDataStepRate = 0;
}


bool InitShader(ID3D11Device* device, HWND &hWnd, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, int inputElements, 
				Shader* shader, wchar_t* shaderFilename, char* vertexShaderName, char* pixelShaderName) {
	HRESULT result;


	//Create Vertex Shader & Pixel Shader
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	result = D3DCompileFromFile(shaderFilename, NULL, NULL, vertexShaderName, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		MessageBoxW(hWnd, shaderFilename, L"Failed to Create Vertex Shader", MB_OK);
		return false;
	}

	result = D3DCompileFromFile(shaderFilename, NULL, NULL, pixelShaderName, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		MessageBoxW(hWnd, shaderFilename, L"Failed to Create Pixel Shader", MB_OK);
		return false;
	}
	device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &shader->vertexShader);
	if (FAILED(result)) {
		return false;
	}
	device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &shader->pixelShader);
	if (FAILED(result)) {
		return false;
	}


	device->CreateInputLayout(inputLayoutDesc, inputElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &shader->inputLayout);
	if (FAILED(result)) {
		MessageBoxW(hWnd, shaderFilename, L"Failed to Create Input Layout", MB_OK);
		return false;
	}


	//Release Vertex Shader & Fragment Shader Blobs
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	return true;
}


bool DrawColorObject(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* colorShader, Object* object,
					 ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P) {
	//Set shaders & 3D data
	unsigned vertexBufferStride = sizeof(VertexData), vertexBufferOffset = 0;
	deviceContext->VSSetShader(colorShader->vertexShader, NULL, 0);
	deviceContext->PSSetShader(colorShader->pixelShader, NULL, 0);
	deviceContext->IASetInputLayout(colorShader->inputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
	deviceContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	//send matrix buffer to shaders
	M = XMMatrixTranspose(M);
	V = XMMatrixTranspose(V);
	P = XMMatrixTranspose(P);

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"DrawColorObject", L"Failed to Lock Matrix Buffer", MB_OK);
		return false;
	}

	MatrixBufferData* matrixBufferPtr = (MatrixBufferData*)mappedResource.pData;
	matrixBufferPtr->world = M;
	matrixBufferPtr->view = V;
	matrixBufferPtr->projection = P;

	deviceContext->Unmap(matrixBuffer, 0);
	matrixBufferPtr = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	//draw 3D Data
	deviceContext->DrawIndexed(object->indexCount, 0, 0);

	return true;
}


bool DrawTextureObject(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* textureShader, Object* object,
					   ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P, ID3D11ShaderResourceView** textureView) {
	//Set shaders & 3D data
	unsigned vertexBufferStride = sizeof(VertexData), vertexBufferOffset = 0;
	deviceContext->VSSetShader(textureShader->vertexShader, NULL, 0);
	deviceContext->PSSetShader(textureShader->pixelShader, NULL, 0);
	deviceContext->IASetInputLayout(textureShader->inputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
	deviceContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	deviceContext->PSSetShaderResources(0, 1, textureView);


	//send matrix buffer to shaders
	M = XMMatrixTranspose(M);
	V = XMMatrixTranspose(V);
	P = XMMatrixTranspose(P);

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"DrawTextureObject", L"Failed to Lock Matrix Buffer", MB_OK);
		return false;
	}

	MatrixBufferData* matrixBufferPtr = (MatrixBufferData*)mappedResource.pData;
	matrixBufferPtr->world = M;
	matrixBufferPtr->view = V;
	matrixBufferPtr->projection = P;

	deviceContext->Unmap(matrixBuffer, 0);
	matrixBufferPtr = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	//draw 3D Data
	deviceContext->DrawIndexed(object->indexCount, 0, 0);

	return true;
}


bool DrawFloorObject(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* floorShader, Object* plane,
					 ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P) {
	//Set shaders & 3D data
	unsigned vertexBufferStride = sizeof(VertexData), vertexBufferOffset = 0;
	deviceContext->VSSetShader(floorShader->vertexShader, NULL, 0);
	deviceContext->PSSetShader(floorShader->pixelShader, NULL, 0);
	deviceContext->IASetInputLayout(floorShader->inputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &plane->vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
	deviceContext->IASetIndexBuffer(plane->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	//send matrix buffer to shaders
	M = XMMatrixTranspose(M);
	V = XMMatrixTranspose(V);
	P = XMMatrixTranspose(P);

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"DrawFloorObject", L"Failed to Lock Matrix Buffer", MB_OK);
		return false;
	}

	MatrixBufferData* matrixBufferPtr = (MatrixBufferData*)mappedResource.pData;
	matrixBufferPtr->world = M;
	matrixBufferPtr->view = V;
	matrixBufferPtr->projection = P;

	deviceContext->Unmap(matrixBuffer, 0);
	matrixBufferPtr = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	//draw 3D Data
	deviceContext->DrawIndexed(plane->indexCount, 0, 0);

	return true;
}


bool DrawSkybox(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* skyboxShader, Object* cube,
				ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P,
				ID3D11ShaderResourceView** skyboxTextureView, ID3D11DepthStencilState* skyboxDepthStencilState, ID3D11DepthStencilState* depthEnabledStencilState) {
	//Set shaders & 3D data
	unsigned vertexBufferStride = sizeof(VertexData), vertexBufferOffset = 0;
	deviceContext->VSSetShader(skyboxShader->vertexShader, NULL, 0);
	deviceContext->PSSetShader(skyboxShader->pixelShader, NULL, 0);
	deviceContext->IASetInputLayout(skyboxShader->inputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &cube->vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
	deviceContext->IASetIndexBuffer(cube->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set depth state to skybox depth state
	deviceContext->OMSetDepthStencilState(skyboxDepthStencilState, 1);


	//Set Skybox Cubemap texture
	deviceContext->PSSetShaderResources(0, 1, skyboxTextureView);


	//send matrix buffer to Skybox shaders
	M = XMMatrixTranspose(M);
	V = XMMatrixTranspose(V);
	P = XMMatrixTranspose(P);

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"DrawSkybox", L"Failed to Lock Matrix Buffer", MB_OK);
		return false;
	}

	MatrixBufferData* matrixBufferPtr = (MatrixBufferData*)mappedResource.pData;
	matrixBufferPtr->world = M;
	matrixBufferPtr->view = V;
	matrixBufferPtr->projection = P;

	deviceContext->Unmap(matrixBuffer, 0);
	matrixBufferPtr = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	//draw Skybox Cube
	deviceContext->DrawIndexed(cube->indexCount, 0, 0);

	//Reset depth state to normal
	deviceContext->OMSetDepthStencilState(depthEnabledStencilState, 1);

	return true;
}


bool DrawObject2D(ID3D11DeviceContext* deviceContext, HWND &hWnd, Shader* textureShader, Object* quad2DDynamic,
				  ID3D11Buffer* matrixBuffer, XMMATRIX M, XMMATRIX V, XMMATRIX P, ID3D11ShaderResourceView** textureView,
				  bool update, int width, int height, int posX, int posY) {
	//Set shaders & 3D data
	unsigned vertexBufferStride = sizeof(VertexData), vertexBufferOffset = 0;
	deviceContext->VSSetShader(textureShader->vertexShader, NULL, 0);
	deviceContext->PSSetShader(textureShader->pixelShader, NULL, 0);
	deviceContext->IASetInputLayout(textureShader->inputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &quad2DDynamic->vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
	deviceContext->IASetIndexBuffer(quad2DDynamic->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set texture in shader
	deviceContext->PSSetShaderResources(0, 1, textureView);


	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (update) {
		float screen_left = (float)posX;
		float screen_top = (float)posY;

		float left = screen_left;
		float right = screen_left + (float)width;

		float top = screen_top;
		float bottom = screen_top - (float)height;

	
		VertexData* vertexArr = new VertexData[quad2DDynamic->vertexCount];

		vertexArr[0].position = XMFLOAT3(left, bottom, 0.0);
		vertexArr[0].color = XMFLOAT4(1.0, 0.0, 0.0, 1.0);
		vertexArr[0].uv = XMFLOAT2(0.0, 1.0);
	
		vertexArr[1].position = XMFLOAT3(right, bottom, 0.0);
		vertexArr[1].color = XMFLOAT4(0.0, 1.0, 0.0, 1.0);
		vertexArr[1].uv = XMFLOAT2(1.0, 1.0);
	
		vertexArr[2].position = XMFLOAT3(left, top, 0.0);
		vertexArr[2].color = XMFLOAT4(0.0, 0.0, 1.0, 1.0);
		vertexArr[2].uv = XMFLOAT2(0.0, 0.0);
	
		vertexArr[3].position = XMFLOAT3(right, top, 0.0);
		vertexArr[3].color = XMFLOAT4(1.0, 1.0, 0.0, 1.0);
		vertexArr[3].uv = XMFLOAT2(1.0, 0.0);


		// Lock the vertex buffer.
		result = deviceContext->Map(quad2DDynamic->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result)){
			MessageBoxW(hWnd, L"DrawObject2D", L"Failed to Lock Vertex Buffer", MB_OK);
			return false;
		}

		// Copy the vertex data
		VertexData* vertexBufferPtr = (VertexData*)mappedResource.pData;
		memcpy(vertexBufferPtr, (void*)vertexArr, (sizeof(VertexData) * quad2DDynamic->vertexCount));

		// Unlock the vertex buffer.
		deviceContext->Unmap(quad2DDynamic->vertexBuffer, 0);

		// Release the memory
		vertexBufferPtr = 0;

		delete[] vertexArr;
		vertexArr = 0;
	}



	//send matrix buffer to shaders
	M = XMMatrixTranspose(M);
	V = XMMatrixTranspose(V);
	P = XMMatrixTranspose(P);


	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		MessageBoxW(hWnd, L"DrawObject2D", L"Failed to Lock Matrix Buffer", MB_OK);
		return false;
	}

	MatrixBufferData* matrixBufferPtr = (MatrixBufferData*)mappedResource.pData;
	matrixBufferPtr->world = M;
	matrixBufferPtr->view = V;
	matrixBufferPtr->projection = P;

	deviceContext->Unmap(matrixBuffer, 0);
	matrixBufferPtr = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	//draw 3D Data
	deviceContext->DrawIndexed(quad2DDynamic->indexCount, 0, 0);

	return true;
}


void FreeShaderData(Shader* shader) {
	if (shader->inputLayout) {
		(shader->inputLayout)->Release();
		shader->inputLayout = 0;
	}

	if (shader->pixelShader) {
		(shader->pixelShader)->Release();
		shader->pixelShader = 0;
	}

	if (shader->vertexShader) {
		(shader->vertexShader)->Release();
		shader->vertexShader = 0;
	}
}