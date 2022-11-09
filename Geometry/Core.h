// �ش� Ŭ������ ������ Geometry.dll �κ��� �� �κ� �ʾ��� ��� �𵨵��� �����Ѵ�. 
// Render.dll���� ������ �� �����Ϳ� �ε����� �ű�� �� ���� Geometry.dll�� �����ϸ鼭 Client.exe�� �����͸� �ְ����

//dll header include
#include "../Renderer/GraphicsClass.h"
#include "ImGuiRenderClass.h"
#include "../Collision/Collision.h"
#include "../FileSystem/PathMgr.h"
#include "../Utility/Utility.h"

// debug ����
#pragma comment (lib, "../Output/bin_debug/Renderer.lib")
#pragma comment (lib, "../Output/bin_debug/Collision.lib")
#pragma comment (lib, "../Output/bin_debug/FileSystem.lib")
#pragma comment (lib, "../Output/bin_debug/Utility.lib")

// release ����
//#pragma comment (lib, "../Output/bin/Collision.lib")
//#pragma comment (lib, "../Output/bin/Renderer.lib")
//#pragma comment (lib, "../Output/bin/FileSystem.lib") 

class Object;

class MYDLL_DECLSPEC Core {
private:
	static Core* g_plnst;
	static HWND m_hWnd;

	Core();
	Core(const Core&);
	~Core();

public:

	// Singleton �������� Core ��ü ����
	static Core* GetInst() {

		// �� �Լ� ���� ȣ��ɰ��
		if (nullptr == g_plnst) {
			g_plnst = new Core;  // �� �������� �����Ҵ� 
		}

		return g_plnst;
	}

	static void Release() {
		if (nullptr != g_plnst) {
			delete g_plnst;
			g_plnst = nullptr;
		}
	}

	bool Initialize(int, int, HWND, HINSTANCE);
	void Shutdown();

	bool Frame(POINT);  // �𵨸���Ʈ�� ���� ��ȸ�ϸ� �����Ӹ��� ����Ǵ� �Լ�

	void BeginScene();
	void EndScene();


	// Common

	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetDeviceContext();
	static ID3D11RenderTargetView* const* GetRenderTargetView();
	static ID3D11DepthStencilView* GetDepthStencilView();
	static XMMATRIX GetViewMatrix();
	static XMMATRIX GetProjectionMatrix();
	static void SetBackBufferRenderTarget();
	static void ResetViewport();

	static HWND GetMainHwnd() { return m_hWnd; }

	static wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	static wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
	static wstring ProcessDragAndDropPayloadMesh(ImGuiPayload* payload);
};
