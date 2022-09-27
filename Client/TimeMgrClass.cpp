#include "pch.h"
#include "TimeMgrClass.h"

TimeMgrClass::TimeMgrClass()
	: m_llPrevcount{}
	, m_llCurcount{}
	, m_llFrequency{}
	, m_dDT(0.)
	, m_dAcc(0.)
	, m_iCallCount(0)
{

}

TimeMgrClass::~TimeMgrClass() {

}

bool TimeMgrClass::Initialize()
{
	// GetTickCount 함수는 정확하지않고 단위가 1000(1초)라서 프레임당 시간을 세기에는 너무 크다.
	// GetTickCount 함수는 1초가 벌어지면 카운트 1000이 차이가 나는 고정값이 있음

	// QueryPerformacneCounter 함수는 1초가 벌어지면 카운트 값이 얼마나 벌어지는지 구해야함
	QueryPerformanceCounter(&m_llPrevcount);  // 이전 카운트 
	QueryPerformanceFrequency(&m_llFrequency);  // 초당 카운트 가능횟수 -> 1초에 이만큼 셀수 있어!!

	return true;
}

bool TimeMgrClass::Frame(HWND _hWnd)
{
	QueryPerformanceCounter(&m_llCurcount);  // 현재 카운트 

	// 이전 프레임의 카운팅과, 현재 프레임 카운팅 값의 차이를 구한다. -> 한 프레임동안 센 카운팅 수 
	// 한 프레임동안 센 카운팅 수를 초당 셀수 있는 카운팅 수로 나누면 한 프레임동안 걸리는 시간이 나옴
	m_dDT = (double)(m_llCurcount.QuadPart - m_llPrevcount.QuadPart) / (double)m_llFrequency.QuadPart;

	// m_dDT를 역수 취하면 FPS가 나옴 
	// 1. / m_dDT;  // FPS를 이렇게 측정하면 불안정함 -> m_dDT가 너무 미세한 시간이라 요동이 심함, 직접 계산하는게 정확


	// 이전 카운트 값을 현재 값으로 갱신(다음번 계산을 위해서)
	m_llPrevcount = m_llCurcount;

	++m_iCallCount;
	m_dAcc += m_dDT;  // DT 누적  -> 한 프레임당 걸린 시간을 계속 누적하면 현재 흐른 시간이 된다. 
	// 누적 시간을 1초에 한번 체크 
	if (m_dAcc >= 1.) {
		m_iFPS = m_iCallCount;
		m_dAcc = 0.;
		m_iCallCount = 0;

		wchar_t szBuffer[255] = {};
		swprintf_s(szBuffer, L"hoguengine ( FPS : %d, DT : %f )", m_iFPS, m_dDT);
		SetWindowText(_hWnd, szBuffer);
	}


	// 이렇게 함으로써 프로그램이 빨리 돌고와는 상관없이 각 프레임에서 흐른 시간동안만 일함 
	// A는 1초에 100 프레임 돌리니 한 프레임에 1/100의 작업만 하겠다. B는 1초에 200프레임이 도니 한프레임에 1/200의 작업만 하겠다.
	// 1초가 지나면 A와 B가 동일한 작업 결과를 보여줌 

	return true;
}
