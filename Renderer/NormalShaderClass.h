// ColorShaderClass는 GPU 상에 존재하는 3D 모델들을 그리는 데 사용하는 HLSL 셰이더를 호출하는 것이다.

#ifndef _NORMALSHADERCLASS_H_ 
#define _NORMALSHADERCLASS_H_ 

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

class NormalShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightBufferType {
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		XMFLOAT4 ambientColor;
		// 패딩을 붙여주는 이유는 나중에 상수버퍼를 만들때 CreateBuffer가 16바이트 단위를 요구하기 때문이다. 
		// 따라서 XMFLOAT3 의경우 12바이트가 드는데 나머지 4바이트를 padding으로 자동 채워준다.
		FLOAT padding;
	};
	struct CameraBufferType {
		XMFLOAT3 cameraPosition;
		FLOAT padding;
	};
public:
	NormalShaderClass();
	NormalShaderClass(const NormalShaderClass&);
	~NormalShaderClass();

	//아래 함수들은 셰이더의 초기화와 마무리를 제어한다.
	// Render 함수는 셰이더에 사용되는 변수들을 설정하고 셰이더를 이용해 준비된 모델의 정점들을 그려낸다.

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**, XMFLOAT4, XMFLOAT3, XMFLOAT4, XMFLOAT3);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**, XMFLOAT4, XMFLOAT3, XMFLOAT4, XMFLOAT3);
	void RenderShader(ID3D11DeviceContext*, int);
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	// 전역변수 값을 초기화하기 위한 상수버퍼
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	// 텍스처 샘플러 객체를 만들기 위한 객체를 선언
	ID3D11SamplerState* m_sampleState;
};

#endif