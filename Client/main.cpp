#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

#include "pch.h"
#include "SystemClass.h"


// Main �Լ� ȣ���Ҷ� �տ� �Ű������� _In_ ���� ���ڰ� �ٴ´�. �׳� �ΰ����� ������� ����
// �ƹ��ǹ� ������ �����찡 �����Ǵٺ��� �������������� �ǹ̰� �־����� ������ ������ �����Ҽ� ��� �״�� ����ϴ� ��
int WINAPI WinMain(HINSTANCE hInstance, 
	/* ���μ����� ���۵Ǵ� �޸� �ּ� ���α׷��� �ν��ͽ̵ɶ����� ������ */
	/* -> ���� ������� ������ ���α׷��̴��� ������ hInstance���� ���� */
	/* ���� ���μ����� �޸𸮴� ���������� �и��Ǿ� ������ �����찡 �������� ����޸𸮸� ����ϱ� ������ ������ �ּҰ� ����ڿ��� ������ */
	/* ���� �ٸ� ��(���μ���)�� �ٸ� ������ ���������� ������ ��(������ ���μ���)���忡�� ���� ���� ���� ��(����޸�)�� �����ϰ� ��Ī�Ҽ� ���� */
	/* �̰Ͷ����� ���� �������� �޸� �ּҸ� �� ���� ����. */
	HINSTANCE hPrevInstance, 
	PSTR pScmdline, 
	int iCmdshow) {

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(215);  // �޸� ������ �߻������� �ش� ���� �ߴ��� �ɾ��� 

	SystemClass* System;
	bool result;



	// �ý��� ��ü�� �����Ѵ�.
	System = new SystemClass;
	if (!System) {
		return 0;
	}

	// ������ �ý��� ��ü�� �ʱ�ȭ�Ѵ�.
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	// �ý��� ��ü�� �޸𸮿��� �����Ѵ�.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}