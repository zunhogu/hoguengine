#include "pch.h"
#include "Scene_Start.h"
#include "KeyMgrClass.h"
#include "SceneMgrClass.h"
#include "CameraClass.h"
#include "LightClass.h"

Scene_Start::Scene_Start()
{
}

Scene_Start::Scene_Start(const Scene_Start&)
{
}

Scene_Start::~Scene_Start()
{
}

bool Scene_Start::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_iScreenWidth = screenWidth;
	m_iScreenHeight = screenHeight;
	m_hWnd = hwnd;

	m_allNode = new vector<ModelNode*>;

	return false;
}

void Scene_Start::Shutdown()
{
}

void Scene_Start::Frame()
{

	//if (KEY_TAP(DIK_F5))
	//{
	//	Exit();
	//	SceneMgrClass::GetInst()->SetCurScene((UINT)SCENE_TYPE::TOOL, m_iScreenWidth, m_iScreenHeight, m_hWnd);

	//	return;
	//}

	//CameraClass* tCamera = (CameraClass*)m_arrObj[(UINT)GROUP_TYPE::CAMERA][0];

	//// 카메라의 위치를 토대로 뷰행렬
	//tCamera->UpdateViewMatrix();

	//// 뷰행령이 만들어지고 나면 뷰행렬의 복사본을 가져올 수 있다.
	//// 또한 D3DClass 객체로 부터 월드행렬과 투영행렬도 복사해온다.
	//tCamera->GetViewMatrix(m_ViewMatrix);

	//// 투영행렬
	//m_ProjectionMatrix = GraphicsClass::GetInst()->GetProjectionMatrix();

	//// Render 함수의 큰 변화는 매 프레임마다 뷰 행렬에 근거하여 시야 프러스텀을 구축하는 것이다.
	//// 이 구축 과정은 뷰행렬이 바뀌거나 우리의 프러스텀 확인이 맞지 않을때 발생한다.
	//CollisionClass::GetInst()->ConstructFrustum(SCREEN_DEPTH, m_ProjectionMatrix, m_ViewMatrix);
}

void Scene_Start::Render()
{
}

void Scene_Start::Enter()
{
	cout << "Success to enter Start_Scene" << endl;
}

void Scene_Start::Exit()
{
	cout << "Success to exit Start_Scene" << endl;
}

void Save(FILE* _pFile)
{

}

void Load(FILE* _pFile)
{

}