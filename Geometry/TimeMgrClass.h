#pragma once
#pragma comment(lib, "pdh.lib")


//////////////
// INCLUDES //
//////////////
#include <pdh.h>
// CTimeMgr�� �پ��� ��ǻ�� ��翡 ���ؼ� ������ �ӵ��� ���ǽð��� �����ϰ� ���� 
// �̵��� * 1 / ������ (�� �����ӿ� �ɸ��� ���� �ð�)

class TimeMgrClass
{
	// FPS -> �������� �ǽð����� �ٲ�� �ֱ⶧���� �ż��� �����ؾ���
	LARGE_INTEGER m_llPrevcount;   // ���� ī��Ʈ ��
	LARGE_INTEGER m_llCurcount;   // ���� ī��Ʈ �� 
	LARGE_INTEGER m_llFrequency;   // 1�ʿ� ���� ī��Ʈ �������� 

	UINT m_iCallCount;  // �ʴ� ȣ�� �Լ� 
	UINT m_iFPS;   // �ʴ� ȣ�� Ƚ��

	double m_dDT;  // �� �����ӿ� �ɸ� �ð�
	double m_dAcc;  // ������ �ð�

	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;

public:
	SINGLE(TimeMgrClass);
	TimeMgrClass();
	TimeMgrClass(const TimeMgrClass&);
	~TimeMgrClass();

	bool Initialize();
	void Shutdown();
	bool Frame(HWND _hWnd);

	double GetDT() { return m_dDT; }
	float GetfDT() { return (float)m_dDT; }

	UINT GetFPS() { return m_iFPS; }
};

