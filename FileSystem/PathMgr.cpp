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
	// 배포될 프로젝트의 최종 폴더는 Release 방식으로 디버그한 출력 디렉토리다.
	// CPathMgr는 출력디렉토리까지의 경로를 찾아준다.

	// 프로젝트가 실행됬을때 현재 디렉토리
	GetCurrentDirectoryW(255, m_szContentPath);

	int iLen = wcslen(m_szContentPath);


	// 상위 폴더로
	for (int i = iLen - 1; i >= 0; i--)
	{
		if (m_szContentPath[i] == '\\')
		{
			m_szContentPath[i] = '\0';
			break;
		}
	}
	wcscat_s(m_szContentPath, 255, L"\\bin\\");


	// visual studio의 F5, Ctrl+F5로 실행하게 되면 visual studio가 실행되고 프로젝트가 그 위에서 올라가기 때문에 현재 디렉토리를 프로젝트 경로로 잡히게 된다.
	// 프로젝트 속성에서 디버깅-> 작업 디렉터리 수정
	//SetWindowText(CCore::GetInst()->GetMainWnd(), m_szContentPath);
}