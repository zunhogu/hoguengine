#include "pch.h"
#include "GraphicsClass.h"

// functions in Class
GraphicsClass::GraphicsClass() {
	m_D3D = 0;
	m_shaderMgr = 0;
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

	m_shaderMgr = new ShaderManagerClass;
	if (!m_shaderMgr)
		return false;

	result = m_shaderMgr->Initialize(m_D3D, hwnd);
	if(!result) 
	{
		MessageBox(hwnd, "Could not initialize ShaderManagerClass", "Error", MB_OK);
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

	return true;
}

void GraphicsClass::Shutdown() {

	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_shaderMgr)
	{
		m_shaderMgr->Shutdown();
		delete m_shaderMgr;
		m_shaderMgr = 0;
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

void GraphicsClass::TurnZBufferOn()
{
	m_D3D->TurnZBufferOn();
}

void GraphicsClass::TurnZBufferOff()
{
	m_D3D->TurnZBufferOff();
}

void GraphicsClass::TurnOnCulling()
{
	m_D3D->TurnOnCulling();
}

void GraphicsClass::TurnOffCulling()
{
	m_D3D->TurnOffCulling();
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

void GraphicsClass::RenderModel(int indexcount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT4 ambientColor, XMFLOAT4 emmisvieColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float shinnes, ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisiveTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture, XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning)
{
	XMMATRIX projectionMatrix;
	projectionMatrix = m_D3D->GetProjectionMatrix();
	m_D3D->SetViewMatrix(viewMatrix);

	m_shaderMgr->RenderModelShader(m_D3D->GetDeviceContext(), indexcount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, lightColor, lightPos, ambientColor, emmisvieColor, diffuseColor, specularColor, shinnes, ambientTexture, emmisiveTexture, diffuseTexture, specularTexture, normalTexture, boneScale, boneMatrixArray, skinning);
}

void GraphicsClass::RenderGridShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix)
{
	XMMATRIX projectionMatrix;
	projectionMatrix = m_D3D->GetProjectionMatrix();
	m_D3D->SetViewMatrix(viewMatrix);

	m_shaderMgr->RenderGridShaderSetParam(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
}

void GraphicsClass::RenderGirdShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	m_shaderMgr->RenderGridShader(deviceContext, indexCount);
}

void GraphicsClass::RenderSkyDome(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor)
{
	XMMATRIX projectionMatrix;
	projectionMatrix = m_D3D->GetProjectionMatrix();
	m_D3D->SetViewMatrix(viewMatrix);

	m_shaderMgr->RenderSkyDomeShader(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
}

void GraphicsClass::RenderTerrainShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, ID3D11ShaderResourceView* texture)
{
	XMMATRIX projectionMatrix;
	projectionMatrix = m_D3D->GetProjectionMatrix();
	m_D3D->SetViewMatrix(viewMatrix);

	m_shaderMgr->RenderTerrainShaderSetParam(deviceContext, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, lightDirection, texture);
}

void GraphicsClass::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	m_shaderMgr->RenderTerrainShader(deviceContext, indexCount);
}
