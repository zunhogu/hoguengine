#include "pch.h"
#include "BitMapClass.h"

BitMapClass::BitMapClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

BitMapClass::BitMapClass(const BitMapClass&)
{
}

BitMapClass::~BitMapClass()
{
}

bool BitMapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// 화면의 사이즈를 입력받는다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 이미지의 사이즈를 입력받는다.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 렌더링 위치 변수를 -1로 초기화한다. 이 변수는 이전 위치를 기억해두는 용도다
	// 만약 이전 프레임과 비교하여 위치가 변하지 않았다면 동적 정점 버퍼를 바꾸지 않기 때문에 성능향상을 꾀할 수 있다.
	m_previousPosX = -1;
	m_previousPosY = -1;

	result = InitializeBuffers(device);
	if (!result)
		return false;

	return false;
}

void BitMapClass::Shutdown()
{
	ShutdownBuffers();
}

bool BitMapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
		return false;

	RenderBuffers(deviceContext);

	return true;
}

int BitMapClass::GetIndexCount()
{
	return m_indexCount;
}

bool BitMapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i < m_indexCount; i++)
		indices[i] = i;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void BitMapClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

bool BitMapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// 이미지 위치가 이전과 비교하여 달라졌는지 확인한다. 바뀌지 않았다면 버퍼를 수정할 필요가 없으므로 그냥 빠져나간다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY)) {
		return true;
	}

	// 만약 이미지 위치가 바뀌었다면 다음번에 그려질 위치를 위해 새로운 위치를 기록한다.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 이미지의 네변의 위치가 계산된다. 

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// 좌표가 계산되면 임시로 정점배열을 만들고 새로운 정점 위치를 채워넣는다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// 첫번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 정점버퍼의 내용을 쓸수 있도록 잠근다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// 정점버퍼의 데이터에 대한 포인터를 가져온다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 임시 정점배열의 내용을 메모리에 미리 할당된 정점버퍼에 복사한다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼를 잠근다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 정점배열을 반환한다.
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitMapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

