#include "pch.h"
#include "Core.h"
#include "SceneMgrClass.h"
#include "KeyMgrClass.h"
#include "ResMgrClass.h"
#include "TimeMgrClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "ContentBrowserPanel.h"
#include "ImGuIRenderClass.h"
#include "ViewPortPanel.h"
#include "RenderTextureClass.h"

Core* Core::g_plnst = nullptr;
HWND Core::m_hWnd = 0;

Core::Core() 
{
}

Core::Core(const Core& other) {}

Core::~Core() {}

bool Core::Initialize(int screendWidth, int screeHeight, HWND hwnd, HINSTANCE hInstance) {
	bool result = false;

	srand((unsigned int)time(NULL));

	m_hWnd = hwnd;

	result = GraphicsClass::GetInst()->Initialize(screendWidth, screeHeight, hwnd);
	if (!result) {
		MessageBox(nullptr, "GraphicsClass 객체 초기화 실패", "ERROR", MB_OK);
		return false;
	}

	result = ImGuIRenderClass::GetInst()->Initialize(screendWidth, screeHeight, hwnd);
	if (!result) {
		MessageBox(hwnd, "Could not initialize ImGui", "Error", MB_OK);
		return false;
	}

	PathMgr::GetInst()->Initialize();

	result = CollisionClass::GetInst()->Initialize(screendWidth, screeHeight);
	if (!result) {
		MessageBox(nullptr, "CollisionClass 객체 초기화 실패", "ERROR", MB_OK);
		return false;
	}

	result = KeyMgrClass::GetInst()->Initialize(hInstance, hwnd, screendWidth, screeHeight);
	if (!result) {
		MessageBox(nullptr, "KeyManagerClass 객체 초기화 실패", "ERROR", MB_OK);
		return false;
	}

	result = ResMgrClass::GetInst()->Initialize();
	if (!result)
	{
		MessageBox(nullptr, "ResMgrClass 객체 초기화 실패", "ERROR", MB_OK);
		return false;
	}

	result = TimeMgrClass::GetInst()->Initialize();
	if (!result) {
		return false;
	}

	// 리소스 불러오기
	srand((unsigned int)time(NULL));

	result = SceneMgrClass::GetInst()->Initialize(screendWidth, screeHeight, hwnd);
	if (!result) {
		return false;
	}


	return result;
}


void Core::Shutdown() {

	if (TimeMgrClass::GetInst())
	{
	}
	if (ResMgrClass::GetInst()) {
		ResMgrClass::GetInst()->Shutdown();
	}

	if (SceneMgrClass::GetInst()) {
		SceneMgrClass::GetInst()->Shutdown();
	}

	if (KeyMgrClass::GetInst()) {
		KeyMgrClass::GetInst()->Shutdown();
	}

	if (CollisionClass::GetInst()) {
		CollisionClass::GetInst()->Shutdown();
	}

	if (ImGuIRenderClass::GetInst()) {
		ImGuIRenderClass::GetInst()->Shutdown();
	}

	if (GraphicsClass::GetInst()) {
		GraphicsClass::GetInst()->Shutdown();
	}
}


bool Core::Frame(POINT pt) {

	KeyMgrClass::GetInst()->Frame(pt);
	SceneMgrClass::GetInst()->Frame();

	SceneMgrClass::GetInst()->Render();
	TimeMgrClass::GetInst()->Frame(m_hWnd);

	return true;
}

void Core::BeginScene()
{
	GraphicsClass::GetInst()->BeginScene(0.45f, 0.55f, 0.60f, 1.00f);
}

void Core::EndScene()
{
	GraphicsClass::GetInst()->EndScene();
}

ID3D11Device* Core::GetDevice()
{
	return GraphicsClass::GetInst()->GetDevice();
}
ID3D11DeviceContext* Core::GetDeviceContext()
{
	return GraphicsClass::GetInst()->GetDeviceContext();
}

ID3D11RenderTargetView* const* Core::GetRenderTargetView()
{
	return GraphicsClass::GetInst()->GetRenderTargetView();
}

ID3D11DepthStencilView* Core::GetDepthStencilView()
{
	return GraphicsClass::GetInst()->GetDepthStencilView();
}

XMMATRIX Core::GetViewMatrix()
{
	return GraphicsClass::GetInst()->GetViewMatrix();
}

XMMATRIX Core::GetProjectionMatrix()
{
	return GraphicsClass::GetInst()->GetProjectionMatrix();
}

void Core::SetBackBufferRenderTarget()
{
	GraphicsClass::GetInst()->SetBackBufferRenderTarget();
}

void Core::ResetViewport()
{
	GraphicsClass::GetInst()->ResetViewPort();
}

char* Core::ConvWcharTochar(wstring _wstr)
{
	char* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, _wstr.c_str(), -1, NULL, 0, NULL, NULL);
	pStr = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, _wstr.c_str(), -1, pStr, strSize, 0, 0);

	return  pStr;
}

wchar_t* Core::ConvCharToWchar(char* _cstr)
{
	wchar_t* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, _cstr, -1, NULL, NULL);
	pStr = new wchar_t[strSize];
	MultiByteToWideChar(CP_ACP, 0, _cstr, strlen(_cstr) + 1, pStr, strSize);

	return pStr;
}

wstring Core::GetFileName(wstring filePath)
{
	wstring result = L"";

	for (int i = filePath.length(); i >= 0; i--)
	{
		if (filePath[i] == L'\\')
		{
			result = filePath.substr(i + 1, filePath.length());
			break;
		}
	}

	return result;
}

wstring Core::GetFileExtension(wstring filePath)
{
	wstring result = L"";

	for (int i = filePath.length(); i >= 0; i--)
	{
		if (filePath[i] == L'.')
		{
			result = filePath.substr(i + 1, filePath.length());
			break;
		}
	}

	return result;
}


void Core::ConvertData(char* buff, float& dest) {
	string changeStr = "";
	int i;
	float data;
	if (buff[0] == '-') {
		for (i = 1; i < sizeof(buff); i++) {
			if (isdigit(buff[i]) || buff[i] == '.') {
				changeStr += buff[i];

				data = stof(changeStr);
				if (data == 0) { dest = 0; }
				else { dest = -data; }
			}
			else { break; }
		}
	}
	else if (buff[0] == '+') {
		for (i = 1; i < sizeof(buff); i++) {
			if (isdigit(buff[i]) || buff[i] == '.') {
				changeStr += buff[i];

				data = stof(changeStr);
				dest = data;
			}
			else { break; }
		}
	}
	else {
		for (i = 0; i < sizeof(buff); i++) {
			if (isdigit(buff[i]) || buff[i] == '.') {
				changeStr += buff[i];
				if (buff[i] != '.') {
					data = stof(changeStr);
					dest = data;
				}
			}
			else { break; }
		}
	}
}

wstring Core::GetRandomKey()
{
	wstring key = L"";

	for (int i = 0; i < 7; i++)
	{
		switch (rand() % 3)
		{
			case 0:
				key += rand() % 10 + 48;
				break;
			case 1:
				key += rand() % 26 + 65;
				break;
			case 2:
				key += rand() % 26 + 97;
				break;
		}
	}
	return key;
}

wstring Core::ProcessDragAndDropPayloadTexture(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;
	wstring fileExtension = GetFileExtension(fileRelativePath);

	for (int i = 0; i < IM_ARRAYSIZE(g_textureExtension); i++)
	{
		if (fileExtension == g_textureExtension[i])
		{
			result = fileRelativePath;
			break;
		}
	}

	return result;
}

wstring Core::ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;

	wstring fileExtension = GetFileExtension(fileRelativePath);

	if (fileExtension == L"material")
		result = fileRelativePath;

	return result;
}

wstring Core::ProcessDragAndDropPayloadMesh(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;

	wstring fileExtension = GetFileExtension(fileRelativePath);

	if (fileExtension == L"mesh")
		result = fileRelativePath;

	return result;
}