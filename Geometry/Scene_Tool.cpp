#include "pch.h"
#include "Scene_Tool.h"
#include "KeyMgrClass.h"
#include "SceneMgrClass.h"
#include "ResMgrClass.h"
#include "TimeMgrClass.h"
#include "ModelNode.h"
#include "ModelInfoComp.h"
#include "Prefab.h"
#include "Animation.h"
#include "TerrainComp.h"

// 선택된 Node들
vector<ModelNode*> g_selectedNodes;
ModelNode* g_copyModelNodeBuffer = nullptr;

Scene_Tool::Scene_Tool()
{
	m_Starfe = 0;
	m_Walk = 0;
	m_sCurProjectPath = L"";
}

Scene_Tool::~Scene_Tool()
{
}

bool Scene_Tool::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result = true;

	m_iScreenWidth = screenWidth;
	m_iScreenHeight = screenHeight;
	m_hWnd = hwnd;

	m_Camera = new CameraClass(XMFLOAT3(0.0f, 5.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	if (!m_Camera)
		return false;
	m_Camera->UpdateViewMatrix();

	m_Light = new LightClass(XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.05f, 0.05f, 0.05f, 0.05f));
	if (!m_Light)
		return false;

	m_Grid = new GridClass;
	if (!m_Grid)
		return false;

	result = m_Grid->Initialize(Core::GetDevice());
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the Grid in Scene Tool.", "Error", MB_OK);
		return false;
	}

	m_GridQuadTree = new GridQuadTreeClass;
	if (!m_GridQuadTree)
		return false;

	result = m_GridQuadTree->Initialize(m_Grid, Core::GetDevice());
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the Grid Quad Tree in Scene Tool.", "Error", MB_OK);
		return false;
	}

	m_SkyDome = new SkyDomeClass;
	result = m_SkyDome->Initialize(Core::GetDevice());
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the Sky Dome in Scene Tool.", "Error", MB_OK);
		return false;
	}

	m_allNode = new vector<ModelNode*>;

	return result;
}

void Scene_Tool::Shutdown()
{
	if (m_GridQuadTree)
	{
		m_GridQuadTree->Shutdown();
		delete m_GridQuadTree;
		m_GridQuadTree = 0;
	}

	if (m_Grid)
	{
		m_Grid->Shutdown();
		delete m_Grid;
		m_Grid = 0;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
}

void Scene_Tool::Frame()
{
	// 임시 초기화 코드 
	static bool init = false;
	if (!init)
	{
		GetScene((UINT)SCENE_TYPE::SCNENE1);

		ResMgrClass::GetInst()->LoadPrefab(L"i_cube", L"contents\\assets\\id_cube\\id_cube.pref");
		ResMgrClass::GetInst()->LoadPrefab(L"id_sphere", L"contents\\assets\\id_sphere\\id_sphere.pref");
		ResMgrClass::GetInst()->LoadPrefab(L"id_cone", L"contents\\assets\\id_cone\\id_cone.pref");
		ResMgrClass::GetInst()->LoadPrefab(L"id_cylinder", L"contents\\assets\\id_cylinder\\id_cylinder.pref");
		ResMgrClass::GetInst()->LoadPrefab(L"id_plane", L"contents\\assets\\id_plane\\id_plane.pref");

		ModelNode* empty = new ModelNode;
		empty->AddModelComp(new ModelInfoComp);
		empty->AddModelComp(new TransformComp);
		TerrainComp* terrain = new TerrainComp;
		terrain->Initialize(empty);
		empty->AddModelComp(terrain);
		empty->AddPathToRootNode(empty);
		SceneMgrClass::GetInst()->GetCurScene()->AddNodeToScene(empty);

		//Prefab* dino = ResMgrClass::GetInst()->LoadPrefab(L"Ankylosaurus.pref", L"contents\\assets\\Ankylosaurus\\Ankylosaurus.pref");
		//ModelNode* node = new ModelNode(*dino->GetModelNode());
		//AddNodeToScene(node);

		//AnimationComp* animationComp = new AnimationComp;

		//Animation* anim1 = ResMgrClass::GetInst()->LoadAnimation(L"Ankylosaurus_Idle.pref", L"contents\\assets\\Ankylosaurus\\animation\\Idle.anim");
		//animationComp->AddAnimation(anim1);

		//Animation* anim2 = ResMgrClass::GetInst()->LoadAnimation(L"Ankylosaurus_Walk.pref", L"contents\\assets\\Ankylosaurus\\animation\\Walk.anim");
		//animationComp->AddAnimation(anim2);

		//Animation* anim3 = ResMgrClass::GetInst()->LoadAnimation(L"Ankylosaurus_Turn.pref", L"contents\\assets\\Ankylosaurus\\animation\\Turn.anim");
		//animationComp->AddAnimation(anim3);

		//node->AddModelComp(animationComp);


		//Prefab* man = ResMgrClass::GetInst()->LoadPrefab(L"man.pref", L"contents\\assets\\man\\man.pref");
		//ModelNode* node = new ModelNode(*man->GetModelNode());
		//AddNodeToScene(node);

		//Animation* anim = ResMgrClass::GetInst()->LoadAnimation(L"anim.pref", L"contents\\assets\\man\\animation\\mixamo.com.anim");
		//AnimationComp* animationComp = new AnimationComp;
		//animationComp->AddAnimation(anim);

		//node->AddModelComp(animationComp);

		init = true;
	}

	if ((KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_N)))
	{
		m_sCurProjectPath = L"";
	}

	if ((KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_S)))
	{

		if (m_sCurProjectPath != L"")
		{
			SaveProject(m_sCurProjectPath);
		}
		else
		{
			OpenSaveProjectDialog();
		}
	}

	if ((KEY_HOLD(DIK_LCONTROL) && KEY_HOLD(DIK_LSHIFT) && KEY_TAP(DIK_S)))
	{
		OpenSaveProjectDialog();
	}

	if ((KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_O)))
	{
		OpenLoadProjectDialog();
	}

	//
	ImGuiUpdate();
	ViewPortLogic();
	HierachyLogic();
	ResourceLogic();
}

void Scene_Tool::Render()
{
	XMFLOAT3 cameraPos = m_Camera->GetPosition();// 카메라 위치

	// Render 함수의 큰 변화는 매 프레임마다 뷰 행렬에 근거하여 시야 프러스텀을 구축하는 것이다.
	// 이 구축 과정은 뷰행렬이 바뀌거나 우리의 프러스텀 확인이 맞지 않을때 발생한다.

	CollisionClass::GetInst()->ConstructFrustum(SCREEN_DEPTH, Core::GetProjectionMatrix(), m_Camera->GetViewMatrix());

	GraphicsClass::GetInst()->BeginScene(0.45f, 0.55f, 0.60f, 1.00f);

	// ImGui Rendering
	ImGuIRenderClass::GetInst()->RenderMain();

	GraphicsClass::GetInst()->RenderToTextureStart();

	// SkyDome Rendering
	GraphicsClass::GetInst()->TurnOffCulling();
	GraphicsClass::GetInst()->TurnZBufferOff();

	m_SkyDome->Render(Core::GetDeviceContext());
	GraphicsClass::GetInst()->RenderSkyDome(Core::GetDeviceContext(), m_SkyDome->GetIndexCount(), XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z), m_Camera->GetViewMatrix(), m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	GraphicsClass::GetInst()->TurnOnCulling();
	GraphicsClass::GetInst()->TurnZBufferOn();

	
	// Grid Rendering
	//GraphicsClass::GetInst()->RenderGridShaderSetParam(Core::GetDeviceContext(), m_Grid->GetWorldMatrix(), m_Camera->GetViewMatrix());

	//m_GridQuadTree->Render(Core::GetDeviceContext(), m_Grid->GetWorldMatrix());

	// Model Rendering

	for (int i = 0; i < m_allNode->size(); ++i)
	{
		ModelNode* node = m_allNode->at(i);

		node->Render(m_Camera->GetViewMatrix(), cameraPos, m_Light->GetDiffuseColor(), m_Light->GetPosition());

	}
	GraphicsClass::GetInst()->RenderToTextureEnd();

	GraphicsClass::GetInst()->EndScene();
}

void Scene_Tool::Enter()
{
	cout << "Success to enter Tool_Scene" << endl;
}

void Scene_Tool::Exit()
{
	cout << "Success to exit Tool_Scene" << endl;
}

void Scene_Tool::ViewPortLogic()
{
	// 키보드 입력에 대한 처리
	CameraMoveKeyBoard();

	XMFLOAT3 mousePos = KeyMgrClass::GetInst()->GetMousePos();
	XMFLOAT3 prevMousePos = KeyMgrClass::GetInst()->GetPrevMousePos();
	XMFLOAT2 StrafeAndWalk = XMFLOAT2(m_Starfe, m_Walk);
	XMFLOAT3 DisplacementMouse = XMFLOAT3(mousePos.x - prevMousePos.x,
										  mousePos.y - prevMousePos.y,
									      mousePos.z - prevMousePos.z);

	// 툴 씬에서 뷰포트가 포커싱 되지 않는다면 카메라를 움직이지 않음
	if (ImGuIRenderClass::GetInst()->GetCurFocusImGuiWindowID() == IMGUI_WINDOW_TYPE::VIEWPORT &&
		ImGuIRenderClass::GetInst()->GetMousePosInImGuiWindowID() == IMGUI_WINDOW_TYPE::VIEWPORT)
	{
		if (KEY_TAP(DIK_DELETE))  // 오브젝트 삭제
			DeleteKeyLogic();

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_C)) // 오브젝트 복사
		{
			if (!g_selectedNodes.empty())
				g_copyModelNodeBuffer = g_selectedNodes[g_selectedNodes.size() - 1];
		}

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_V))  // 오브젝트 붙여넣기
		{
			if (g_copyModelNodeBuffer != nullptr)
			{
				ModelNode* node = new ModelNode(*g_copyModelNodeBuffer);
				m_allNode->push_back(node);
			}
		}

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_E))
			ImGuIRenderClass::GetInst()->SetTransformOper(0);
		else if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_R))
			ImGuIRenderClass::GetInst()->SetTransformOper(1);
		else if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_T))
			ImGuIRenderClass::GetInst()->SetTransformOper(2);

		// 마우스 입력에 대한 처리 
		CameraMoveKeyMouse(StrafeAndWalk, DisplacementMouse);

		// 뷰포트 상에서 피킹 체크
		PickingCheckInViewPort();

	}
}

void Scene_Tool::HierachyLogic()
{
	if (ImGuIRenderClass::GetInst()->GetCurFocusImGuiWindowID() == IMGUI_WINDOW_TYPE::HIERACHY)
	{
		if (KEY_TAP(DIK_DELETE))  // 오브젝트 삭제
			DeleteKeyLogic();

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_C)) // 오브젝트 복사
		{
			if (!g_selectedNodes.empty())
				g_copyModelNodeBuffer = g_selectedNodes[g_selectedNodes.size() - 1];
		}

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_V))  // 오브젝트 붙여넣기
		{
			if (g_copyModelNodeBuffer != nullptr)
			{
				ModelNode* node = new ModelNode(*g_copyModelNodeBuffer);
				m_allNode->push_back(node);
			}
		}
	}
}

void Scene_Tool::ResourceLogic()
{
}

void Scene_Tool::CameraMoveKeyBoard()
{
	// 키보드 입력에 대한 처리를 한다.
	if ((KEY_HOLD(DIK_W)) && !(KEY_HOLD(DIK_S))) {
		m_Starfe = 1.0f;
	}
	if (KEY_HOLD(DIK_S) && !(KEY_HOLD(DIK_W))) {
		m_Starfe = -1.0f;
	}
	if (((KEY_HOLD(DIK_W)) && (KEY_HOLD(DIK_S))) || (!(KEY_HOLD(DIK_W)) & !(KEY_HOLD(DIK_S)))) {
		m_Starfe = 0.0f;
	}

	if ((KEY_HOLD(DIK_D)) && !(KEY_HOLD(DIK_A))) {
		m_Walk = 1.0f;
	}
	if (KEY_HOLD(DIK_A) && !(KEY_HOLD(DIK_D))) {
		m_Walk = -1.0f;
	}
	if (((KEY_HOLD(DIK_D)) && (KEY_HOLD(DIK_A))) || (!(KEY_HOLD(DIK_D)) & !(KEY_HOLD(DIK_A)))) {
		m_Walk = 0.0f;
	}
}

void Scene_Tool::CameraMoveKeyMouse(XMFLOAT2 StrafeAndWalk, XMFLOAT3 DisplacementMouse)
{
	float dx, dy, dz;

	// 마우스 입력에 대한 처리 
	if (MOUSE_HOLD(1)) {
		// 키보드 입력값을 가지고 카메라 이동시킨다.
		m_Camera->Strafe((StrafeAndWalk).x);
		m_Camera->Walk((StrafeAndWalk).y);

		// 마우스 입력값으로 카메라 회전시킨다.
		// 카메라를 회전시켜주기 위해서는 마우스 이동값의 변위를 라디안으로 변환시켜줘서 각도를 만들어주면 된다.
		dx = XMConvertToRadians(static_cast<float>((DisplacementMouse).x));
		dy = XMConvertToRadians(static_cast<float>((DisplacementMouse).y));

		m_Camera->Pitch(dy);
		m_Camera->RotateY(dx);
	}

	// 카메라 입력값으로 줌인, 줌아웃을 시킨다.
	dz = (DisplacementMouse).z;
	m_Camera->Zoom(dz);

	// 카메라의 위치를 토대로 뷰행렬
	m_Camera->UpdateViewMatrix();
}

void Scene_Tool::PickingCheckInViewPort()
{
	// 뷰포트상에서 왼쪽 마우스 버튼 클릭 시
		// 추가로 피킹이 진행되면 안되는 상황에 대한 조건을 추가시켜준다.
		// 애니메이션 실행되면안됨, gizmo클릭시는 안됨 
	if (MOUSE_AWAY(0) && !ImGuIRenderClass::GetInst()->IsGizmoClicked())
	{
		int tmX;
		int tmY;
		XMFLOAT3 cameraPos;

		// 카메라 위치 get
		cameraPos = m_Camera->GetPosition();

		ImGuIRenderClass::GetInst()->GetMousePosInViewPort(tmX, tmY);

		// 반직선 생성
		CollisionClass::GetInst()->SetRay(tmX, tmY, m_Camera->GetViewMatrix(), Core::GetProjectionMatrix(), cameraPos);
		XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, m_Camera->GetViewMatrix());

		float minDistance = INFINITY;

		XMVECTOR rayOrigin, rayDir;

		// Picking Test
		rayOrigin = CollisionClass::GetInst()->GetRayOrigin();  // World Sapce
		rayDir = CollisionClass::GetInst()->GetRayDir();  // View Space
		rayDir = XMVector3TransformNormal(rayDir, invViewMatrix);  // View Space to World Sapce

		rayDir = XMVector3Normalize(rayDir);

		int pickedIndex = -1;

		// node가 pick 되있지 않다면 모든 씬 안의 모든 Root Node에 대해 피킹 선별 진행
		for (int i = 0; i < m_allNode->size(); i++)
		{
			float distance = 0.0f;

			// 자신의 노드에 대한 picking test
			if (m_allNode->at(i)->CheckPicking(rayOrigin, rayDir, distance))
			{
				if (minDistance >= distance)
				{
					minDistance = distance;
					pickedIndex = i;
				}
			}

			vector<ModelNode*> childNodes = m_allNode->at(i)->GetAllChildNodes();

			for (int j = 0; j < childNodes.size(); j++)
			{
				if (childNodes[j]->CheckPicking(rayOrigin, rayDir, distance))
				{
					if (minDistance >= distance)
					{
						minDistance = distance;
						pickedIndex = i;
					}
				}
			}
		}

		if (pickedIndex != -1)
		{
			// 이미 피킹된 객체가 있다면? 
			if (g_selectedNodes.size() > 0)
			{
				// 피킹된 객체를 초기화해준다.
				for (int i = 0; i < g_selectedNodes.size(); i++)
					g_selectedNodes[i]->Clear();
			}
			g_selectedNodes.push_back(m_allNode->at(pickedIndex));
		}
		else
		{
			for (int i = g_selectedNodes.size() - 1; i >= 0; i--)
				g_selectedNodes.erase(g_selectedNodes.begin() + i);
		}
	}
}

void Scene_Tool::ImGuiUpdate()
{
	// 뷰포트 크기 변경시 재설정 해주는 코드 
	if (ImGuIRenderClass::GetInst()->IsViewPortResizing())
	{
		int viewPortWidth;
		int viewPortHeight;

		ImGuIRenderClass::GetInst()->GetViewPortSize(viewPortWidth, viewPortHeight);

		CollisionClass::GetInst()->InitializePickingClass(viewPortWidth, viewPortHeight);

		ImGuIRenderClass::GetInst()->SuccessInitViewport();
	}
	//
}

void Scene_Tool::SaveProject(const wstring& _strFilePath)
{
	//SetWindowText(m_hWnd, "저장중... ");
	//// 파일을 저장하기 위해서는 RAM 상의 메모리를 한땀한땀 읽어서 파일에 옮겨적는다.
	//// 파일에 옮겨적기 위해서는 file stream이 열려야한다.

	//FILE* pFile = nullptr;  // File 포인터 (File 이라는 놈은 커널 오브젝트 : 파일에 대한 주소를 담기위한 변수 )
	//_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");  // stream open, 바이너리모드로 쓰겠다. b 가없으면 문자형식(아스키)로 저장
	//assert(pFile);

	//// 저장 코드 

	//// 1. 프로젝트 내의 씬 수 Save
	//UINT _cnt = SceneMgrClass::GetInst()->GetSceneList()->size() - 1;
	//fwrite(&_cnt, sizeof(UINT), 1, pFile);

	//// 모든 씬 Save
	//for (UINT s = 1; s < _cnt + 1; ++s)
	//{
	//	Scene* scene = SceneMgrClass::GetInst()->GetSceneOfIndex(s);
	//	if (scene != nullptr)
	//	{
	//		scene->SaveObjects(pFile);
	//	}
	//}

	//fclose(pFile);
}

void Scene_Tool::OpenSaveProjectDialog()
{
	//char strName[256] = {};

	//// 파일 탐색기 구조체
	//OPENFILENAME ofn = {};


	//ofn.lStructSize = sizeof(OPENFILENAME);  // 구조체 사이즈
	//ofn.hwndOwner = Core::GetMainHwnd();  // 부모윈도우 핸들
	//ofn.lpstrFile = strName;  // 파일 경로
	//ofn.nMaxFile = sizeof(strName); // 파일 경로 크기 
	//ofn.lpstrFilter = "ALL\0*.*\0Project\0*.proj";  // 파일 확장자 필터
	//ofn.nFilterIndex = 1;  // 초기 필터 인덱스
	//ofn.lpstrFileTitle = nullptr;  // 수정가능한 타이틀 이름
	//ofn.nMaxFileTitle = 0;  // 최대 타이틀 이름 크기

	//wstring strTileFolder = PathMgr::GetInst()->GetContentPath();
	//strTileFolder += L"project";
	//ofn.lpstrInitialDir = Core::ConvWcharTochar(strTileFolder); // 초기경로

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//// 채워 넣은 구조체를 가지고 다이얼로그 생성 (Modal 방식 : 해당 창이 모든 포커싱을 가져가고 다른 창은 동작하지 않음) 
	//if (GetSaveFileName(&ofn))  // 저장을 누르면 true, 취소를 누르면 false 반환
	//{
	//	return SaveProject(Core::ConvCharToWchar(strName));
	//}

}

void Scene_Tool::LoadProject(const wstring& _strFilePath)
{
	//// 파일을 저장하기 위해서는 RAM 상의 메모리를 한땀한땀 읽어서 파일에 옮겨적는다.
	//// 파일에 옮겨적기 위해서는 file stream이 열려야한다.


	//if (m_sCurProjectPath != L"")  // 이미 열려있는 프로젝트가 있따?!!
	//{
	//	DeleteSceneList();
	//	m_iCurLoadedScene = -1;
	//	m_iPrevLoadedScene = -1;
	//}

	//m_sCurProjectPath = _strFilePath;
	//ImGuIRenderClass::GetInst()->SetCurProjectPath(m_sCurProjectPath);

	//FILE* pFile = nullptr;  // File 포인터 (File 이라는 놈은 커널 오브젝트 : 파일에 대한 주소를 담기위한 변수 )
	//_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");  // stream open
	//assert(pFile);

	//size_t size;

	//// 1. 씬의 수 Load

	//UINT _sceneCount;
	//fread(&_sceneCount, sizeof(UINT), 1, pFile);

	//// 모든 씬 Load
	//for (UINT s = 0; s < _sceneCount; ++s)
	//{

	//	// 2. 씬 번호 Load
	//	UINT _sceneType;
	//	fread(&_sceneType, sizeof(SCENE_TYPE), 1, pFile);

	//	size_t size;

	//	// 3. 씬 이름 Load
	//	wchar_t* _sceneName = nullptr;
	//	FREAD_WCHAR(_sceneName, size, pFile);

	//	SceneMgrClass::GetInst()->CreateScene(_sceneType, _sceneName);

	//	ImGuIRenderClass::GetInst()->AddScene(_sceneType, _sceneName);

	//	Scene* scene = SceneMgrClass::GetInst()->GetSceneOfIndex(_sceneType);
	//	if (scene != nullptr)
	//	{
	//		scene->LoadObjects(pFile);
	//	}

	//}
	//fclose(pFile);
}

void Scene_Tool::OpenLoadProjectDialog()
{
	//char strName[256] = {};

	//// 파일 탐색기 구조체
	//OPENFILENAME ofn = {};


	//ofn.lStructSize = sizeof(OPENFILENAME);  // 구조체 사이즈
	//ofn.hwndOwner = Core::GetMainHwnd();  // 부모윈도우 핸들
	//ofn.lpstrFile = strName;  // 파일 경로
	//ofn.nMaxFile = sizeof(strName); // 파일 경로 크기 
	//ofn.lpstrFilter = "ALL\0*.*\0Project\0*.proj";  // 파일 확장자 필터
	//ofn.nFilterIndex = 1;  // 초기 필터 인덱스
	//ofn.lpstrFileTitle = nullptr;  // 수정가능한 타이틀 이름
	//ofn.nMaxFileTitle = 0;  // 최대 타이틀 이름 크기

	//wstring strTileFolder = PathMgr::GetInst()->GetContentPath();
	//strTileFolder += L"project";
	//ofn.lpstrInitialDir = ConvWcharTochar(strTileFolder); // 초기경로

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//// 채워 넣은 구조체를 가지고 다이얼로그 생성 (Modal 방식 : 해당 창이 모든 포커싱을 가져가고 다른 창은 동작하지 않음) 
	//if (GetOpenFileName(&ofn))  // 저장을 누르면 true, 취소를 누르면 false 반환
	//{
	//	return LoadProject(ConvCharToWchar(strName));
	//}

}

bool Scene_Tool::GetScene(UINT _sNum)
{
	// 불러들일 Scene을 SceneMgr로 부터 받아옴
	Scene* scene = SceneMgrClass::GetInst()->GetSceneOfIndex(_sNum);
	vector<ModelNode*>* nodes = scene->GetAllNode();
	SetAllNode(nodes);

	//cout << "Success Load Scene!!" << endl;
	return true;
}

void Scene_Tool::DeleteKeyLogic()
{
	if (!g_selectedNodes.empty())
	{
		ModelNode* selectedNode = g_selectedNodes[g_selectedNodes.size() - 1];

		if (selectedNode->GetRootNode() == selectedNode)
			selectedNode->Shutdown();

		for (int i = 0; i < m_allNode->size(); i++)
		{
			if (m_allNode->at(i) == selectedNode)
			{
				m_allNode->erase(m_allNode->begin() + i);
				g_selectedNodes.clear();
				break;
			}
		}
	}
}
