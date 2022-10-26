#pragma once
#include "Mesh.h"

class TerrainMesh : public Mesh
{
private:
	XMMATRIX m_worldMatrix;

	int m_terrainWidth;
	int m_terrainHeight;
	int m_vertexCount;
	TerrainVertexType* m_vertices;
public:
	TerrainMesh();
	TerrainMesh(int width, int height);
	~TerrainMesh();
	TerrainMesh(const TerrainMesh&);

	bool Initialize(ID3D11Device* device);
	void Shutdown();

	XMMATRIX GetWorldMatrix() { return m_worldMatrix; }

	int GetTerrainWidth() { return m_terrainWidth; }
	void SetTerrainWidth(int width) { m_terrainWidth = width; }
	int GetTerrainHeight() { return m_terrainHeight; }
	void SetTerrainHeight(int height) { m_terrainHeight = height; }

	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

	TerrainVertexType* GetVertexArray() { return m_vertices; }

private:
	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual void ShutdownBuffers();
};
