#pragma once
#include "ModelComp.h"
#include "MaterialComp.h"

class MeshPartComp
{
private:
	wstring m_materialID;

	vector<VertexType*> m_vetexArray;
	vector<UINT> m_indexArray;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	int m_vertexCount;
	int m_indexCount;

public:
	MeshPartComp();
	~MeshPartComp();

	virtual void Shutdown();

	wstring GetMaterial() { return m_materialID; }
	void SetMaterial(wstring materialID) { m_materialID = materialID; }

	vector<VertexType*>* GetVertexArray() { return &m_vetexArray; }
	vector<UINT>* GetIndexArray() { return &m_indexArray; }

	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	void SetVertexBuffer(ID3D11Buffer* buffer) { m_vertexBuffer = buffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
	void SetIndexBuffer(ID3D11Buffer* buffer) { m_indexBuffer = buffer; }

	int GetVertexCount() { return m_vertexCount; }
	void SetVertexCount(int cnt) { m_vertexCount = cnt; }
	int GetIndexCount() { return m_indexCount; }
	void SetIndexCount(int cnt) { m_indexCount = cnt; }

	void Render(ID3D11DeviceContext* devicecContext);
};

class MeshComp : public ModelComp
{
private:
	string m_meshID;
	vector<MeshPartComp*> m_meshParts;

	bool m_isRender;
public:
	MeshComp();
	MeshComp(const MeshComp& meshComp);
	~MeshComp();

	void Shutdown();

	string GetMeshID() { return m_meshID; }
	void SetMeshID(string id) { m_meshID = id; }

	void AddMeshPart(MeshPartComp* part) { m_meshParts.push_back(part); }
	vector<MeshPartComp*>* GetMeshParts() { return &m_meshParts; }

	int GetIsRender() { return m_isRender; }
	void SetIsRender(bool tf) { m_isRender = tf; }

	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMesh(ImGuiPayload* payload);

	virtual void Render(ModelNode* node);
};

