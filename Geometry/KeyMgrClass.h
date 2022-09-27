// Direct input�� ����Ϸ��� ����� �� ������ define���� �����Ѵ�.
// �׷��� ������ �����Ϸ��� �⺻���� ���� 8�� ���ٴ� �޼����� ������.
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib") 
#pragma comment(lib, "dxguid.lib")

enum KEY_STATE {
	NONE,  // ���� �����ӿ� ������ �ʾҰ� ���� �����ӿ��� ������ ���� ����
	TAP,  // �� ���� ����
	HOLD,  // ������ ����
	AWAY,  // �� �� ����
};

struct tKeyInfo {
	//KEY eKey;   // ���Ϳ��� �ε����� Ű�� ���̶� ��� ��
	KEY_STATE eState;  // Ű�� ���� ��
	bool bPrevPush; // ���� �����ӿ� Ű�� ���ȴ���?
};

#include <dinput.h>
class KeyMgrClass {
private:
	// Ű���� ��ġ, ���콺 ��ġ�� ���� �������̽�
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	HWND m_hwnd;

	// ���� Ű����� ���콺�� ���¸� ����ϴµ� ���Ǵ� ����
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