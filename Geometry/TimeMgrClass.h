#pragma once

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
public:
	SINGLE(TimeMgrClass);
	TimeMgrClass();
	TimeMgrClass(const TimeMgrClass&);
	~TimeMgrClass();

	bool Initialize();
	bool Frame(HWND _hWnd);

	double GetDT() { return m_dDT; }
	float GetfDT() { return (float)m_dDT; }

	UINT GetFPS() { return m_iFPS; }
};

