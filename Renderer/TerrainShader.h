#pragma once
#include "ShaderClass.h"

class TerrainShader : public ShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDir;
		float padding;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float padding;
	};

	struct WireFrameBufferType
	{
		XMFLOAT4 wireColor;
	};	
	
	struct TessellationBufferType
	{
		XMFLOAT4 tessellationAmounts;
		XMFLOAT4 distanceStandard;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_wireFrameBuffer;
	ID3D11Buffer* m_tessellationBuffer;

public:
	TerrainShader();
	TerrainShader(const TerrainShader&);
	~TerrainShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, bool isWireFrame, bool isLOD, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);
	void ShutdownShader();
};

