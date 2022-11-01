#pragma once

#include "ModelComp.h"
#include "TextureClass.h"
#include "MeshComp.h"
#include "MaterialComp.h"
#include "TerrainQuadTreeClass.h"

class MaterialLayer
{
private:
	wstring m_maskID;  // texture layer id
	MaterialComp* m_material;
	float m_weight;

public:
	MaterialLayer();
	~MaterialLayer();
	MaterialLayer(const MaterialLayer&);

	void Shutdown();

	wstring GetMaskID() { return m_maskID; }
	void SetMaskID(wstring id) { m_maskID = id; }
	MaterialComp* GetMaterialComp() { return m_material; }
	void SetMaterialComp(MaterialComp* material) { m_material = material; }
	float GetWeight() { return m_weight; }
	void SetWeight(float weight) { m_weight = m_weight; }

};

class TerrainComp : public ModelComp
{
private:
	wstring m_terrainMeshKey;
	TerrainMesh* m_terrainMesh;
	TerrainQuadTreeClass* m_terrainQuad;
	vector<MaterialLayer*> m_layers;

	TextureClass* m_heightMapTexture;

	bool m_isRender;
	bool m_isWireFrame;
	bool m_isLOD;
public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	bool Initialize(ModelNode* node, wstring relativePath = L"");
	virtual void Shutdown();

	virtual void Render(ModelNode* node);
	void RederTerrain(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lihgtDirection, XMFLOAT3 cameraPos);
	void Mesh(ModelNode* node);
	void TextureLayer(ModelNode* node);
	void Brush(ModelNode* node);

	wstring GetTerrainMeshID() { return m_terrainMeshKey; }

	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
};

