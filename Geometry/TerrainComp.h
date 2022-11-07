#pragma once

#include "ModelComp.h"
#include "TextureClass.h"
#include "MeshComp.h"
#include "TerrainQuadTreeClass.h"
#include "CameraClass.h"
#include "TextureBuffer.h"
#include "TerrainLayer.h"
#include "TerrainEditor.h"

class TerrainComp : public ModelComp
{
private:
	wstring m_terrainMeshKey;
	TerrainMesh* m_terrainMesh;
	TerrainQuadTreeClass* m_terrainQuad;
	vector<TerrainLayer*> m_layers;

	TextureClass* m_heightMapTexture;

	bool m_isRender;
	bool m_isWireFrame;
	bool m_isLOD;

	bool m_isEditMode;
	int m_selected_layer;
	TerrainEditor* m_terrainEditor;

public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	bool Initialize(ModelNode* node, wstring relativePath = L"");
	virtual void Shutdown();

	virtual void Render(ModelNode* node);
	void RederTerrain(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX baseViewMatrix, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lihgtDirection, XMFLOAT3 cameraPos);
	void Mesh(ModelNode* node);
	void TextureLayer(ModelNode* node);
	void Brush(ModelNode* node);

	wstring GetTerrainMeshID() { return m_terrainMeshKey; }

	virtual void UpdateReferComponent(ModelComp* comp);

	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
	MaterialComp* PushMaterialToLayer(ModelNode* node, int index, wstring filePath);
};

