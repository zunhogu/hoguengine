#pragma once

#include "ComputePickingShader.h"
#include "ComputePaintingShader.h"
#include "StructuredBuffer.h"
#include "TerrainQuadTreeClass.h"

class TerrainLayer;

class TerrainEditor
{
private:
	TerrainLayer* m_layer;

	// Brush 
	Brush m_brush;

	// picking을 위한 shader
	ComputePickingShader* m_computePickingShader;
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

	ID3D11Texture2D* m_prevResource;
	stack<ID3D11Texture2D*> m_weightStack;

	map<int, TerrainVertexType*> m_prevVertex;
	stack<map<int, TerrainVertexType*>> m_vertexStack;

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
	void PaintHeightMap(TerrainQuadTreeClass* quadTree, bool isRaise, int paintValue);
	XMFLOAT2 CalculateUV(XMFLOAT3 position, float width, float heigth);
};

