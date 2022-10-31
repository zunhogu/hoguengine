#include "pch.h"
#include "RenderTextureClass.h"
#include "Core.h"

RenderTextureClass::RenderTextureClass() {
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;
}

RenderTextureClass::RenderTextureClass(const RenderTextureClass& other) { }

RenderTextureClass::~RenderTextureClass() { }

// Initialize �Լ��� RTT�� �� �ؽ�ó�� �ʺ�� ���̸� ���ڷ� �޴´�. 
// ���� ȭ���� ������ �ؽ�ó�� �׸��ٸ� ��׷��� ������ ����  �ݵ�� RTT�� ���μ��� ������ ȭ�� ������ ���� �ؾ��ϳ�.

bool RenderTextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int textureWidth, int textureHeight, float screenDepth, float screenNear) {
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;


	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// RTT�� �����Ѵ�.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
		return false;

	// ���� Ÿ�� �信 ���� description�� �ۼ��Ѵ�.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// ���� Ÿ�ٺ並 �����Ѵ�. 
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
		return false;

	// ���ҽ� �信 ���� discription�� �ۼ��Ѵ�.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// ���ҽ��並 �����Ѵ�.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
		return false;

	// ���� ���ۿ� ���� desciption �ۼ�
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ���� ���� ����
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////////////////////

	// ���� ���ٽ� �� description �ۼ�

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �� ����
	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////////////////////

	// �� ��Ʈ ����
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}

// Shutdown �Լ��� RenderTextureClass���� ���� �� �������̽����� �����Ѵ�.
void RenderTextureClass::Shutdown() {

	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}


	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_shaderResourceView) {
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture) {
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	return;
}

// SetRenderTarget �Լ��� �� Ŭ������ m_renderTargetView�� ���� Ÿ������ �����Ͽ� ��� �������� �� �ؽ�ó�� ����ǵ��� �Ѵ�.
bool RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext) {

	// ���� Ÿ�� ��� ���� ���ٽ� ���۸� ��� ���������ο� ���ε��Ѵ�.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// ����Ʈ ����
	deviceContext->RSSetViewports(1, &m_viewport);

	return true;
}

// ClearRenderTarget �Լ��� D3DClass::BeginScene �Լ��� ���� ����ۿ� �׸������� ����۸� �����ִ� �۾��� �ϴ� �Լ���.
// �� �Լ��� �� ������ RTT�� �Ͼ������ ȣ���Ѵ�.
void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha) {
	float color[4];

	// RTT�� �ʱ�ȭ�� ���� �����Ѵ�.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// RTT�� �ʱ�ȭ�Ѵ�.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// ���� ���۸� �ʱ�ȭ�Ѵ�.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

// GetShaderResourceView �Լ��� �ؽ�ó ���ҽ� �並 ��ȯ�Ѵ�.
// �̷��� �ϸ� RTT�� ������� �ٸ� ���̴��� ����� �� �ְ� �ȴ�.
// RTT �ؽ�ó�� ����Ѵٸ� �Ϲ������� ���̴��� �ؽ�ó�� ���� �������ִ� �κп� �� �Լ��� ȣ���ϸ� �ȴ�.
ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView() {
	return m_shaderResourceView;
}

void RenderTextureClass::RenderToTextureStart(ID3D11DeviceContext* deviceContext)
{
	// RTT�� ������ Ÿ���� �ǵ����Ѵ�.
	SetRenderTarget(deviceContext);

	// RTT�� �ʱ�ȭ�Ѵ�.
	ClearRenderTarget(deviceContext, 0.0f, 0.0f, 0.0f, 1.0f);
}

void RenderTextureClass::RenderToTextureEnd()
{
	// ����Ÿ���� �ٽ� ����۷� ������.
	Core::GetInst()->SetBackBufferRenderTarget();

	// ����Ʈ�� ������� ������.
	Core::GetInst()->ResetViewport();
}