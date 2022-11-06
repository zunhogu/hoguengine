#pragma once

#include "ModelComp.h"
#include "TextureClass.h"
#include "MeshComp.h"
#include "MaterialComp.h"
#include "TerrainQuadTreeClass.h"
#include "CameraClass.h"
#include "ComputePickingShader.h"
#include "StructuredBuffer.h"
#include "ComputePaintingShader.h"
#include "TextureBuffer.h"

class MaterialLayer
{
private:
	wstring m_maskID;  // texture layer id
	MaterialComp* m_material1;
	MaterialComp* m_material2;
	MaterialComp* m_material3;
	TextureBuffer* m_textureBuffer;
	BitMapClass* m_bitmap;
	RenderTextureClass* m_alphaMapBoard;

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

	TextureBuffer* GetTexturebuffer() { return m_textureBuffer; }
	void SetTextureBuffer(TextureClass* texture);

	BitMapClass* GetBitmap() { return m_bitmap; }
	RenderTextureClass* GetAlphaMapBoard() { return m_alphaMapBoard; }
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


	// Editor 
	bool m_isEditMode;
	int m_selected_layer;
	struct Brush
	{
		int brushType;
		XMFLOAT3 brushPosition;
		FLOAT brushRange;
		XMFLOAT3 brushColor;
		XMFLOAT3 chanel;
	};
	Brush m_brush;
	ComputePickingShader* m_computeShader;
	StructuredBuffer* m_structuredBuffer;
	ComputePaintingShader* m_computePaintingShader;

	struct InputDesc
	{
		UINT index;
		XMFLOAT3 v0, v1, v2;
	};

	struct OutputDesc
	{
		int picked;
		float u, v, distance;
	};

	InputDesc* m_input;
	OutputDesc* m_output;
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

	void CreateComputeShader();
	bool GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix, XMFLOAT3& position);
	void PaintBrush(XMMATRIX baseViewMatrix);
	XMFLOAT2 CalculateUV(XMFLOAT3 position, float width, float heigth);

	wstring GetTerrainMeshID() { return m_terrainMeshKey; }


	wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
};

