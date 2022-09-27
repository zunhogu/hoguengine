// Direct input을 사용하려면 헤더에 그 버전을 define으로 선언한다.
// 그렇지 않으면 컴파일러는 기본값인 버전 8로 간다는 메세지를 만들어낸다.
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib") 
#pragma comment(lib, "dxguid.lib")

enum KEY_STATE {
	NONE,  // 현재 프레임에 눌리지 않았고 이전 프레임에도 눌리지 않은 상태
	TAP,  // 막 누른 시점
	HOLD,  // 누르고 유지
	AWAY,  // 막 뗀 시점
};

struct tKeyInfo {
	//KEY eKey;   // 벡터에서 인덱스가 키의 값이라서 없어도 댐
	KEY_STATE eState;  // 키의 상태 값
	bool bPrevPush; // 이전 프레임에 키가 눌렸는지?
};

#include <dinput.h>
class KeyMgrClass {
private:
	// 키보드 장치, 마우스 장치에 대한 인터페이스
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	HWND m_hwnd;

	// 현재 키보드와 마우스의 상태를 기록하는데 사용되는 변수
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	vector<tKeyInfo> m_vecKey;
	vector<tKeyInfo> m_vecMouse;

	int m_iScreenWidth, m_iScreenHeigth;
	int m_mouseX, m_mouseY, m_mouseZ;
	int m_previousX, m_previousY, m_previousZ;

public:
	SINGLE(KeyMgrClass);
	KeyMgrClass();
	KeyMgrClass(const KeyMgrClass&);
	~KeyMgrClass();

public:

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame(POINT pt);


	KEY_STATE GetKeyboardState(int key);
	KEY_STATE GetMouseState(int key);

	XMFLOAT3 GetMousePos() { return XMFLOAT3((float)m_mouseX, (float)m_mouseY, (float)m_mouseZ); }
	XMFLOAT3 GetPrevMousePos() { return XMFLOAT3((float)m_previousX, (float)m_previousY, (float)m_previousZ); }

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput(POINT pt);
};