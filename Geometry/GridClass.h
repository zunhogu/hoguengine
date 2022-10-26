#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class GridClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	XMMATRIX m_worldMatrix;

	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount;
	VertexType* m_vertices;

public:
	GridClass();
	GridClass(const GridClass&);
	~GridClass();

	bool Initialize(ID3D11Device* device);
	void Shutdown();

	int GetVertexCount();
	XMMATRIX GetWorldMatrix();
	void CopyVertexArray(void* vertexList);

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
};

