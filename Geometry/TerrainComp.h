#pragma once

#include "ModelComp.h"
#include "TextureClass.h"
#include "MeshComp.h"
#include "MaterialComp.h"

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
	string m_heightMapName;
	vector<MaterialLayer*> m_layers;
public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	void Initialize(TextureClass* m_heightMap, TextureClass* m_colorMap);
	virtual void Shutdown();

	virtual void Render(ModelNode* node);

	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
};

