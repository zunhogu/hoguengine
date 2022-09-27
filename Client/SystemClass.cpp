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

	// �����츦 �ʱ�ȭ��Ų��.
	InitializeWindows(m_screenWidth, m_screenHeight);


	// Core ��ü ����
	if (FAILED(Core::GetInst()->Initialize(m_screenWidth, m_screenHeight, m_hWnd, m_hInstance))) {
		MessageBox(nullptr, L"Core ��ü �ʱ�ȭ ����", L"ERROR", MB_OK);
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

	// Window â�� �����Ų��.
	ShutdownWindows();

	return;
}

//Run() �Լ��� ���α׷��� ����� ������ ������ ���鼭 ���ø����̼��� ��� �۾��� ó���Ѵ�.
//���ø����̼��� ��� �۾��� �������� �Ҹ��� Frame() �Լ����� ����ȴ�. �̰��� ���ø����̼��� �ٸ� �κ��� �ۼ��Ҷ��� ����ؾ��ϴ� �߿��� �����̴�. 
void SystemClass::Run() {
	MSG msg;
	bool done, result;

	//�޼��� ����ü�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&msg, sizeof(MSG));

	// msg.message : �߻� �޽��� ����
	// msg.lParam : �߻��� �޽����� ������ ���콺 ���� �Ķ����
	// msg.wParam : �߻��� �޽����� ������ Ű���� ���� �Ķ����
	// msg.pt : �޽��� �߻��� ���콺 ��ǥ 
	// msg.time : �޽��� �߻� �ð� ...
	// msg.hwnd : �޽����� �߻��� ������

	// ������ ���� ������ ���μ��� �߿� ��Ŀ�̵� ���μ����� �޽���ť���ٰ� �޽����� ����

	// �⺻ �޽��� �����Դϴ�:
	// ������ �޽��� ť�� �� �޽������� �ϳ�������, �޽����� ������ GetMessage �Լ����� block 
	// �߻��� �޽����� ������ WM_QUIT�� ��� false�� ��ȯ -> ���α׷� ����
	// GetMessage �Լ��� block �Ǳ⶧���� ���ӿ� �����ϱ�� ���� �ʴ�. -> PeekMessage() : ���� ���ĺ���.
	// PeekMessage�� �޽����� ������ true, ������ false ��ȯ 

	done = false;
	while (!done) {   //���α׷��� ����Ǿ���?
		// ������ �ý��� �޼����� Ȯ��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);   // �޽��� �м�
			DispatchMessage(&msg);   // ó���� �Լ�(����� WndProc)���� ���� 
		}

		// Ȯ���� �޼����� ó���Ѵ�. ���� ��û �޼����� �������� ��������
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			// �޽����� ���� �� ȣ��
			// �޽����� �߻��������� ���� �޽����� �������� �ξ� �ð��� ���. �� �ð����� OS�� Idle ����


			// ������ �޽��� ��� ����� ������� ����ȭ�Ǿ �����찡 focusing �Ǿ�߸� OS���� �߻��ϴ� �޽��� ó������ 

			// �񵿱� �Լ��� ����ϸ� �����찡 focusing �����ʾƵ� OS���� �߻��ϴ� �޽����� Ȯ���Ҽ� �ִ�. 
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}
	return;
}

//Frame()�Լ��� ���ø����̼��� ��� �۾��� ó���Ǵ� ���̴�.
//���� ���ø����̼��� �Ը� Ŀ���ٸ� �� �κе� ���� Ŀ���� �ȴ�.
bool SystemClass::Frame() {
	bool result;

	// Winapi�� �̿��ؼ� ���콺 ��ǥ�� �޾ƿ�
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


//������ â�� ���� ������ ������ ���� - ������ ��� - ������ ���� - ������ ���
//InitializeWindows �Լ��� �츮�� ������ �ϰ� �� �����츦 ����� �ڵ尡 ����. 
//�� �Լ��� ȣ���� �Լ�(Initialize)���Է� screenWidth�� ScreenHeight ���� �ٽ� �ǵ����ֹǷ� �� ���� ���ø����̼ǿ��� Ȱ���� �� �ִ�.
//����� ��İ� �ƹ� ��輱�� ���� �⺻ ���� �����츦 �����. 
//����� FULL_SCREEN�� ���� ���� ���� �����찡 �ɼ���, �ƴϸ� Ǯ ��ũ���� �����쵵 ���� �� �ִ�.(false�� 800x600�� â�� ������� ȭ�� ��� ��ġ�ȴ�.)
//FULL SCREEN ���������� graphicsclass.h ���� ������ִ�.
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// �ܺ� �����͸� �� ��ü�� �����Ѵ�.
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����´�.
	m_hInstance = GetModuleHandle(NULL);

	// ���ø����̼��� �̸��� �����Ѵ�.

	// ������ Ŭ������ �⺻ �������� ����
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


	// ������ Ŭ������ ����Ѵ�. 
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �˾ƿ´�.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Ǯ��ũ�� ��� ������ ���� ���� ȭ���� �����Ѵ�.
	if (FULL_SCREEN) {
		// ���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ�鿡 ���߰� ������ 32bit�� �����Ѵ�.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ���� �´� ���÷��̸� �����Ѵ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// �������� ��ġ�� ȭ���� ���� ���� �����.
		posX = posY = 0;
	}
	else {
		// ������ ����� 800x600�� ũ�⸦ ������ �Ѵ�.
		screenWidth = 1600;
		screenHeight = 900;

		// â�� ����� �߾ӿ� �������Ѵ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	DWORD wcStyle = WS_SYSMENU;
	// ������ ���� ������ â�� ����� �� �ڵ��� �����´�.
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, WINDOW_NAME, WINDOW_NAME, wcStyle, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);
	//m_hwnd = CreateWindow(m_applicationName, m_applcationName, wcStyle, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//�����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ش�.
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	SetFocus(m_hWnd);


	return;
}

//ShutdownWindow()�� ȭ�� ������ �ǵ����� �� �ڵ���� ��ȯ�Ѵ�.
void SystemClass::ShutdownWindows() {
	// ���콺 Ŀ���� ǥ���Ѵ�.
	ShowCursor(true);

	// Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲ۴�.
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����Ѵ�.
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	// ���ø����̼� �ν��Ͻ��� �����Ѵ�.
	UnregisterClass(WINDOW_NAME, m_hInstance);
	m_hInstance = NULL;

	// �� Ŭ������ ���� �ܺ� ������ ������ �����Ѵ�.
	ApplicationHandle = NULL;

	return;
}

//extern LRESULT CALLBACK ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)

//WndProc �Լ��� ������ �ý��ۿ��� �޼����� ������ ���̴�.
//�̴� SystemClass���� �������� �޼����� ����ä�� ������ �ϰ�, �̰����� �ڵ� ���� �����ϰ� ������ش�.
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {

	//if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam)) {
	//	return true;
	//}

	switch (umessage) 
	{
		//�����찡 ���ŵǾ����� Ȯ���Ѵ�.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		// �����찡 �������� Ȯ���Ѵ�.
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
		// ������ �޽����� �ʹ� ���Ƽ� �������� ����Ʈ ó���Լ� ���
		default: {
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}