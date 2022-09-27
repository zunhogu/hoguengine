//�Ʒ��� �ڵ�� ���� ������ �ʴ� API���� ����ִ� Win32����� �������� �������� ����ӵ��� ���̱� �����̴�.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "TimeMgrClass.h"

//dll header include
#include "../Geometry/Core.h" 

// debug ����
// #pragma comment (lib, "../Output/bin_debug/Geometry.lib")

// release ����
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

	//WinMain�Լ����� ����� �Լ� 
	bool Initialize();
	void Shutdown();
	void Run();

private:
	//Run()���� ����
	bool Frame();
	//Initiallize()���� ����
	void InitializeWindows(int&, int&);
	//Shutdown()���� ����
	void ShutdownWindows();

	float m_previousViewPortSizeX;
	float m_previousViewPortSizeY;

};

// wndProc �Լ��� ApplicationHandle �����ʹ� MessageHandler()���� ����Ѵ�.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;