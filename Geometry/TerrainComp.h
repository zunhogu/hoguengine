#pragma once

#include "ModelComp.h"
#include "TextureClass.h"
#include "MeshComp.h"
#include "MaterialComp.h"
#include "TerrainQuadTreeClass.h"
#include "CameraClass.h"

class MaterialLayer
{
private:
	wstring m_maskID;  // texture layer id
	MaterialComp* m_material1;
	MaterialComp* m_material2;
	MaterialComp* m_material3;

public:
	MaterialLayer();
	~MaterialLayer();
	MaterialLayer(const MaterialLayer&);

	void Shutdown();

	wstring GetMaskID() { return m_maskID; }
	void SetMaskID(wstring id) { m_maskID = id; }
	MaterialComp* GetMaterialComp1() { return m_material1; }
	void SetMaterialComp1(MaterialComp* material) { m_material1 = material; }
	MaterialComp* GetMaterialComp2() { return m_material2; }
	void SetMaterialComp2(MaterialComp* material) { m_material2 = material; }
	MaterialComp* GetMaterialComp3() { return m_material3; }
	void SetMaterialComp3(MaterialComp* material) { m_material3 = material; }
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
	bool m_isBrushMode;

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

	XMFLOAT3 GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix);

	wstring GetTerrainMeshID() { return m_terrainMeshKey; }

	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
};

