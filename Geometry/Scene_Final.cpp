#include "pch.h"
#include "Scene_Final.h"
#include "KeyMgrClass.h"
#include "SceneMgrClass.h"
#include "CameraClass.h"
#include "LightClass.h"

Scene_Final::Scene_Final()
{
}

Scene_Final::Scene_Final(const Scene_Final&)
{
}

Scene_Final::~Scene_Final()
{
}

bool Scene_Final::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_iScreenWidth = screenWidth;
	m_iScreenHeight = screenHeight;
	m_hWnd = hwnd;

	m_allNode = new vector<ModelNode*>;

	return false;
}

void Scene_Final::Shutdown()
{
}

void Scene_Final::Frame()
{
}

void Scene_Final::Render()
{
}

void Scene_Final::Enter()
{
	cout << "Success to enter Final_Scene" << endl;
}

void Scene_Final::Exit()
{
	cout << "Success to exit Final_Scene" << endl;
}
