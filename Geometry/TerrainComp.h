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

class TerrainComp : public ModelComp
{
private:
	wstring m_terrainMeshKey;
	TerrainMesh* m_terrainMesh;
	TerrainQuadTreeClass* m_terrainQuad;
	vector<MaterialLayer*> m_layers;
public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	bool Initialize(ModelNode* node);
	virtual void Shutdown();

	virtual void Render(ModelNode* node);
	void RederMesh(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightPos);
	void Mesh(ModelNode* node);
	void TextureLayer(ModelNode* node);
	void Brush(ModelNode* node);

	wstring GetTerrainMeshID() { return m_terrainMeshKey; }

	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
};

