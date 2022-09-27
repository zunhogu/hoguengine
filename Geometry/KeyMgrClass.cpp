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
	// Direct input의 인터페이스를 초기화한다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// 키보드 입력장치를 초기화한다.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 입력할 데이터 포맷을 세팅한다.
	// 이는 키보드에서 우리가 입력할 데이터를 미리 정의할 수 있다.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	// 키보드 협력레벨(cooperative level)을 정하는 것은 이 장치가 무엇을 하는지, 그리고 어떻게 사용될 것인지를 결정하므로 중요하다.
	// DISCL_EXCLUSIVE 플래그로 다른 프로그램들과 공유하지 않는다고 알려준다. (배제 상태)
	// 이렇게 하면 오직 이 어플리케이션만 이 입력을 감지할 수 있게 해준다.
	// 하지만 만일 이 어플리케이션이 아닌 다른 어플리케이션에도 키보드 입력에 접근가능하게 하려면 DISCL_NONEXCLUSIVE를 사용하면 된다. (비배제 상태)
	// 중요한 것은 비배제 상태이고 풀스크린이 아니라면 장치가 다른 윈도우로 포커스가 이동했는지, 그리고 다시 포커스를 얻어서 장치르 사용할 수 있게 되엇는지 확인한다는 것이다.
	// 이런 포커스를 잃어버린 경우를 예를 들면 대개 다른 윈도우가 메인 윈도우가 되어 포커스를 얻었다던가 아니면 우리 어플리케이션이 최소화된 경우를 말한다.
	result = m_keyboard->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// 키보드 세팅이 되면 Acquire 함수를 호출하여 이 포인터로 키보드에 대한 접근을 취득한다.
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////

	// 마우스 입력장치를 초기화한다.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 마우스 입력장치의 포맷을 세팅한다.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	// 마우에 대해서는 비배제 상태로 설정한다. 마우스가 포커스를 잃었는지 다시 찾았는지 확인해야한다.
	result = m_mouse->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// 마우스 세팅이 되면 Acquire 함수를 호출하여 이 포인터로 마우스 대한 접근을 취득한다.
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}


	// 키보드 상태를 초기화 
	for (int i = 0; i < 256; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}

	// 마우스 상태를 초기화
	for (int i = 0; i < 4; i++)
	{
		m_vecMouse.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}

	return  true;
}

// Shutdown 함수는 두 장치와 direct input에 대한 인터페이스를 해제한다..이 절차는 항상 Unacquire->Release 순으로 이루어진다.
void KeyMgrClass::Shutdown() {
	// 마우스 포인터 해제
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// direct input 인터페이스 해제
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}

	// 키보드 포인터 해제
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	return;
}

// Frame함수는 장치의 현재 상태를 읽어 앞서 만든 상태버퍼에 기록한다.
// 각 장치의 상태를 읽은 뒤 변경사항을 처리하게 한다.
bool KeyMgrClass::Frame(POINT pt) {
	bool result;

	// 현재 키보드의 상태를 읽는다.
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	// 현재 마우스의 상태를 읽는다.
	result = ReadMouse();
	if (!result) {
		return false;
	}

	// 마우스와 키보드의 변화를 처리한다.
	ProcessInput(pt);

	return true;
}


// ReadKeyboard 함수는 키보드의 상태를 m_keyboardState 변수에 저장한다.
// 이 상태 변수는 모든 키에 대해 현재 눌렸는지 그렇지 않은지 보여준다.
// 만약 키보드 상태를 읽는데 실패한다면 다섯가지 요인들 중 하나다.
// 여기서 다룰 두 요인은 키보드가 포커스를 잃거나 취득 불가 상태인 경우다.
// 이런 경우라면 매 프레임마다 제어권을 돌려받을 때까지 Acquire 함수를 호출해야한다
// 윈도우가 최소화된 경우 Acquire 함수가 실패하게 된다. 하지만 다시 되돌아 온 경우 Acquire 함수는 다시 키보드 상태를 읽을 수 있게 된다.
// 다른 에러 타입들은 다루지 않는다.
bool KeyMgrClass::ReadKeyboard() {
	HRESULT result;

	// 키보드 장치의 상태를 읽는다.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {
		// 키보드가 포커스를 잃거나, 취득불가인 경우 다시 Acquire함수를 매프레임마다 호출해서 제어권을 돌려받아야함
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

// ReadMouse 함수는 키보드의 상태를 읽는 ReadKeyboard 함수처럼 마우스의 상태를 읽는다.
// 하지만 마우스의 상태는 이전 프레임과 달라진 위치(변위)만을 저장한다.
// 예를 들어 마우스를 5단위만큼 오른쪽으로 이동시켰다면 화면상의 위치를 알려주지는 않는다.
// 하지만 변위(delta) 정보는 알아낼 수 있기 때문에 이를 이용하여 마우스의 위치를 자체적으로 관리할 수 있다.
bool KeyMgrClass::ReadMouse() {
	HRESULT result;

	// 마우스 장치의 상태를 읽는다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		// 마우스가 포커스를 잃거나, 취득불가인 경우 다시 Acquire함수를 매프레임마다 호출해서 제어권을 돌려받아야함
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

	// 마우스 회전 계산을 위해 이전값을 기록해둔다. 
	m_previousX = m_mouseX;
	m_previousY = m_mouseY;
	m_previousZ = m_mouseZ;

	// 마우스 변위값을 가지고 마우스 위치값을 기록한다.
	m_mouseX = pt.x;
	m_mouseY = pt.y;
	m_mouseZ += (int)m_mouseState.lZ;

	// cout << m_mouseX << ", " << m_mouseY << endl;

	// 만약 화면범위를 넘어서면 값을 고정시킨다.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_iScreenWidth) { m_mouseX = m_iScreenWidth; }
	if (m_mouseY > m_iScreenHeigth) { m_mouseY = m_iScreenHeigth; }

	if (m_hwnd != nullptr)
	{
		for (int i = 0; i < 256; ++i) {
			if (m_keyboardState[i] & 0x80)   // 현재 프레임에 키가 눌렸다?
			{
				if (m_vecKey[i].bPrevPush)
				{
					// 이전 프레임에도 눌렸다? 
					m_vecKey[i].eState = KEY_STATE::HOLD;
				}
				else {
					// 이전 프레임에 눌리지 않았다?
					m_vecKey[i].eState = KEY_STATE::TAP;
				}
				m_vecKey[i].bPrevPush = true;
			}
			else {  // 현재 프레임에 키가 안눌렸다?
				if (m_vecKey[i].bPrevPush) // 이전 프레임에 키가 눌렸다?
				{
					m_vecKey[i].eState = KEY_STATE::AWAY;
				}
				else {  // 이전에도 눌리지 않았다?
					m_vecKey[i].eState = KEY_STATE::NONE;

				}
				m_vecKey[i].bPrevPush = false;
			}
		}

		// 마우스 상태 체크
		for (int i = 0; i < 4; ++i) {
			if (m_mouseState.rgbButtons[i] & 0x80)   // 현재 프레임에 키가 눌렸다?
			{
				if (m_vecMouse[i].bPrevPush)
				{
					// 이전 프레임에도 눌렸다? 
					m_vecMouse[i].eState = KEY_STATE::HOLD;
				}
				else {
					// 이전 프레임에 눌리지 않았다?
					m_vecMouse[i].eState = KEY_STATE::TAP;
				}
				m_vecMouse[i].bPrevPush = true;
			}
			else {  // 현재 프레임에 키가 안눌렸다?
				if (m_vecMouse[i].bPrevPush) // 이전 프레임에 키가 눌렸다?
				{
					m_vecMouse[i].eState = KEY_STATE::AWAY;
				}
				else {  // 이전에도 눌리지 않았다?
					m_vecMouse[i].eState = KEY_STATE::NONE;

				}
				m_vecMouse[i].bPrevPush = false;
			}
		}
	}
	else {  // 윈도우가 포커싱이 되지 않았다면?

		 // 키보드 상태 초기화
		for (int i = 0; i < 256; ++i)
		{
			m_vecKey[i].bPrevPush = false;

			// TAP, HOLD 상태면 AWAY 상태로 보내줌, AWAY는 NONE으로 보내줌 TAP, HOLD였던 놈도 다음프레임에서는 AWAY->NONE으로 바뀜 
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

		// 마우스 상태 초기화
		for (int i = 0; i < 4; ++i)
		{
			m_vecMouse[i].bPrevPush = false;

			// TAP, HOLD 상태면 AWAY 상태로 보내줌, AWAY는 NONE으로 보내줌 TAP, HOLD였던 놈도 다음프레임에서는 AWAY->NONE으로 바뀜 
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