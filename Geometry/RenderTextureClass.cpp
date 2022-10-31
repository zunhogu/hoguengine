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

// Initialize 함수는 RTT를 할 텍스처의 너비와 높이를 인자로 받는다. 
// 만약 화면의 내용을 텍스처에 그린다면 찌그러짐 방지를 위해  반드시 RTT의 가로세로 비율을 화면 비율과 같게 해야하낟.

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

	// RTT를 생성한다.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
		return false;

	// 렌더 타겟 뷰에 대한 description을 작성한다.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타겟뷰를 생성한다. 
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
		return false;

	// 리소스 뷰에 대한 discription을 작성한다.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// 리소스뷰를 생성한다.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
		return false;

	// 깊이 버퍼에 대한 desciption 작성
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

	// 깊이 버퍼 생성
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////////////////////

	// 깊이 스텐실 뷰 description 작성

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰 생성
	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////////////////////

	// 뷰 포트 설정
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}

// Shutdown 함수는 RenderTextureClass에서 사용된 새 인터페이스들을 해제한다.
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

// SetRenderTarget 함수는 이 클래스의 m_renderTargetView를 렌더 타겟으로 설정하여 모든 렌더링이 이 텍스처에 적용되도록 한다.
bool RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext) {

	// 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 파이프라인에 바인딩한다.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 뷰포트 설정
	deviceContext->RSSetViewports(1, &m_viewport);

	return true;
}

// ClearRenderTarget 함수는 D3DClass::BeginScene 함수와 같이 백버퍼에 그리기전에 백버퍼를 지워주는 작업을 하는 함수다.
// 이 함수는 매 프레임 RTT가 일어나기전에 호출한다.
void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha) {
	float color[4];

	// RTT를 초기화할 색을 지정한다.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// RTT를 초기화한다.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼를 초기화한다.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

// GetShaderResourceView 함수는 텍스처 리소스 뷰를 반환한다.
// 이렇게 하면 RTT의 결과물을 다른 셰이더가 사용할 수 있게 된다.
// RTT 텍스처를 사용한다면 일반적으로 셰이더에 텍스처를 직접 전달해주던 부분에 이 함수를 호출하면 된다.
ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView() {
	return m_shaderResourceView;
}

void RenderTextureClass::RenderToTextureStart(ID3D11DeviceContext* deviceContext)
{
	// RTT가 렌더링 타겟이 되도록한다.
	SetRenderTarget(deviceContext);

	// RTT를 초기화한다.
	ClearRenderTarget(deviceContext, 0.0f, 0.0f, 0.0f, 1.0f);
}

void RenderTextureClass::RenderToTextureEnd()
{
	// 렌더타깃을 다시 백버퍼로 돌린다.
	Core::GetInst()->SetBackBufferRenderTarget();

	// 뷰포트도 원래대로 돌린다.
	Core::GetInst()->ResetViewport();
}