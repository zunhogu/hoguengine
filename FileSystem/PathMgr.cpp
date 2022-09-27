#include "pch.h"
#include "PathMgr.h"

PathMgr::PathMgr() :m_szContentPath{}
{
}

PathMgr::PathMgr(const PathMgr&)
{
}

PathMgr::~PathMgr()
{
}

void PathMgr::Initialize()
{
	// ������ ������Ʈ�� ���� ������ Release ������� ������� ��� ���丮��.
	// CPathMgr�� ��µ��丮������ ��θ� ã���ش�.

	// ������Ʈ�� ���������� ���� ���丮
	GetCurrentDirectoryW(255, m_szContentPath);

	int iLen = wcslen(m_szContentPath);


	// ���� ������
	for (int i = iLen - 1; i >= 0; i--)
	{
		if (m_szContentPath[i] == '\\')
		{
			m_szContentPath[i] = '\0';
			break;
		}
	}
	wcscat_s(m_szContentPath, 255, L"\\bin\\");


	// visual studio�� F5, Ctrl+F5�� �����ϰ� �Ǹ� visual studio�� ����ǰ� ������Ʈ�� �� ������ �ö󰡱� ������ ���� ���丮�� ������Ʈ ��η� ������ �ȴ�.
	// ������Ʈ �Ӽ����� �����-> �۾� ���͸� ����
	//SetWindowText(CCore::GetInst()->GetMainWnd(), m_szContentPath);
}