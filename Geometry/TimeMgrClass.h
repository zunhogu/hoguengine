#pragma once
#pragma comment(lib, "pdh.lib")


//////////////
// INCLUDES //
//////////////
#include <pdh.h>
// CTimeMgr는 다양한 컴퓨터 사양에 대해서 렌더링 속도를 현실시간과 동일하게 해줌 
// 이동량 * 1 / 프레임 (한 프레임에 걸리는 실제 시간)

class TimeMgrClass
{
	// FPS -> 프레임은 실시간으로 바뀔수 있기때문에 매순간 측정해야함
	LARGE_INTEGER m_llPrevcount;   // 이전 카운트 값
	LARGE_INTEGER m_llCurcount;   // 현재 카운트 값 
	LARGE_INTEGER m_llFrequency;   // 1초에 몇초 카운트 가능한지 

	UINT m_iCallCount;  // 초당 호출 함수 
	UINT m_iFPS;   // 초당 호출 횟수

	double m_dDT;  // 한 프레임에 걸린 시간
	double m_dAcc;  // 누적된 시간

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

