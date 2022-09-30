s#pragma once

#include "ShaderClass.h"

class ModelShader : public ShaderClass
{
private:
	// 상수버퍼
	struct CameraBufferType 
	{
		XMFLOAT3 cameraPosition;
		FLOAT padding;
	};

	struct LightBufferType 
	{
		XMFLOAT4 lightColor;
		XMFLOAT3 lightPos;
		FLOAT padding;
	};

	struct MaterialBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 emmisiveColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT4 specularColor;
		FLOAT shinness;
		XMFLOAT3 padding;
	};

	struct BoneBufferType
	{
		XMMATRIX boneScale;
		XMMATRIX boneMatrixArray[100];
		UINT skinning;
		XMFLOAT3 padding;
	};

	ID3D11Buffer* m_CameraBuffer;
	ID3D11Buffer* m_LightBuffer;
	ID3D11Buffer* m_MaterialBuffer;
	ID3D11Buffer* m_BoneBuffer;

public:
	ModelShader();
	ModelShader(const ModelShader&);
	~ModelShader();

	void Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

	void InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();

	void Render(ID3D11DeviceContext* deviceContext, int indexCount, 
				XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
				XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, 
				XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness,
				ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisvieTexture, 
				ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture,
				XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning
				);

	void SetShaderParameters(ID3D11DeviceContext* deviceContext,
							XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
							XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos,
							XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness,
							ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisvieTexture,
							ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture,
							XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning
							);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

