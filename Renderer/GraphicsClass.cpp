#include "pch.h"
#include "GraphicsClass.h"

// functions in Class
GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_NormalShader = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other) {}

GraphicsClass::~GraphicsClass() {}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;

	// Dricect3D 객체를 생성한다.
	m_D3D = new D3DClass;
	if (!m_D3D) {
		return false;
	}

	// Direct3D 객체를 초기화한다.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	// RTT 객체 생성 
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		cout << "error rtt can't initialize!!" << endl;
		return false;
	}

	// DefaultShaderClass 객체 생성
	m_DefaultShader = new DefaultShaderClass();
	if (!m_DefaultShader) {
		return false;
	}

	// DefaultShaderClas 객체 초기화
	result = m_DefaultShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the default shader object.", "Error", MB_OK);
		return false;
	}

	// NormalShaderClass 객체 생성
	m_NormalShader = new NormalShaderClass;
	if (!m_NormalShader) {
		return false;
	}

	// NormalShaderClass 객체 초기화
	result = m_NormalShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the normal shader object.", "Error", MB_OK);
		return false;
	}

	m_modelShader = new ModelShader;
	if (!m_modelShader) {
		return false;
	}

	// NormalShaderClass 객체 초기화
	m_modelShader->Initialize(m_D3D->GetDevice(), hwnd);

	return true;
}

void GraphicsClass::Shutdown() {

	if (m_NormalShader) {
		m_NormalShader->Shutdown();
		delete m_NormalShader;
		m_NormalShader = 0;
	}

	if (m_DefaultShader) {
		m_DefaultShader->Shutdown();
		delete m_DefaultShader;
		m_DefaultShader = 0;
	}

	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Direct3D 객체 반환
	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

D3DClass* GraphicsClass::GetDirectDevice() {
	return m_D3D;
}

ID3D11Device* GraphicsClass::GetDevice() {
	return m_D3D->GetDevice();
}
ID3D11DeviceContext* GraphicsClass::GetDeviceContext()
{
	return m_D3D->GetDeviceContext();
}

ID3D11RenderTargetView* const* GraphicsClass::GetRenderTargetView()
{
	return m_D3D->GetRenderTargetView();
}

ID3D11ShaderResourceView* GraphicsClass::GetShaderResourceView()
{
	return m_RenderTexture->GetShaderResourceView();
}

XMMATRIX GraphicsClass::GetProjectionMatrix() {
	XMMATRIX projectionMatrix;
	projectionMatrix = m_D3D->GetProjectionMatrix();
	return projectionMatrix;
}

XMMATRIX GraphicsClass::GetViewMatrix()
{
	return m_D3D->GetViewMatrix();
}

RenderTextureClass* GraphicsClass::GetRenderTextureClass()
{
	return m_RenderTexture;
}

void GraphicsClass::BeginScene(float x, float y, float z, float w) {
	m_D3D->BeginScene(x, y, z, w);
}
void GraphicsClass::EndScene() {
	m_D3D->EndScene();
}

void GraphicsClass::RenderToTextureStart() {

	// RTT가 렌더링 타겟이 되도록한다.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// RTT를 초기화한다.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
}

void GraphicsClass::RenderToTextureEnd()
{
	// 렌더타깃을 다시 백버퍼로 돌린다.
	m_D3D->SetBackBufferRenderTarget();

	// 뷰포트도 원래대로 돌린다.
	m_D3D->ResetViewport();
}

bool GraphicsClass::RenderDefault(int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT3 cameraPos, XMFLOAT4 diffuseColor, XMFLOAT3 lightPos, XMFLOAT4 ambientColor, ID3D11ShaderResourceView* texture)
{
	bool result;
	XMMATRIX projectionMatrix;

	projectionMatrix = m_D3D->GetProjectionMatrix();
	m_D3D->SetViewMatrix(viewMatrix);

	// DefaultShader 실행
	result = m_DefaultShader->Render(m_D3D->GetDeviceContext(), indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::RenderNormal(int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT3 cameraPos, XMFLOAT4 diffuseColor, XMFLOAT3 lightPos, XMFLOAT4 ambientColor, ID3D11ShaderResourceView* texture)
{
	//bool result;
	//XMMATRIX projectionMatrix;

	//m_D3D->GetProjectionMatrix(projectionMatrix);

	////	// NormalShader 실행
	//result = m_NormalShader->Render(m_D3D->GetDeviceContext(), indexCount, worldMatrix, viewMatrix, projectionMatrix, textureArray, diffuseColor, lightPos, ambientColor, cameraPos);
	//if (!result) {
	//	return false;
	//}

	return true;
}

bool GraphicsClass::Render(int indexcount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT4 ambientColor, XMFLOAT4 emmisvieColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float shinnes, ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisiveTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture, XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning)
{
	bool result;
	XMMATRIX projectionMatrix;

	projectionMatrix = m_D3D->GetProjectionMatrix();
	m_D3D->SetViewMatrix(viewMatrix);

	m_modelShader->Render(m_D3D->GetDeviceContext(), indexcount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, lightColor, lightPos, ambientColor, emmisvieColor, diffuseColor, specularColor, shinnes, ambientTexture, emmisiveTexture, diffuseTexture, specularTexture, normalTexture, boneScale, boneMatrixArray, skinning);

	return true;
}
