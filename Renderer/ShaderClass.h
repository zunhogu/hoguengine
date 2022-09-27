#pragma once

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

class ShaderClass
{
protected:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

public:
	ShaderClass();
	ShaderClass(const ShaderClass&);
	virtual ~ShaderClass();

protected:
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
};

