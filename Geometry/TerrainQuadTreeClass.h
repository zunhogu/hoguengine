#pragma once

const int MAX_TRIANGLE = 10000;

class TerrainMesh;

// TerrainQuadTreeClass�� ���� Ʈ���� ��尡 �� Ŭ������ TerrainClass�� ������ �޾ƿ´�.
class TerrainQuadTreeClass
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
		int triangleCount;
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		NodeType* nodes[4];
	};

public:
	TerrainQuadTreeClass();
	TerrainQuadTreeClass(const TerrainQuadTreeClass&);
	~TerrainQuadTreeClass();

	bool Initialize(TerrainMesh* grid, ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device);
	int CountTriangles(float positionX, float positionZ, float width);
	bool IsTriangleContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(NodeType* node);
	void RenderNode(NodeType* node, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix);

private:
	int m_triangleCount, m_drawCount;

	// �θ� ����κ��� �������� �������� ��� ���� �迭
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};