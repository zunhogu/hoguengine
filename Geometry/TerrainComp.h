#pragma once

#include "ModelComp.h"
#include "TextureClass.h"
#include "MeshComp.h"
#include "MaterialComp.h"
#include "TerrainQuadTreeClass.h"

class MaterialLayer
{
private:
	wstring m_maskID;
	MaterialComp* m_material;
	XMFLOAT4 m_chanel;

public:
	MaterialLayer();
	~MaterialLayer();
	MaterialLayer(const MaterialLayer&);

	void Shutdown();

	wstring GetMaskID() { return m_maskID; }
	void SetMaskID(wstring id) { m_maskID = id; }
	MaterialComp* GetMaterialComp() { return m_material; }
	void SetMaterialComp(MaterialComp* material) { m_material = material; }
	XMFLOAT4 GetChanel() { return m_chanel; }
	void SetChanel(XMFLOAT4 chanel) { m_chanel = chanel; }

};

class TerrainMesh
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
	XMMATRIX m_worldMatrix;
	int m_terrainWidth;
	int m_terrainHeight;
	int m_vertexCount;
	VertexType* m_vertices;
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

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
};

class TerrainComp : public ModelComp
{
private:
	TerrainMesh* m_terrainMesh;
	TerrainQuadTreeClass* m_terrainQuad;
	vector<MaterialLayer*> m_layers;
public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	bool Initialize();
	virtual void Shutdown();

	virtual void Render(ModelNode* node);
	void RederMesh(XMMATRIX worldMatrix, XMMATRIX viewMatrix);
	void Mesh(ModelNode* node);
	void TextureLayer(ModelNode* node);
	void Brush(ModelNode* node);

	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
};

