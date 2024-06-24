#include "Objects.h"


bool InitTriangle(ID3D11Device* device, HWND &hWnd, Object* object) {
	//Create Vertex & Index Arrays
	object->vertexCount = 3;
	object->indexCount = 3;

	VertexData* vertexArr = new VertexData[object->vertexCount];
	unsigned long* indexArr = new unsigned long[object->indexCount];

	ZeroMemory(vertexArr, sizeof(VertexData) * object->vertexCount);
	ZeroMemory(indexArr, sizeof(unsigned long) * object->indexCount);


	vertexArr[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertexArr[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertexArr[0].uv = XMFLOAT2(0.0f, 1.0f);

	vertexArr[1].position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexArr[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[1].uv = XMFLOAT2(1.0f, 1.0f);

	vertexArr[2].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexArr[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertexArr[2].uv = XMFLOAT2(0.0f, 0.0f);

	indexArr[0] = 0;
	indexArr[1] = 2;
	indexArr[2] = 1;

	wchar_t objectName[20] = L"Triangle";
	bool result = InitObject(device, hWnd, false, objectName, object, vertexArr, indexArr);
	if (!result) {
		return false;
	}

	//Free Vertex and Index Arrays
	delete[] vertexArr;
	vertexArr = 0;
	
	delete[] indexArr;
	indexArr = 0;

	return true;
}


bool InitCube(ID3D11Device* device, HWND &hWnd, Object* object) {
	//Create Vertex & Index Arrays
	object->vertexCount = 24;	//4 vertices * 6 faces
	object->indexCount = 36;    //6 vertices (2 triangles) * 6 faces

	VertexData* vertexArr = new VertexData[object->vertexCount];
	unsigned long* indexArr = new unsigned long[object->indexCount];

	ZeroMemory(vertexArr, sizeof(VertexData) * object->vertexCount);
	ZeroMemory(indexArr, sizeof(unsigned long) * object->indexCount);


	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);


	XMFLOAT3 front_bottomLeft =	 XMFLOAT3(-1.0f, -1.0f, -1.0f);
	XMFLOAT3 front_bottomRight = XMFLOAT3( 1.0f, -1.0f, -1.0f);
	XMFLOAT3 front_topLeft =	 XMFLOAT3(-1.0f,  1.0f, -1.0f);
	XMFLOAT3 front_topRight =	 XMFLOAT3( 1.0f,  1.0f, -1.0f);

	XMFLOAT3 back_bottomLeft =	 XMFLOAT3(-1.0f, -1.0f,  1.0f);
	XMFLOAT3 back_bottomRight =  XMFLOAT3( 1.0f, -1.0f,  1.0f);
	XMFLOAT3 back_topLeft =		 XMFLOAT3(-1.0f,  1.0f,  1.0f);
	XMFLOAT3 back_topRight =	 XMFLOAT3( 1.0f,  1.0f,  1.0f);


	XMFLOAT2 uv_bottomLeft = XMFLOAT2(0.0f, 1.0f);
	XMFLOAT2 uv_bottomRight = XMFLOAT2(1.0f, 1.0f);
	XMFLOAT2 uv_topLeft = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT2 uv_topRight = XMFLOAT2(1.0f, 0.0f);



	//front face	
	vertexArr[0].position = front_bottomLeft;
	vertexArr[0].color = red;
	vertexArr[0].uv = uv_bottomLeft;

	vertexArr[1].position = front_bottomRight;
	vertexArr[1].color = green;
	vertexArr[1].uv = uv_bottomRight;

	vertexArr[2].position = front_topLeft;
	vertexArr[2].color = blue;
	vertexArr[2].uv = uv_topLeft;

	vertexArr[3].position = front_topRight;
	vertexArr[3].color = yellow;
	vertexArr[3].uv = uv_topRight;

	indexArr[0] = 0;
	indexArr[1] = 2;
	indexArr[2] = 1;

	indexArr[3] = 1;
	indexArr[4] = 2;
	indexArr[5] = 3;

	 
	//back face
	vertexArr[4].position = back_bottomRight;
	vertexArr[4].color = red;
	vertexArr[4].uv = uv_bottomLeft;

	vertexArr[5].position = back_bottomLeft;
	vertexArr[5].color = green;
	vertexArr[5].uv = uv_bottomRight;

	vertexArr[6].position = back_topRight;
	vertexArr[6].color = blue;
	vertexArr[6].uv = uv_topLeft;

	vertexArr[7].position = back_topLeft;
	vertexArr[7].color = yellow;
	vertexArr[7].uv = uv_topRight;

	indexArr[6] = 4;
	indexArr[7] = 6;
	indexArr[8] = 5;

	indexArr[9] = 5;
	indexArr[10] = 6;
	indexArr[11] = 7;


	//left face
	vertexArr[8].position = back_bottomLeft;
	vertexArr[8].color = green;
	vertexArr[8].uv = uv_bottomLeft;
	
	vertexArr[9].position = front_bottomLeft;
	vertexArr[9].color = red;
	vertexArr[9].uv = uv_bottomRight;
	
	vertexArr[10].position = back_topLeft;
	vertexArr[10].color = yellow;
	vertexArr[10].uv = uv_topLeft;
	
	vertexArr[11].position = front_topLeft;
	vertexArr[11].color = blue;
	vertexArr[11].uv = uv_topRight;
	
	indexArr[12] = 8;
	indexArr[13] = 10;
	indexArr[14] = 9;
	
	indexArr[15] = 9;
	indexArr[16] = 10;
	indexArr[17] = 11;
	
	
	//right face
	vertexArr[12].position = front_bottomRight;
	vertexArr[12].color = green;
	vertexArr[12].uv = uv_bottomLeft;
	
	vertexArr[13].position = back_bottomRight;
	vertexArr[13].color = red;
	vertexArr[13].uv = uv_bottomRight;
	
	vertexArr[14].position = front_topRight;
	vertexArr[14].color = yellow;
	vertexArr[14].uv = uv_topLeft;
	
	vertexArr[15].position = back_topRight;
	vertexArr[15].color = blue;
	vertexArr[15].uv = uv_topRight;
	
	indexArr[18] = 12;
	indexArr[19] = 14;
	indexArr[20] = 13;
	
	indexArr[21] = 13;
	indexArr[22] = 14;
	indexArr[23] = 15;
	
	
	//top face
	vertexArr[16].position = front_topLeft;
	vertexArr[16].color = blue;
	vertexArr[16].uv = uv_bottomLeft;
	
	vertexArr[17].position = front_topRight;
	vertexArr[17].color = yellow;
	vertexArr[17].uv = uv_bottomRight;
	
	vertexArr[18].position = back_topLeft;
	vertexArr[18].color = yellow;
	vertexArr[18].uv = uv_topLeft;

	vertexArr[19].position = back_topRight;
	vertexArr[19].color = blue;
	vertexArr[19].uv = uv_topRight;
	
	indexArr[24] = 16;
	indexArr[25] = 18;
	indexArr[26] = 17;
	
	indexArr[27] = 17;
	indexArr[28] = 18;
	indexArr[29] = 19;
	
	
	//bottom face
	vertexArr[20].position = back_bottomLeft;
	vertexArr[20].color = green;
	vertexArr[20].uv = uv_bottomLeft;
	
	vertexArr[21].position = back_bottomRight;
	vertexArr[21].color = red;
	vertexArr[21].uv = uv_bottomRight;
	
	vertexArr[22].position = front_bottomLeft;
	vertexArr[22].color = red;
	vertexArr[22].uv = uv_topLeft;
	
	vertexArr[23].position = front_bottomRight;
	vertexArr[23].color = green;
	vertexArr[23].uv = uv_topRight;
	
	indexArr[30] = 20;
	indexArr[31] = 22;
	indexArr[32] = 21;
	
	indexArr[33] = 21;
	indexArr[34] = 22;
	indexArr[35] = 23;



	wchar_t objectName[20] = L"Cube";
	bool result = InitObject(device, hWnd, false, objectName, object, vertexArr, indexArr);
	if (!result) {
		return false;
	}

	//Free Vertex and Index Arrays
	delete[] vertexArr;
	vertexArr = 0;
	
	delete[] indexArr;
	indexArr = 0;

	return true;
}


bool InitPlane(ID3D11Device* device, HWND &hWnd, Object* object) {
	//Create Vertex & Index Arrays
	object->vertexCount = 4;
	object->indexCount = 6;

	VertexData* vertexArr = new VertexData[object->vertexCount];
	unsigned long* indexArr = new unsigned long[object->indexCount];

	ZeroMemory(vertexArr, sizeof(VertexData) * object->vertexCount);
	ZeroMemory(indexArr, sizeof(unsigned long) * object->indexCount);


	float size = 0.5f;

	vertexArr[0].position = XMFLOAT3(-1.0f*size, 0.0f, -1.0f*size);
	vertexArr[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertexArr[0].uv = XMFLOAT2(0.0f, 1.0f);

	vertexArr[1].position = XMFLOAT3(1.0f*size, 0.0f, -1.0f*size);
	vertexArr[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[1].uv = XMFLOAT2(1.0f, 1.0f);

	vertexArr[2].position = XMFLOAT3(-1.0f*size, 0.0f, 1.0f*size);
	vertexArr[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertexArr[2].uv = XMFLOAT2(0.0f, 0.0f);

	vertexArr[3].position = XMFLOAT3(1.0f*size, 0.0f, 1.0f*size);
	vertexArr[3].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[3].uv = XMFLOAT2(1.0f, 0.0f);

	indexArr[0] = 0;
	indexArr[1] = 2;
	indexArr[2] = 1;

	indexArr[3] = 1;
	indexArr[4] = 2;
	indexArr[5] = 3;


	wchar_t objectName[20] = L"Plane";
	bool result = InitObject(device, hWnd, false, objectName, object, vertexArr, indexArr);
	if (!result) {
		return false;
	}

	//Free Vertex and Index Arrays
	delete[] vertexArr;
	vertexArr = 0;
	
	delete[] indexArr;
	indexArr = 0;

	return true;
}


bool InitQuad(ID3D11Device* device, HWND &hWnd, Object* object) {
	//Create Vertex & Index Arrays
	object->vertexCount = 4;
	object->indexCount = 6;

	VertexData* vertexArr = new VertexData[object->vertexCount];
	unsigned long* indexArr = new unsigned long[object->indexCount];

	ZeroMemory(vertexArr, sizeof(VertexData) * object->vertexCount);
	ZeroMemory(indexArr, sizeof(unsigned long) * object->indexCount);


	vertexArr[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertexArr[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertexArr[0].uv = XMFLOAT2(0.0f, 1.0f);

	vertexArr[1].position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexArr[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[1].uv = XMFLOAT2(1.0f, 1.0f);

	vertexArr[2].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexArr[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertexArr[2].uv = XMFLOAT2(0.0f, 0.0f);

	vertexArr[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertexArr[3].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[3].uv = XMFLOAT2(1.0f, 0.0f);

	indexArr[0] = 0;
	indexArr[1] = 2;
	indexArr[2] = 1;

	indexArr[3] = 1;
	indexArr[4] = 2;
	indexArr[5] = 3;


	wchar_t objectName[20] = L"Quad";
	bool result = InitObject(device, hWnd, false, objectName, object, vertexArr, indexArr);
	if (!result) {
		return false;
	}

	//Free Vertex and Index Arrays
	delete[] vertexArr;
	vertexArr = 0;
	
	delete[] indexArr;
	indexArr = 0;

	return true;
}


bool InitQuad2D(ID3D11Device* device, HWND &hWnd, Object* object, int width, int height, int posX, int posY) {
	//Create Vertex & Index Arrays
	object->vertexCount = 4;
	object->indexCount = 6;

	VertexData* vertexArr = new VertexData[object->vertexCount];
	unsigned long* indexArr = new unsigned long[object->indexCount];

	ZeroMemory(vertexArr, sizeof(VertexData) * object->vertexCount);
	ZeroMemory(indexArr, sizeof(unsigned long) * object->indexCount);


	float screen_left = (float)posX;
	float screen_top = (float)posY;

	float left = screen_left;
	float right = screen_left + (float)width;

	float top = screen_top;
	float bottom = screen_top - (float)height;



	vertexArr[0].position = XMFLOAT3(left, bottom, 0.0f);
	vertexArr[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertexArr[0].uv = XMFLOAT2(0.0f, 1.0f);

	vertexArr[1].position = XMFLOAT3(right, bottom, 0.0f);
	vertexArr[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[1].uv = XMFLOAT2(1.0f, 1.0f);

	vertexArr[2].position = XMFLOAT3(left, top, 0.0f);
	vertexArr[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertexArr[2].uv = XMFLOAT2(0.0f, 0.0f);

	vertexArr[3].position = XMFLOAT3(right, top, 0.0f);
	vertexArr[3].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	vertexArr[3].uv = XMFLOAT2(1.0f, 0.0f);

	indexArr[0] = 0;
	indexArr[1] = 2;
	indexArr[2] = 1;
	
	indexArr[3] = 1;
	indexArr[4] = 2;
	indexArr[5] = 3;


	wchar_t objectName[20] = L"Quad2D";
	bool result = InitObject(device, hWnd, false, objectName, object, vertexArr, indexArr);
	if (!result) {
		return false;
	}

	//Free Vertex and Index Arrays
	delete[] vertexArr;
	vertexArr = 0;
	
	delete[] indexArr;
	indexArr = 0;

	return true;
}


bool InitQuad2D_Dynamic(ID3D11Device* device, HWND &hWnd, Object* object) {
	//Create Vertex & Index Arrays
	object->vertexCount = 4;
	object->indexCount = 6;

	VertexData* vertexArr = new VertexData[object->vertexCount];
	unsigned long* indexArr = new unsigned long[object->indexCount];

	ZeroMemory(vertexArr, sizeof(VertexData) * object->vertexCount);
	ZeroMemory(indexArr, sizeof(unsigned long) * object->indexCount);


	indexArr[0] = 0;
	indexArr[1] = 2;
	indexArr[2] = 1;

	indexArr[3] = 1;
	indexArr[4] = 2;
	indexArr[5] = 3;


	wchar_t objectName[20] = L"Quad2D_Dynamic";
	bool result = InitObject(device, hWnd, true, objectName, object, vertexArr, indexArr);
	if (!result) {
		return false;
	}

	//Free Vertex and Index Arrays
	delete[] vertexArr;
	vertexArr = 0;
	
	delete[] indexArr;
	indexArr = 0;

	return true;
}


//Create Vertex and Index Buffers from Vertex Buffers
bool InitObject(ID3D11Device* device, HWND &hWnd, bool isDynamic, wchar_t* objectName, Object* object, VertexData* &vertexArr, unsigned long* &indexArr) {
	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VertexData) * object->vertexCount;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	if (isDynamic) {
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexArr;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &object->vertexBuffer);
	if (FAILED(result)) {
		MessageBoxW(hWnd, objectName, L"Object Initialization Error - Failed to Create Vertex Buffer", MB_OK);
		return false;
	}



	//Create Index Buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * object->indexCount;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indexArr;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &object->indexBuffer);
	if (FAILED(result)) {
		MessageBoxW(hWnd, objectName, L"Object Initialization Error - Failed to Create Index Buffer", MB_OK);
		return false;
	}

	return true;
}


void FreeObjectData(Object* object) {
	if (object->indexBuffer) {
		(object->indexBuffer)->Release();
		object->indexBuffer = 0;
	}

	if (object->vertexBuffer) {
		(object->vertexBuffer)->Release();
		object->vertexBuffer = 0;
	}

	object->indexCount = 0;
	object->vertexCount = 0;
}