#pragma once

const int MAX_SQUARE = 2000;

#include "GridClass.h"

// GridQuadTreeClass�� ���� Ʈ���� ��尡 �� Ŭ������ TerrainClass�� ������ �޾ƿ´�.
class GridQuadTreeClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	// ���� Ʈ���� ���(���� ����)�� ������ �ִ� ������ ��� ����ü��.
	struct NodeType
	{
		float positionX, positionZ, width;
		int squareCount;
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		NodeType* nodes[4];
	};

public:
	GridQuadTreeClass();
	GridQuadTreeClass(const GridQuadTreeClass&);
	~GridQuadTreeClass();

	bool Initialize(GridClass* grid, ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device);
	int CountSquares(float positionX, float positionZ, float width);
	bool IsSquareContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(NodeType* node);
	void RenderNode(NodeType* node, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix);

private:
	int m_squareCount, m_drawCount;

	// �θ� ����κ��� �������� �������� ��� ���� �迭
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};

