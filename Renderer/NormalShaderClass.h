// ColorShaderClass�� GPU �� �����ϴ� 3D �𵨵��� �׸��� �� ����ϴ� HLSL ���̴��� ȣ���ϴ� ���̴�.

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
		// �е��� �ٿ��ִ� ������ ���߿� ������۸� ���鶧 CreateBuffer�� 16����Ʈ ������ �䱸�ϱ� �����̴�. 
		// ���� XMFLOAT3 �ǰ�� 12����Ʈ�� ��µ� ������ 4����Ʈ�� padding���� �ڵ� ä���ش�.
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

	//�Ʒ� �Լ����� ���̴��� �ʱ�ȭ�� �������� �����Ѵ�.
	// Render �Լ��� ���̴��� ���Ǵ� �������� �����ϰ� ���̴��� �̿��� �غ�� ���� �������� �׷�����.

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
	// �������� ���� �ʱ�ȭ�ϱ� ���� �������
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	// �ؽ�ó ���÷� ��ü�� ����� ���� ��ü�� ����
	ID3D11SamplerState* m_sampleState;
};

#endif