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
	Brush m_brush;

	// picking을 위한 shader
	ComputePickingShader* m_computeShader;
	StructuredBuffer* m_structuredBuffer;

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

	stack<ID3D11Resource*> m_weightStack;

	// weight map painting을 위한 shader -> rtt에 렌더링
	ComputePaintingShader* m_computePaintingShader;

	int m_triangleSize;
	int m_terrainWidth;
	int m_terrainHeight;

public:
	TerrainEditor(TerrainVertexType* vertexArray, int teriangleSize, int terrainWidth, int terrainHeight);
	~TerrainEditor();

	void StartTerrainEditMode(TerrainLayer* material = nullptr);
	void EndTerrainEditMode();

	Brush* GetBrush() { return &m_brush; }

	void CreateComputeShader();
	bool GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix, XMFLOAT3& position);
	void PaintWeightMap(XMMATRIX baseViewMatrix);
	void PaintHeightMap(TerrainVertexType* vertices, int vertexSize, bool isRaise, int paintValue);
	XMFLOAT2 CalculateUV(XMFLOAT3 position, float width, float heigth);
};

