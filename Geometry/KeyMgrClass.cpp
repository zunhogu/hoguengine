#include "pch.h"
#include "KeyMgrClass.h"

KeyMgrClass::KeyMgrClass() {
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;

	m_mouseX = 0;
	m_mouseY = 0;
	m_mouseZ = 0;
}

KeyMgrClass::KeyMgrClass(const KeyMgrClass& other) {}

KeyMgrClass::~KeyMgrClass() {}

bool KeyMgrClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeigth) {
	HRESULT result;

	m_hwnd = hwnd;
	m_iScreenWidth = screenWidth;
	m_iScreenHeigth = screenHeigth;

	////////////////////////////////////////////////////////////////////////////////
	// Direct input�� �������̽��� �ʱ�ȭ�Ѵ�.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// Ű���� �Է���ġ�� �ʱ�ȭ�Ѵ�.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	// �Է��� ������ ������ �����Ѵ�.
	// �̴� Ű���忡�� �츮�� �Է��� �����͸� �̸� ������ �� �ִ�.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	// Ű���� ���·���(cooperative level)�� ���ϴ� ���� �� ��ġ�� ������ �ϴ���, �׸��� ��� ���� �������� �����ϹǷ� �߿��ϴ�.
	// DISCL_EXCLUSIVE �÷��׷� �ٸ� ���α׷���� �������� �ʴ´ٰ� �˷��ش�. (���� ����)
	// �̷��� �ϸ� ���� �� ���ø����̼Ǹ� �� �Է��� ������ �� �ְ� ���ش�.
	// ������ ���� �� ���ø����̼��� �ƴ� �ٸ� ���ø����̼ǿ��� Ű���� �Է¿� ���ٰ����ϰ� �Ϸ��� DISCL_NONEXCLUSIVE�� ����ϸ� �ȴ�. (����� ����)
	// �߿��� ���� ����� �����̰� Ǯ��ũ���� �ƴ϶�� ��ġ�� �ٸ� ������� ��Ŀ���� �̵��ߴ���, �׸��� �ٽ� ��Ŀ���� �� ��ġ�� ����� �� �ְ� �Ǿ����� Ȯ���Ѵٴ� ���̴�.
	// �̷� ��Ŀ���� �Ҿ���� ��츦 ���� ��� �밳 �ٸ� �����찡 ���� �����찡 �Ǿ� ��Ŀ���� ����ٴ��� �ƴϸ� �츮 ���ø����̼��� �ּ�ȭ�� ��츦 ���Ѵ�.
	result = m_keyboard->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// Ű���� ������ �Ǹ� Acquire �Լ��� ȣ���Ͽ� �� �����ͷ� Ű���忡 ���� ������ ����Ѵ�.
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// ���콺 �Է���ġ�� �ʱ�ȭ�Ѵ�.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	// ���콺 �Է���ġ�� ������ �����Ѵ�.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	// ���쿡 ���ؼ��� ����� ���·� �����Ѵ�. ���콺�� ��Ŀ���� �Ҿ����� �ٽ� ã�Ҵ��� Ȯ���ؾ��Ѵ�.
	result = m_mouse->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// ���콺 ������ �Ǹ� Acquire �Լ��� ȣ���Ͽ� �� �����ͷ� ���콺 ���� ������ ����Ѵ�.
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}


	// Ű���� ���¸� �ʱ�ȭ 
	for (int i = 0; i < 256; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}

	// ���콺 ���¸� �ʱ�ȭ
	for (int i = 0; i < 4; i++)
	{
		m_vecMouse.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}

	return  true;
}

// Shutdown �Լ��� �� ��ġ�� direct input�� ���� �������̽��� �����Ѵ�..�� ������ �׻� Unacquire->Release ������ �̷������.
void KeyMgrClass::Shutdown() {
	// ���콺 ������ ����
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// direct input �������̽� ����
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}

	// Ű���� ������ ����
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	return;
}

// Frame�Լ��� ��ġ�� ���� ���¸� �о� �ռ� ���� ���¹��ۿ� ����Ѵ�.
// �� ��ġ�� ���¸� ���� �� ��������� ó���ϰ� �Ѵ�.
bool KeyMgrClass::Frame(POINT pt) {
	bool result;

	// ���� Ű������ ���¸� �д´�.
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	// ���� ���콺�� ���¸� �д´�.
	result = ReadMouse();
	if (!result) {
		return false;
	}

	// ���콺�� Ű������ ��ȭ�� ó���Ѵ�.
	ProcessInput(pt);

	return true;
}


// ReadKeyboard �Լ��� Ű������ ���¸� m_keyboardState ������ �����Ѵ�.
// �� ���� ������ ��� Ű�� ���� ���� ���ȴ��� �׷��� ������ �����ش�.
// ���� Ű���� ���¸� �дµ� �����Ѵٸ� �ټ����� ���ε� �� �ϳ���.
// ���⼭ �ٷ� �� ������ Ű���尡 ��Ŀ���� �Ұų� ��� �Ұ� ������ ����.
// �̷� ����� �� �����Ӹ��� ������� �������� ������ Acquire �Լ��� ȣ���ؾ��Ѵ�
// �����찡 �ּ�ȭ�� ��� Acquire �Լ��� �����ϰ� �ȴ�. ������ �ٽ� �ǵ��� �� ��� Acquire �Լ��� �ٽ� Ű���� ���¸� ���� �� �ְ� �ȴ�.
// �ٸ� ���� Ÿ�Ե��� �ٷ��� �ʴ´�.
bool KeyMgrClass::ReadKeyboard() {
	HRESULT result;

	// Ű���� ��ġ�� ���¸� �д´�.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {
		// Ű���尡 ��Ŀ���� �Ұų�, ���Ұ��� ��� �ٽ� Acquire�Լ��� �������Ӹ��� ȣ���ؼ� ������� �����޾ƾ���
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

// ReadMouse �Լ��� Ű������ ���¸� �д� ReadKeyboard �Լ�ó�� ���콺�� ���¸� �д´�.
// ������ ���콺�� ���´� ���� �����Ӱ� �޶��� ��ġ(����)���� �����Ѵ�.
// ���� ��� ���콺�� 5������ŭ ���������� �̵����״ٸ� ȭ����� ��ġ�� �˷������� �ʴ´�.
// ������ ����(delta) ������ �˾Ƴ� �� �ֱ� ������ �̸� �̿��Ͽ� ���콺�� ��ġ�� ��ü������ ������ �� �ִ�.
bool KeyMgrClass::ReadMouse() {
	HRESULT result;

	// ���콺 ��ġ�� ���¸� �д´�.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		// ���콺�� ��Ŀ���� �Ұų�, ���Ұ��� ��� �ٽ� Acquire�Լ��� �������Ӹ��� ȣ���ؼ� ������� �����޾ƾ���
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			result = m_mouse->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

void KeyMgrClass::ProcessInput(POINT pt) {

	// ���콺 ȸ�� ����� ���� �������� ����صд�. 
	m_previousX = m_mouseX;
	m_previousY = m_mouseY;
	m_previousZ = m_mouseZ;

	// ���콺 �������� ������ ���콺 ��ġ���� ����Ѵ�.
	m_mouseX = pt.x;
	m_mouseY = pt.y;
	m_mouseZ += (int)m_mouseState.lZ;

	// cout << m_mouseX << ", " << m_mouseY << endl;

	// ���� ȭ������� �Ѿ�� ���� ������Ų��.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_iScreenWidth) { m_mouseX = m_iScreenWidth; }
	if (m_mouseY > m_iScreenHeigth) { m_mouseY = m_iScreenHeigth; }

	if (m_hwnd != nullptr)
	{
		for (int i = 0; i < 256; ++i) {
			if (m_keyboardState[i] & 0x80)   // ���� �����ӿ� Ű�� ���ȴ�?
			{
				if (m_vecKey[i].bPrevPush)
				{
					// ���� �����ӿ��� ���ȴ�? 
					m_vecKey[i].eState = KEY_STATE::HOLD;
				}
				else {
					// ���� �����ӿ� ������ �ʾҴ�?
					m_vecKey[i].eState = KEY_STATE::TAP;
				}
				m_vecKey[i].bPrevPush = true;
			}
			else {  // ���� �����ӿ� Ű�� �ȴ��ȴ�?
				if (m_vecKey[i].bPrevPush) // ���� �����ӿ� Ű�� ���ȴ�?
				{
					m_vecKey[i].eState = KEY_STATE::AWAY;
				}
				else {  // �������� ������ �ʾҴ�?
					m_vecKey[i].eState = KEY_STATE::NONE;

				}
				m_vecKey[i].bPrevPush = false;
			}
		}

		// ���콺 ���� üũ
		for (int i = 0; i < 4; ++i) {
			if (m_mouseState.rgbButtons[i] & 0x80)   // ���� �����ӿ� Ű�� ���ȴ�?
			{
				if (m_vecMouse[i].bPrevPush)
				{
					// ���� �����ӿ��� ���ȴ�? 
					m_vecMouse[i].eState = KEY_STATE::HOLD;
				}
				else {
					// ���� �����ӿ� ������ �ʾҴ�?
					m_vecMouse[i].eState = KEY_STATE::TAP;
				}
				m_vecMouse[i].bPrevPush = true;
			}
			else {  // ���� �����ӿ� Ű�� �ȴ��ȴ�?
				if (m_vecMouse[i].bPrevPush) // ���� �����ӿ� Ű�� ���ȴ�?
				{
					m_vecMouse[i].eState = KEY_STATE::AWAY;
				}
				else {  // �������� ������ �ʾҴ�?
					m_vecMouse[i].eState = KEY_STATE::NONE;

				}
				m_vecMouse[i].bPrevPush = false;
			}
		}
	}
	else {  // �����찡 ��Ŀ���� ���� �ʾҴٸ�?

		 // Ű���� ���� �ʱ�ȭ
		for (int i = 0; i < 256; ++i)
		{
			m_vecKey[i].bPrevPush = false;

			// TAP, HOLD ���¸� AWAY ���·� ������, AWAY�� NONE���� ������ TAP, HOLD���� �� ���������ӿ����� AWAY->NONE���� �ٲ� 
			if (KEY_STATE::TAP == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::HOLD == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::NONE;
			}
		}

		// ���콺 ���� �ʱ�ȭ
		for (int i = 0; i < 4; ++i)
		{
			m_vecMouse[i].bPrevPush = false;

			// TAP, HOLD ���¸� AWAY ���·� ������, AWAY�� NONE���� ������ TAP, HOLD���� �� ���������ӿ����� AWAY->NONE���� �ٲ� 
			if (KEY_STATE::TAP == m_vecMouse[i].eState)
			{
				m_vecMouse[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::HOLD == m_vecMouse[i].eState)
			{
				m_vecMouse[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_vecMouse[i].eState)
			{
				m_vecMouse[i].eState = KEY_STATE::NONE;
			}
		}
	}
	return;
}

KEY_STATE KeyMgrClass::GetKeyboardState(int key)
{
	return m_vecKey[key].eState;
}

KEY_STATE KeyMgrClass::GetMouseState(int key)
{
	return m_vecMouse[key].eState;
}