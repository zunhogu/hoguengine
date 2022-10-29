#pragma once
#include "ShaderClass.h"

class TerrainWireFrameShader : public ShaderClass
{
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct TessellationBufferType {
		float tessellationAmount;
		XMFLOAT3 padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_tessellationBuffer;
public:
	TerrainWireFrameShader();
	TerrainWireFrameShader(const TerrainWireFrameShader&);
	~TerrainWireFrameShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, WCHAR* hsFilename, WCHAR* dsFilename);
	void ShutdownShader();
};

