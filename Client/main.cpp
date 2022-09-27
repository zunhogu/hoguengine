#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

#include "pch.h"
#include "SystemClass.h"


// Main 함수 호출할때 앞에 매개변수로 _In_ 등의 인자가 붙는다. 그냥 부가적인 정보라고 생각
// 아무의미 없지만 윈도우가 오래되다보니 이전버전에서는 의미가 있었지만 윈도우 구조를 변경할수 없어서 그대로 사용하는 것
int WINAPI WinMain(HINSTANCE hInstance, 
	/* 프로세스가 시작되는 메모리 주소 프로그램이 인스터싱될때마다 생성됨 */
	/* -> 현재 윈도우는 동일한 프로그램이더라도 동일한 hInstance값이 나옴 */
	/* 실제 프로세스의 메모리는 물리적으로 분리되어 있지만 윈도우가 독립적인 가상메모리를 사용하기 때문에 동일한 주소가 사용자에게 보여짐 */
	/* 서로 다른 섬(프로세스)는 다른 공간에 존재하지만 각각의 섬(각각의 프로세스)입장에서 보면 섬의 왼쪽 끝(가상메모리)는 동일하게 지칭할수 있음 */
	/* 이것때문에 실제 물리적인 메모리 주소를 알 수는 없다. */
	HINSTANCE hPrevInstance, 
	PSTR pScmdline, 
	int iCmdshow) {

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(215);  // 메모리 누수가 발생했을때 해당 지점 중단점 걸어줌 

	SystemClass* System;
	bool result;



	// 시스템 객체를 생성한다.
	System = new SystemClass;
	if (!System) {
		return 0;
	}

	// 생성한 시스템 객체를 초기화한다.
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	// 시스템 객체를 메모리에서 정리한다.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}