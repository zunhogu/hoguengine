//������� ���� ��Ÿ���� ���� ��ü ����� ����ϱ� ���� ��ũ�ϴ� ���̺귯������ ����ϴ� ���̴�.
//�� ���̺귯������ DriectX �ʱ�ȭ, 3D �׷��� ������, ���ΰ�ħ ����(������ : FPS)�� ��ų�
//����ϴ� �׷���ī���� ���� ��� ���� �ϵ������� ����� ���� �ʿ��� Driect3D ����� ����ִ�.
#ifndef  _D3DClass_H_
#define _D3DClass_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "d3d11.lib") 
//#pragma comment(lib, "d3dx11.lib") 
//#pragma comment(lib, "d3dx10.lib")


//��ũ�� ���̺귯������ DirectX Ÿ�� ���ǿ� ���� �͵鿡 ���� ����� include �Ѵ�.
#include <dxgi.h> 
#include <d3dcommon.h> 
#include <d3d11.h> 
#include <DirectXMath.h>
using namespace DirectX;
//DirectX11������ d3dxmath10.h�� ����� �� ����. �� ��ɰ� ���� ������ DirectXMath.h�� include�ؿ���

//D3DClass�� �⺻Ŭ���� �����ڰ� �ְ�, ��������ڿ� �ı��ڰ� �ִ�.
class D3DClass {
private:
	bool m_vsync_enabled;    // �������� ����� �ֻ��� or ������ �ٽ� �׸� ������ ���ϴ� ����
	int m_videoCardMemory;   // �׷���ī�� ��� ������ �޸�ũ��
	char m_videoCardDescription[128];   // �׷���ī�� �̸� 

	IDXGISwapChain* m_swapChain;   // ����ü��
	ID3D11Device* m_device;   // Direct3D ��ġ
	ID3D11DeviceContext* m_deviceContext;   //Direct3D ��ġ ���ؽ�Ʈ

	ID3D11Texture2D* m_renderTargetTexture;   // ���� �������� ����Ǵ� �� = ����۶�� ���� ����
	ID3D11ShaderResourceView* m_shaderResourceView;   // m_renderTargetTexture�� �����ϴ� �� �ڿ�
	ID3D11RenderTargetView* m_renderTargetView;   // Direct3D ��ġ�� �������� �� ���۸� ����Ű�� ������ ���߿� ����۰� ����ȴ�. 

	ID3D11Texture2D* m_depthStencilBuffer;   // ����-���ٽ� ���� 
	ID3D11DepthStencilState* m_depthStencilState; // ����-���ٽ� ���� ����1
	ID3D11DepthStencilState* m_depthDisabledStencilState; // ����-���ٽ� ���� ����2 �̴� Z���۸� ���� ���ȴ�.
	ID3D11DepthStencilView* m_depthStencilView;  // ����-���ٽ� �� 

	ID3D11RasterizerState* m_rasterState; // ������ȭ�� ����
	ID3D11RasterizerState* m_rasterStateNoCulling; // ������ȭ�� ����2

	XMMATRIX m_worldMatrix;   // ���� �������� ���
	XMMATRIX m_viewMatrix;  // �� ��� 
	XMMATRIX m_projectionMatrix;   // ���� �����������
	XMMATRIX m_orthoMatrix;   // ���� �������� ���


	ID3D11BlendState* m_alphaEnableBlendingState;   // ���ĺ��� On ���º���
	ID3D11BlendState* m_alphaDisableBlendingState;   // ���ĺ��� Off ���º���

	D3D11_VIEWPORT m_viewport;

	float m_depthNear;
	float m_depthFar;
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void SetWorldMatrix(XMMATRIX matrix) { m_worldMatrix = matrix; }
	XMMATRIX GetWorldMatrix() { return m_worldMatrix; }
	void SetViewMatrix(XMMATRIX matrix) { m_viewMatrix = matrix; }
	XMMATRIX GetViewMatrix() { return m_viewMatrix; }
	XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	// 2D �̹����� �׸� �� Z ���۸� �Ѱ� ���� �Լ��� �����. 
	void TurnZBufferOn();
	void TurnZBufferOff();

	// ���ڰ� �ڿ������� ��濡 �׷����� �ֵ��� ���ĺ����Լ��� �����Ѵ�.
	// �̴� ������ ������ ��������� ����ȴ�.
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	// �ĸ� �ø� on/off
	void TurnOnCulling();
	void TurnOffCulling();

	ID3D11RenderTargetView* const* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();

	// SetBackBufferRenderTarget�Լ��� ����۸� ���� ���� Ÿ������ ������ �� �ְ� ���ش�.
	void SetBackBufferRenderTarget();
	void ResetViewport();

};

#endif 
