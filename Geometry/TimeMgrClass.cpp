#include "pch.h"
#include "TimeMgrClass.h"
#include "Core.h"

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
	// GetTickCount �Լ��� ��Ȯ�����ʰ� ������ 1000(1��)�� �����Ӵ� �ð��� ���⿡�� �ʹ� ũ��.
	// GetTickCount �Լ��� 1�ʰ� �������� ī��Ʈ 1000�� ���̰� ���� �������� ����

	// QueryPerformacneCounter �Լ��� 1�ʰ� �������� ī��Ʈ ���� �󸶳� ���������� ���ؾ���
	QueryPerformanceCounter(&m_llPrevcount);  // ���� ī��Ʈ 
	QueryPerformanceFrequency(&m_llFrequency);  // �ʴ� ī��Ʈ ����Ƚ�� -> 1�ʿ� �̸�ŭ ���� �־�!!


	// CPU ��뷮 �ʱ�ȭ

	PDH_STATUS status;

	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	m_canReadCpu = true;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
		m_canReadCpu = false;

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
		m_canReadCpu = false;

	// Initialize the start time and cpu usage.
	m_lastSampleTime = GetTickCount();
	m_cpuUsage = 0;

	return true;
}

void TimeMgrClass::Shutdown()
{
	if (m_canReadCpu)
		PdhCloseQuery(m_queryHandle);

	return;
}

bool TimeMgrClass::Frame(HWND _hWnd)
{
	QueryPerformanceCounter(&m_llCurcount);  // ���� ī��Ʈ 

	// ���� �������� ī���ð�, ���� ������ ī���� ���� ���̸� ���Ѵ�. -> �� �����ӵ��� �� ī���� �� 
	// �� �����ӵ��� �� ī���� ���� �ʴ� ���� �ִ� ī���� ���� ������ �� �����ӵ��� �ɸ��� �ð��� ����
	m_dDT = (double)(m_llCurcount.QuadPart - m_llPrevcount.QuadPart) / (double)m_llFrequency.QuadPart;

	// m_dDT�� ���� ���ϸ� FPS�� ���� 
	// 1. / m_dDT;  // FPS�� �̷��� �����ϸ� �Ҿ����� -> m_dDT�� �ʹ� �̼��� �ð��̶� �䵿�� ����, ���� ����ϴ°� ��Ȯ


	// ���� ī��Ʈ ���� ���� ������ ����(������ ����� ���ؼ�)
	m_llPrevcount = m_llCurcount;


	// CPU ��뷮 üũ 
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		// If it has been 1 second then update the current cpu usage and reset the 1 second timer again.
		if ((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}


	++m_iCallCount;
	m_dAcc += m_dDT;  // DT ����  -> �� �����Ӵ� �ɸ� �ð��� ��� �����ϸ� ���� �帥 �ð��� �ȴ�. 
	// ���� �ð��� 1�ʿ� �ѹ� üũ 
	if (m_dAcc >= 1.) {
		m_iFPS = m_iCallCount;
		m_dAcc = 0.;
		m_iCallCount = 0;

		wchar_t szBuffer[255] = {};
		swprintf_s(szBuffer, L"hoguengine ( FPS : %d, CPU : %d )", m_iFPS, m_cpuUsage);
		SetWindowText(_hWnd, Utility::GetInst()->ConvWcharTochar(szBuffer));
	}


	// �̷��� �����ν� ���α׷��� ���� ����ʹ� ������� �� �����ӿ��� �帥 �ð����ȸ� ���� 
	// A�� 1�ʿ� 100 ������ ������ �� �����ӿ� 1/100�� �۾��� �ϰڴ�. B�� 1�ʿ� 200�������� ���� �������ӿ� 1/200�� �۾��� �ϰڴ�.
	// 1�ʰ� ������ A�� B�� ������ �۾� ����� ������ 

	return true;
}
