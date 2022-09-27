#include "pch.h"
#include "SystemClass.h"

#define FULL_SCREEN false
#define WINDOW_NAME L"hoguengine"
#define WINDOW_MODE_WIDTH 1600
#define WINDOW_MODE_HEIGHT 900

SystemClass::SystemClass() {
}

SystemClass::SystemClass(const SystemClass& other) {}

SystemClass::~SystemClass() {}

bool SystemClass::Initialize() {
	bool result;

	m_screenWidth = 0;
	m_screenHeight = 0;

	// 윈도우를 초기화시킨다.
	InitializeWindows(m_screenWidth, m_screenHeight);


	// Core 객체 생성
	if (FAILED(Core::GetInst()->Initialize(m_screenWidth, m_screenHeight, m_hWnd, m_hInstance))) {
		MessageBox(nullptr, L"Core 객체 초기화 실패", L"ERROR", MB_OK);
		return FALSE;
	}

	//Core::GetInst()->SetData(SCENE_TYPE::SCNENE2, GROUP_TYPE::LIGHT, RENDER_TYPE::NONE, L"Light");
	//Core::GetInst()->SetData(SCENE_TYPE::SCNENE2, GROUP_TYPE::CAMERA, RENDER_TYPE::NONE,  L"Camera");
	//Core::GetInst()->SetData(SCENE_TYPE::SCNENE1, GROUP_TYPE::DEFAULT, RENDER_TYPE::DEFAULT, L"Cube", L"id_cube", L"model\\Cube.txt");

	//Core::GetInst()->SetData(SCENE_TYPE::SCNENE1, GROUP_TYPE::LIGHT, RENDER_TYPE::NONE, L"Light");
	//Core::GetInst()->SetData(SCENE_TYPE::SCNENE1, GROUP_TYPE::CAMERA, RENDER_TYPE::NONE, L"Camera");
	//Core::GetInst()->SetData(SCENE_TYPE::SCNENE1, GROUP_TYPE::DEFAULT, RENDER_TYPE::DEFAULT, L"Cube", L"id_cube", L"model\\Cube.txt");

	return true;
}


void SystemClass::Shutdown() {

	// Window 창을 종료시킨다.
	ShutdownWindows();

	return;
}

//Run() 함수는 프로그램이 종료될 때까지 루프를 돌면서 어플리케이션의 모든 작업을 처리한다.
//어플리케이션의 모든 작업은 루프마다 불리는 Frame() 함수에서 수행된다. 이것은 어플리케이션의 다른 부분을 작성할때도 고려해야하는 중요한 개념이다. 
void SystemClass::Run() {
	MSG msg;
	bool done, result;

	//메세지 구조체를 초기화한다.
	ZeroMemory(&msg, sizeof(MSG));

	// msg.message : 발생 메시지 종류
	// msg.lParam : 발생한 메시지에 딸려온 마우스 정보 파라미터
	// msg.wParam : 발생한 메시지에 딸려온 키보드 정보 파라미터
	// msg.pt : 메시지 발생시 마우스 좌표 
	// msg.time : 메시지 발생 시간 ...
	// msg.hwnd : 메시지가 발생한 윈도우

	// 윈도우 상의 수많은 프로세스 중에 포커싱된 프로세스의 메시지큐에다가 메시지를 담음

	// 기본 메시지 루프입니다:
	// 윈도우 메시지 큐에 들어간 메시지들을 하나가져옴, 메시지가 없으면 GetMessage 함수에서 block 
	// 발생한 메시지의 종류가 WM_QUIT인 경우 false를 반환 -> 프로그램 종료
	// GetMessage 함수가 block 되기때문에 게임에 적용하기는 쉽지 않다. -> PeekMessage() : 몰래 훔쳐보다.
	// PeekMessage는 메시지가 있으면 true, 없으면 false 반환 

	done = false;
	while (!done) {   //프로그램이 종료되었나?
		// 윈도우 시스템 메세지를 확인
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);   // 메시지 분석
			DispatchMessage(&msg);   // 처리기 함수(현재는 WndProc)으로 보냄 
		}

		// 확인한 메세지를 처리한다. 종료 요청 메세지면 루프문을 빠져나감
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			// 메시지가 없을 때 호출
			// 메시지가 발생했을때에 비해 메시지가 없을때가 훨씬 시간이 길다. 이 시간동안 OS는 Idle 상태


			// 윈도우 메시지 기반 방식은 윈도우와 동기화되어서 윈도우가 focusing 되어야만 OS에서 발생하는 메시지 처리가능 

			// 비동기 함수를 사용하면 윈도우가 focusing 되지않아도 OS에서 발생하는 메시지를 확인할수 있다. 
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}
	return;
}

//Frame()함수는 어플리케이션의 모든 작업이 처리되는 곳이다.
//만약 어플리케이션의 규모가 커진다면 이 부분도 같이 커지게 된다.
bool SystemClass::Frame() {
	bool result;

	// Winapi를 이용해서 마우스 좌표를 받아옴
	POINT pt;
	RECT Rect;
	GetWindowRect(m_hWnd, &Rect);
	GetCursorPos(&pt);

	LONG Dx = pt.x - Rect.left;
	LONG Dy = pt.y - Rect.top;
	pt.x = Dx;
	pt.y = Dy;

	result = Core::GetInst()->Frame(pt);
	if (!result) {
		return false;
	}

	return true;
}


//윈도우 창의 생성 순서는 윈도우 설정 - 윈도우 등록 - 윈도우 생성 - 윈도우 출력
//InitializeWindows 함수는 우리가 렌더링 하게 될 윈도우를 만드는 코드가 들어간다. 
//이 함수는 호출한 함수(Initialize)에게로 screenWidth와 ScreenHeight 값을 다시 되돌려주므로 이 값을 어플리케이션에서 활용할 수 있다.
//현재는 까맣고 아무 경계선이 없는 기본 설정 윈도우를 만든다. 
//현재는 FULL_SCREEN의 값에 따라 작은 윈도우가 될수도, 아니면 풀 스크린의 윈도우도 만들 수 있다.(false면 800x600의 창이 만들어져 화면 가운데 위치된다.)
//FULL SCREEN 전역변수는 graphicsclass.h 에서 선언되있다.
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// 외부 포인터를 이 객체로 설정한다.
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스를 가져온다.
	m_hInstance = GetModuleHandle(NULL);

	// 어플리케이션의 이름을 설정한다.

	// 윈도우 클래스를 기본 설정으로 정의
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_NAME;
	wc.cbSize = sizeof(WNDCLASSEX);


	// 윈도우 클래스를 등록한다. 
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도를 알아온다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 풀스크린 모드 변수의 값에 따라 화면을 설정한다.
	if (FULL_SCREEN) {
		// 만약 풀스크린 모드라면 화면 크기를 데스크톱에 맞추고 색상을 32bit로 설정한다.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이를 설정한다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽 위로 맞춘다.
		posX = posY = 0;
	}
	else {
		// 윈도우 모드라면 800x600의 크기를 가지게 한다.
		screenWidth = 1600;
		screenHeight = 900;

		// 창을 모니터 중앙에 오도록한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	DWORD wcStyle = WS_SYSMENU;
	// 설정한 것을 가지고 창을 만들고 그 핸들을 가져온다.
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, WINDOW_NAME, WINDOW_NAME, wcStyle, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);
	//m_hwnd = CreateWindow(m_applicationName, m_applcationName, wcStyle, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//윈도우를 화면에 표시하고 포커스를 준다.
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	SetFocus(m_hWnd);


	return;
}

//ShutdownWindow()는 화면 설정을 되돌리고 그 핸들들을 반환한다.
void SystemClass::ShutdownWindows() {
	// 마우스 커서를 표시한다.
	ShowCursor(true);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꾼다.
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거한다.
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	// 어플리케이션 인스턴스를 제거한다.
	UnregisterClass(WINDOW_NAME, m_hInstance);
	m_hInstance = NULL;

	// 이 클래스에 대한 외부 포인터 참조를 제거한다.
	ApplicationHandle = NULL;

	return;
}

//extern LRESULT CALLBACK ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)

//WndProc 함수는 윈도우 시스템에서 메세지를 보내는 곳이다.
//이는 SystemClass에서 윈도우의 메세지를 가로채는 역할을 하고, 이것으로 코드 역시 간단하게 만들어준다.
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {

	//if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam)) {
	//	return true;
	//}

	switch (umessage) 
	{
		//윈도우가 제거되었는지 확인한다.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		// 윈도우가 닫혔는지 확인한다.
		case WM_CLOSE: 
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN: 
			switch (wparam)
			{
				case VK_ESCAPE :
					PostQuitMessage(0);
					break;
				default:
					break;
			}
			return 0;
			break;
		// 윈도우 메시지가 너무 많아서 나머지는 디폴트 처리함수 사용
		default: {
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}