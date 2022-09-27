#ifndef _RENDERTEXTURECLASS_H_ 
#define _RENDERTEXTURECLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// RenderToTextureClass�� ����� ��� ����ڰ� ���ϴ� �ؽ�ó�� ���� Ÿ������ ������ �� �ִ�.
// ���� �������� �����͸� �ڿ��� �������ε� ������ �� �ִ�.
class RenderTextureClass {
private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;

	ID3D11Texture2D* m_depthStencilBuffer;   // ����-���ٽ� ���� 
	ID3D11DepthStencilView* m_depthStencilView;  // ����-���ٽ� �� 

	D3D11_VIEWPORT m_viewport;

public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, float, float);
	void Shutdown();

	bool SetRenderTarget(ID3D11Device*, ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);

	ID3D11ShaderResourceView* GetShaderResourceView();
};

#endif