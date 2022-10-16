#include "pch.h"
#include "ShaderManagerClass.h"
#include "ModelShader.h"

ShaderManagerClass::ShaderManagerClass()
{
	m_D3D = 0;

	m_modelShader = 0;
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

	return true;
}

void ShaderManagerClass::Shutdown()
{
	if (m_modelShader)
	{
		m_modelShader->Shutdown();
		delete m_modelShader;
		m_modelShader = 0;
	}
}

void ShaderManagerClass::RenderModelShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisiveTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture, XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning)
{
	m_modelShader->Render(m_D3D->GetDeviceContext(), indexCount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, lightColor, lightPos, ambientColor, emmisiveColor, diffuseColor, specularColor, shinness, ambientTexture, emmisiveTexture, diffuseTexture, specularTexture, normalTexture, boneScale, boneMatrixArray, skinning);
}

