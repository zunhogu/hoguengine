#include "pch.h"
#include "GridClass.h"

const float TERRAIN_SCALE = 100.f;

GridClass::GridClass()
{
	m_vertices = 0;
}

GridClass::GridClass(const GridClass&)
{
}

GridClass::~GridClass()
{
}

bool GridClass::Initialize(ID3D11Device* device)
{

	bool result;

	// Manually set the width and height of the terrain.
	m_terrainWidth = 100;
	m_terrainHeight = 100;

	XMMATRIX scaleMatrix, translation;
	scaleMatrix = XMMatrixScaling(TERRAIN_SCALE, 1.f, TERRAIN_SCALE);

	translation = XMMatrixTranslation(-m_terrainWidth * TERRAIN_SCALE / 2, 0.0f, -m_terrainHeight * TERRAIN_SCALE / 2);

	m_worldMatrix = scaleMatrix * translation;

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void GridClass::Shutdown()
{
	// Release the vertex and index buffer.
	ShutdownBuffers();
}

int GridClass::GetVertexCount()
{
	return m_vertexCount;
}

XMMATRIX GridClass::GetWorldMatrix()
{
	return m_worldMatrix;
}

void GridClass::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
	return;
}

bool GridClass::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float positionX, positionZ;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 8;

	// Create the vertex array.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
		return false;

	// Initialize the index to the vertex array.
	index = 0;

	// Load the vertex and index arrays with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			// LINE 1
			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// LINE 2
			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// LINE 3
			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// LINE 4
			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;
		}
	}

	return true;
}

void GridClass::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}
}