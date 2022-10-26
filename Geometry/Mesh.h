#pragma once
#include "ResourceClass.h"
#include "../Renderer/define.h"

class MeshPart
{
private:
	wstring m_materialID;

	vector<VertexType*> m_vertexArray;
	vector<UINT> m_indexArray;

	UINT m_vertexCount;
	UINT m_indexCount;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

public:
	MeshPart();
	MeshPart(const MeshPart&);
	~MeshPart();
public:

	wstring GetMaterialID() { return m_materialID; }
	void SetMaterialID(wstring id) { m_materialID = id; }

	void AddVertex(VertexType* vertex) { m_vertexArray.push_back(vertex); }
	void AddIndex(UINT index) { m_indexArray.push_back(index); }

	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }

	vector<VertexType*> GetVertexArray() { return m_vertexArray; }
	vector<UINT> GetIndexArray() { return m_indexArray; }

	UINT GetVertexCount() { return m_vertexCount; }
	void SetVertexCount(UINT cnt) { m_vertexCount = cnt; }
	UINT GetIndexCount() { return m_indexCount; }
	void SetIndexCount(UINT cnt) { m_indexCount = cnt; }

	void InitializeBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
};

class Mesh : public ResourceClass
{
private:
	vector<MeshPart*> m_meshParts;

public:
	Mesh();
	Mesh(const Mesh&);
	~Mesh();

public:
	virtual bool Initialize(ID3D11Device* device, const wstring& _strFilePath);
	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual void Shutdown();

	vector<MeshPart*>* GetMeshParts() { return &m_meshParts; }

	// 모델 데이터를 읽어들이고 해제하는 역할을 하는 함수를 추가한다.
	bool LoadMesh(const wstring& _strFilePath);
};
