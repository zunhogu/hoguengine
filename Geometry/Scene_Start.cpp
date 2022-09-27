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

	//// ī�޶��� ��ġ�� ���� �����
	//tCamera->UpdateViewMatrix();

	//// ������� ��������� ���� ������� ���纻�� ������ �� �ִ�.
	//// ���� D3DClass ��ü�� ���� ������İ� ������ĵ� �����ؿ´�.
	//tCamera->GetViewMatrix(m_ViewMatrix);

	//// �������
	//m_ProjectionMatrix = GraphicsClass::GetInst()->GetProjectionMatrix();

	//// Render �Լ��� ū ��ȭ�� �� �����Ӹ��� �� ��Ŀ� �ٰ��Ͽ� �þ� ���������� �����ϴ� ���̴�.
	//// �� ���� ������ ������� �ٲ�ų� �츮�� �������� Ȯ���� ���� ������ �߻��Ѵ�.
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