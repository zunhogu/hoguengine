// 해당 클래스는 엔진의 Geometry.dll 부분이 될 부분 맵안의 모든 모델들을 관리한다. 
// Render.dll에서 가져온 모델 데이터에 인덱스에 매기고 그 값을 Geometry.dll이 관리하면서 Client.exe와 데이터를 주고받음

//dll header include
#include "../Renderer/GraphicsClass.h"
#include "ImGuiRenderClass.h"
#include "../Collision/Collision.h"
#include "../FileSystem/PathMgr.h"

// debug 버전
#pragma comment (lib, "../Output/bin_debug/Renderer.lib")
#pragma comment (lib, "../Output/bin_debug/Collision.lib")
#pragma comment (lib, "../Output/bin_debug/FileSystem.lib")

// release 버전
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

	// Singleton 패턴으로 Core 객체 생성
	static Core* GetInst() {

		// 이 함수 최초 호출될경우
		if (nullptr == g_plnst) {
			g_plnst = new Core;  // 힙 영역으로 동적할당 
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

	bool Frame(POINT);  // 모델리스트의 모델을 순회하며 프레임마다 실행되는 함수

	void BeginScene();
	void EndScene();


	// Common

	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetDeviceContext();
	static ID3D11RenderTargetView* const* GetRenderTargetView();
	static ID3D11ShaderResourceView* GetShaderResourceView();
	static XMMATRIX GetViewMatrix();
	static XMMATRIX GetProjectionMatrix();

	static HWND GetMainHwnd() { return m_hWnd; }

	static char* ConvWcharTochar(wstring _wstr);
	static wchar_t* ConvCharToWchar(char* _str);

	static wstring GetFileName(wstring filePath);
	static wstring GetFileExtension(wstring filePath);

	static void ConvertData(char* buff, float& dest);

	static wstring GetRandomKey();

	static wstring ProcessDragAndDropPayloadTexture(ImGuiPayload* payload);
	static wstring ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload);
	static wstring ProcessDragAndDropPayloadMesh(ImGuiPayload* payload);
};
