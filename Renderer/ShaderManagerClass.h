#pragma once

#include "D3DClass.h"
#include "ShaderClass.h"

class ModelShader;
class GridShader;
class SkyDomeShader;
class TerrainShader;

class ShaderManagerClass
{
	D3DClass* m_D3D;
	HWND m_hwnd;

	ModelShader* m_modelShader;
	GridShader* m_gridShader;
	SkyDomeShader* m_skyDomeShader;
	TerrainShader* m_terrainShader;

public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Initialize(D3DClass* d3d, HWND hwnd);
	void Shutdown();

	void RenderModelShader(ID3D11DeviceContext* deviceContext, int indexCount,
						   XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
						   XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos,
						   XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness,
						   ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisiveTexture,
						   ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture,
						   XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning
						   );

	void RenderGridShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void RenderGridShader(ID3D11DeviceContext* deviceContext, int indexCount);

	void RenderTerrainShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* texture);
	void RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount);

	void RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor);
};

