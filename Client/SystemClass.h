//아래의 코드는 자주 사용되지 않는 API들을 담고있는 Win32헤더를 포함하지 않음으로 빌드속도를 높이기 위함이다.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "TimeMgrClass.h"

//dll header include
#include "../Geometry/Core.h" 

// debug 버전
// #pragma comment (lib, "../Output/bin_debug/Geometry.lib")

// release 버전
#pragma comment (lib, "../Output/bin/Geometry.lib")

class SystemClass {
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_screenWidth;
	int m_screenHeight;

	//TimeMgrClass* m_TimeMgr;


public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	//WinMain함수에서 실행될 함수 
	bool Initialize();
	void Shutdown();
	void Run();

private:
	//Run()에서 실행
	bool Frame();
	//Initiallize()에서 실행
	void InitializeWindows(int&, int&);
	//Shutdown()에서 실행
	void ShutdownWindows();

	float m_previousViewPortSizeX;
	float m_previousViewPortSizeY;

};

// wndProc 함수와 ApplicationHandle 포인터는 MessageHandler()에서 사용한다.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;