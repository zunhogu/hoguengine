#include "pch.h"
#include "ShaderManagerClass.h"
#include "ModelShader.h"
#include "GridShader.h"
#include "SkyDomeShader.h"
#include "TerrainShader.h"
#include "TerrainWireFrameShader.h"
#include "MaterialShader.h"

ShaderManagerClass::ShaderManagerClass()
{
	m_D3D = 0;

	m_modelShader = 0;
	m_gridShader = 0;
	m_skyDomeShader = 0;
	m_terrainShader = 0;
	m_materialShader = 0;
}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass&)
{
}

ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Initialize(D3DClass* d3d, HWND hwnd)
{
	m_D3D = d3d;
	m_hwnd = hwnd;

	m_modelShader = new ModelShader;
	if (!m_modelShader)
		return false;

	m_modelShader->Initialize(m_D3D->GetDevice(), m_hwnd);

	m_gridShader = new GridShader;
	if (!m_gridShader)
		return false;

	m_gridShader->Initialize(m_D3D->GetDevice(), m_hwnd);


	m_skyDomeShader = new SkyDomeShader;
	if (!m_skyDomeShader)
		return false;

	m_skyDomeShader->Initialize(m_D3D->GetDevice(), m_hwnd);

	m_terrainShader = new TerrainShader;
	if (!m_terrainShader)
		return false;

	m_terrainShader->Initialize(m_D3D->GetDevice(), m_hwnd);

	m_materialShader = new MaterialShader;
	if (!m_materialShader)
		return false;

	m_materialShader->Initialize(m_D3D->GetDevice(), m_hwnd);
	if (!m_materialShader)
		return false;

	return true;
}

void ShaderManagerClass::Shutdown()
{
	if (m_materialShader)
	{
		m_materialShader->Shutdown();
		delete m_materialShader;
		m_materialShader = 0;
	}

	if (m_terrainWireFrameShader)
	{
		m_terrainWireFrameShader->Shutdown();
		delete m_terrainWireFrameShader;
		m_terrainWireFrameShader = 0;
	}

	if (m_terrainShader)
	{
		m_terrainShader->Shutdown();
		delete m_terrainShader;
		m_terrainShader = 0;
	}

	if (m_skyDomeShader)
	{
		m_skyDomeShader->Shutdown();
		delete m_skyDomeShader;
		m_skyDomeShader = 0;
	}

	if (m_gridShader)
	{
		m_gridShader->Shutdown();
		delete m_gridShader;
		m_gridShader = 0;
	}

	if (m_modelShader)
	{
		m_modelShader->Shutdown();
		delete m_modelShader;
		m_modelShader = 0;
	}
}

void ShaderManagerClass::RenderModelShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT3 lightDir, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisiveTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture, XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning)
{
	m_modelShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, lightColor, lightPos, lightDir, ambientColor, emmisiveColor, diffuseColor, specularColor, shinness, ambientTexture, emmisiveTexture, diffuseTexture, specularTexture, normalTexture, boneScale, boneMatrixArray, skinning);
}

void ShaderManagerClass::RenderGridShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	m_gridShader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
}

void ShaderManagerClass::RenderGridShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	m_gridShader->RenderShader(deviceContext, indexCount);
}

void ShaderManagerClass::RenderTerrainShaderSetParam(ID3D11DeviceContext* deviceContext, bool isWireFrame, bool isLOD, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT3 cameraPos, vector<ID3D11ShaderResourceView*>& layers, vector<float>& weights)
{
	m_terrainShader->SetShaderParameters(deviceContext, isWireFrame, isLOD, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, lightDirection, cameraPos, layers, weights);
}

void ShaderManagerClass::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	m_terrainShader->RenderShader(deviceContext, indexCount);
}

void ShaderManagerClass::RenderTerrainWireFrameSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	m_terrainWireFrameShader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount);
}

void ShaderManagerClass::RenderTerrainWireFrameShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	m_terrainWireFrameShader->RenderShader(deviceContext, indexCount);
}

void ShaderManagerClass::RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor)
{
	m_skyDomeShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
}

void ShaderManagerClass::RenderMaterialShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView** textures)
{
	m_materialShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, orthoMatrix, ambientColor, emmisiveColor, diffuseColor, specularColor, shinness, textures);
}

