#pragma once
#include "Mesh.h"
#include "TextureClass.h"

const int TEXTURE_REPEAT = 8;

class TerrainMesh : public Mesh
{
private:
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float tu2, tv2;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	int m_terrainWidth;
	int m_terrainHeight;

	int m_heightMapWidth;
	int m_heightMapHeight;

	int m_vertexCount;
	TerrainVertexType* m_vertices;

	HeightMapType* m_heightMap;
	ModelType* m_model;

	TextureClass* m_Texture;
public:
	TerrainMesh();
	TerrainMesh(int width, int height);
	~TerrainMesh();
	TerrainMesh(const TerrainMesh&);

	bool Initialize(ID3D11Device* device, char* heightFileName, float maximumHeight);
	void Shutdown();

	int GetTerrainWidth() { return m_terrainWidth; }
	void SetTerrainWidth(int width) { m_terrainWidth = width; }
	int GetTerrainHeight() { return m_terrainHeight; }
	void SetTerrainHeight(int height) { m_terrainHeight = height; }

	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

	TerrainVertexType* GetVertexArray() { return m_vertices; }

	bool SaveHeightMap(unsigned char* bitMapImage, char* path);
	bool LoadHeightMap(char* path);
	void NormalizeHeightMap(float maximumHeight);
	bool CalculateNormals();
	void ShutdownHeightMap();

	bool BuildModel();
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal);

	void CalculateTextureCoordinates();
private:
	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual void ShutdownBuffers();

	float LinearInterpoation(float A, float B, float Alpha);
	float BilinearInterpolation(float A, float B, float C, float D, float Alpha, float Beta);
};
