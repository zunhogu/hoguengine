#pragma once
#include "ShaderClass.h"

class MaterialShader : public ShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
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

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11SamplerState* m_sampleState;
public:
	MaterialShader();
	MaterialShader(const MaterialShader&);
	~MaterialShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView** textures);

	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	void ShutdownShader();

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView** textures);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

};

