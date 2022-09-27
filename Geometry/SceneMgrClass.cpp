
#include "pch.h"
#include "SceneMgrClass.h"
#include "Scene_Tool.h"
#include "Scene_Start.h"
#include "Scene_Final.h"


SceneMgrClass::SceneMgrClass()
	:m_pCurScene(nullptr)
	, m_arrScene{}
{
}

SceneMgrClass::~SceneMgrClass()
{
}

bool SceneMgrClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_hWnd = hwnd;
	m_iScreenWidth = screenWidth;
	m_iScreenHeigth = screenHeight;

	// Scene 생성
	m_arrScene.push_back(new Scene_Tool);
	m_arrScene[(UINT)SCENE_TYPE::TOOL]->SetName(L"Tool Scene");
	m_arrScene[(UINT)SCENE_TYPE::TOOL]->Initialize(screenWidth, screenHeight, hwnd);

	CreateScene((UINT)SCENE_TYPE::SCNENE1, L"Start Scene");
	CreateScene((UINT)SCENE_TYPE::SCNENE2, L"Final Scene");

	// 현재 Scene 설정
	SetCurScene((UINT)SCENE_TYPE::TOOL, screenWidth, screenHeight, hwnd);

	return true;
}

void SceneMgrClass::Shutdown()
{
	for (UINT i = 0; i < (UINT)SCENE_TYPE::END; ++i)
	{
		if (nullptr != m_arrScene[i])
		{
			m_arrScene[i]->Shutdown();
			delete m_arrScene[i];
			m_arrScene[i] = 0;
		}
	}
}


const wstring SceneMgrClass::GetCurSceneName()
{
	return m_pCurScene->GetName();
}

void SceneMgrClass::Frame()
{
	// 현재 Scene을 업데이트
	if (m_pCurScene != nullptr)
	{
		m_pCurScene->Frame();
	}
}

void SceneMgrClass::Render()
{
	// 현재 Scene을 렌더링
	if (m_pCurScene != nullptr)
	{
		m_pCurScene->Render();
	}
}

bool SceneMgrClass::SetCurScene(UINT _SType, int _screenWidth, int _screenHeight, HWND _hwnd)
{
	m_pCurScene = m_arrScene[_SType];
	m_pCurScene->Enter();  // 씬 진입 
	return true;
}

UINT SceneMgrClass::GetCurSceneIndex(Scene* currScene)
{
	for (int i = 0; i < m_arrScene.size(); i++)
	{
		if (m_arrScene[i] == currScene)
		{
			return i;
		}
	}
	return -1;
}

void SceneMgrClass::CreateScene(UINT _sceneNum, const wstring& _sceneName)
{
	switch (_sceneNum) {
		case (UINT)SCENE_TYPE::SCNENE1:
		{
			m_arrScene.push_back(new Scene_Start);
			break;
		}
		case (UINT)SCENE_TYPE::SCNENE2:
		{
			m_arrScene.push_back(new Scene_Final);
			break;
		}
	}

	m_arrScene[m_arrScene.size() - 1]->SetName(_sceneName);
	m_arrScene[m_arrScene.size() - 1]->Initialize(m_iScreenWidth, m_iScreenHeigth, m_hWnd);
}


