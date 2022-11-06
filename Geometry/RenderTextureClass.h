#pragma once
// RenderToTextureClass�� ����� ��� ����ڰ� ���ϴ� �ؽ�ó�� ���� Ÿ������ ������ �� �ִ�.
// ���� �������� �����͸� �ڿ��� �������ε� ������ �� �ִ�.

class RenderTextureClass {
public:
	ID3D11Texture2D* m_texture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;

	ID3D11Texture2D* m_depthStencilBuffer;   // ����-���ٽ� ���� 
	ID3D11DepthStencilView* m_depthStencilView;  // ����-���ٽ� �� 

	D3D11_VIEWPORT m_viewport;

	XMMATRIX m_orthoMatrix;

public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screeWidth, int screenHeight, float screenDepth, float screenNear, D3D11_TEXTURE2D_DESC* desc = 0);
	void Shutdown();

	bool SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha);

	ID3D11Texture2D* GetResource() { return m_texture; }
 	ID3D11ShaderResourceView*& GetShaderResourceView();

	XMMATRIX GetOrthoMatirx() { return m_orthoMatrix; }

	void RenderToTextureStart(ID3D11DeviceContext* deviceContext);
	void RenderToTextureEnd();
};