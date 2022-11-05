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

bool BitMapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight)
{
	bool result;

	// 화면의 사이즈를 입력받는다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	result = InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

void BitMapClass::Shutdown()
{
	ShutdownBuffers();
}

bool BitMapClass::Render(ID3D11DeviceContext* deviceContext)
{
	bool result;

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
	float left, right, top, bottom;

	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	left = -(float)((m_screenWidth / 2));

	right = (float)((m_screenWidth / 2));

	top = (float)(m_screenHeight / 2);

	bottom = -(float)(m_screenHeight / 2);

	// 첫번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	indices[0] = 0;

	vertices[1].position = XMFLOAT3(right, top, 0.0f);  // Top Right
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);
	indices[1] = 1;

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	indices[2] = 2;

	// 두번째 삼각형
	vertices[3].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[3].texture = XMFLOAT2(0.0f, 1.0f);
	indices[3] = 3;

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top Right
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	indices[4] = 4;

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);
	indices[5] = 5;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

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

