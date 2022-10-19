#pragma once

const int MAX_SQUARE = 2000;

#include "GridClass.h"

// GridQuadTreeClass는 쿼드 트리의 노드가 될 클래스로 TerrainClass의 정보를 받아온다.
class GridQuadTreeClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	// 쿼드 트리의 노드(서브 쿼드)가 가지고 있는 정보를 담는 구조체다.
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

	// 부모 쿼드로부터 물려받을 정점들을 담기 위한 배열
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};

