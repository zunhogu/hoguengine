#pragma once
#include "ShaderClass.h"

class TerrainPaintShader : ShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct BrushBufferType
	{
		UINT type;
		XMFLOAT2 brushUV;
		FLOAT brushRange;
		XMFLOAT3 chanel;
		FLOAT padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_brushBuffer;
public:
	TerrainPaintShader();
	~TerrainPaintShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, UINT type, XMFLOAT2 uv, float range, XMFLOAT3 chanel, ID3D11ShaderResourceView* texture);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, UINT type, XMFLOAT2 uv, float range, XMFLOAT3 chanel, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
};

