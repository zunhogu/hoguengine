#pragma once
// RenderToTextureClass�� ����� ��� ����ڰ� ���ϴ� �ؽ�ó�� ���� Ÿ������ ������ �� �ִ�.
// ���� �������� �����͸� �ڿ��� �������ε� ������ �� �ִ�.

class RenderTextureClass {
public:
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

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	void Shutdown();

	bool SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha);

	ID3D11ShaderResourceView* GetShaderResourceView();

	void RenderToTextureStart(ID3D11DeviceContext* deviceContext);
	void RenderToTextureEnd();
};