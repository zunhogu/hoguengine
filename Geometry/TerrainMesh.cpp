#include "pch.h"
#include "TerrainMesh.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TERRAIN MESH ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
TerrainMesh::TerrainMesh()
	: m_terrainHeight(100), m_terrainWidth(100)
{
}

TerrainMesh::TerrainMesh(int width, int height)
{
	m_terrainWidth = width;
	m_terrainHeight = height;
}

TerrainMesh::~TerrainMesh()
{
}

TerrainMesh::TerrainMesh(const TerrainMesh&)
{
}

bool TerrainMesh::Initialize(ID3D11Device* device)
{
	m_worldMatrix = XMMatrixTranslation(-m_terrainWidth / 2, 0.0f, -m_terrainHeight / 2);

	bool result = true;
	InitializeBuffers(device);
	if (!result)
		return false;
}

void TerrainMesh::Shutdown()
{
	ShutdownBuffers();
}

int TerrainMesh::GetVertexCount()
{
	return m_vertexCount;
}

void TerrainMesh::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(TerrainVertexType) * m_vertexCount);
	return;
}

bool TerrainMesh::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float positionX, positionZ;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 12;

	// Create the vertex array.
	m_vertices = new TerrainVertexType[m_vertexCount];
	if (!m_vertices)
		return false;

	// Initialize the index to the vertex array.
	index = 0;

	// Load the vertex and index arrays with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{

			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper left.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;
		}
	}

	return true;
}

void TerrainMesh::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}
}