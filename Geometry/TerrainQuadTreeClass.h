#pragma once

const int MAX_TRIANGLE = 10000;

class TerrainMesh;

// 쿼드 트리의 노드(서브 쿼드)가 가지고 있는 정보를 담는 구조체다.
struct NodeType
{
	float positionX, positionZ, width;
	int triangleCount;
	TerrainVertexType* vertices;
	ID3D11Buffer* vertexBuffer, * indexBuffer;
	NodeType* nodes[4];
};

// TerrainQuadTreeClass는 쿼드 트리의 노드가 될 클래스로 TerrainClass의 정보를 받아온다.
class TerrainQuadTreeClass
{
public:

private:
	int m_triangleCount, m_drawCount;

	// 부모 쿼드로부터 물려받을 정점들을 담기 위한 배열
	TerrainVertexType* m_vertexList;
	NodeType* m_parentNode;
public:

	TerrainQuadTreeClass();
	TerrainQuadTreeClass(const TerrainQuadTreeClass&);
	~TerrainQuadTreeClass();

	bool Initialize(TerrainMesh* mesh, ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, bool isWireFrame);

	int GetDrawCount();

	NodeType* GetParentNode() { return m_parentNode; }
	void NodeTraversal(vector<NodeType*>& result, NodeType* node);

	unsigned char* CreateHeightMap(int terrainWidth, int terrainHeight);
private:
	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device);
	int CountTriangles(float positionX, float positionZ, float width);
	bool IsTriangleContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(NodeType* node);
	void RenderNode(NodeType* node, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, bool isWireFrame);
};