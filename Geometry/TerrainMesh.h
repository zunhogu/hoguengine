#pragma once
#include "Mesh.h"
#include "TextureClass.h"

const int TEXTURE_REPEAT = 8;

class TerrainMesh : public Mesh
{
private:
	struct VectorType
	{
		float x, y, z;
	};

	XMMATRIX m_worldMatrix;

	int m_terrainWidth;
	int m_terrainHeight;

	int m_heightMapWidth;
	int m_heightMapHeight;

	int m_vertexCount;
	TerrainVertexType* m_vertices;

	HeightMapType* m_heightMap;
	TextureClass* m_Texture;
public:
	TerrainMesh();
	TerrainMesh(int width, int height);
	~TerrainMesh();
	TerrainMesh(const TerrainMesh&);

	bool Initialize(ID3D11Device* device, char* heightFileName);
	void Shutdown();

	XMMATRIX GetWorldMatrix() { return m_worldMatrix; }

	int GetTerrainWidth() { return m_terrainWidth; }
	void SetTerrainWidth(int width) { m_terrainWidth = width; }
	int GetTerrainHeight() { return m_terrainHeight; }
	void SetTerrainHeight(int height) { m_terrainHeight = height; }

	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

	TerrainVertexType* GetVertexArray() { return m_vertices; }

	bool LoadHeightMap(char* path);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
private:
	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual void ShutdownBuffers();

	float LinearInterpoation(float A, float B, float Alpha);
	float BilinearInterpolation(float A, float B, float C, float D, float Alpha, float Beta);
};
