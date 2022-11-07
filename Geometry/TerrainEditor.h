#pragma once

#include "ComputePickingShader.h"
#include "ComputePaintingShader.h"
#include "StructuredBuffer.h"

class TerrainLayer;

class TerrainEditor
{
private:
	TerrainLayer* m_layer;

	// Brush 
	bool m_isEditMode;

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
	int m_triangleSize;
	int m_terrainWidth;
	int m_terrainHeight;

public:
	TerrainEditor(TerrainVertexType* vertexArray, int teriangleSize, int terrainWidth, int terrainHeight);
	~TerrainEditor();

	void StartTerrainEditMode(TerrainLayer* material);
	void UpdateTerrainEditor(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX baseViewMatrix, XMMATRIX viewMatrix);
	void EndTerrainEditMode();

	Brush* GetBrush() { return &m_brush; }

	void CreateComputeShader();
	bool GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix, XMFLOAT3& position);
	void PaintBrush(XMMATRIX baseViewMatrix);
	XMFLOAT2 CalculateUV(XMFLOAT3 position, float width, float heigth);
};

