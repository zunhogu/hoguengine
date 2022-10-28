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

// ���õ� Node��
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
	// �ӽ� �ʱ�ȭ �ڵ� 
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
	XMFLOAT3 cameraPos = m_Camera->GetPosition();// ī�޶� ��ġ

	// Render �Լ��� ū ��ȭ�� �� �����Ӹ��� �� ��Ŀ� �ٰ��Ͽ� �þ� ���������� �����ϴ� ���̴�.
	// �� ���� ������ ������� �ٲ�ų� �츮�� �������� Ȯ���� ���� ������ �߻��Ѵ�.

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
	// Ű���� �Է¿� ���� ó��
	CameraMoveKeyBoard();

	XMFLOAT3 mousePos = KeyMgrClass::GetInst()->GetMousePos();
	XMFLOAT3 prevMousePos = KeyMgrClass::GetInst()->GetPrevMousePos();
	XMFLOAT2 StrafeAndWalk = XMFLOAT2(m_Starfe, m_Walk);
	XMFLOAT3 DisplacementMouse = XMFLOAT3(mousePos.x - prevMousePos.x,
										  mousePos.y - prevMousePos.y,
									      mousePos.z - prevMousePos.z);

	// �� ������ ����Ʈ�� ��Ŀ�� ���� �ʴ´ٸ� ī�޶� �������� ����
	if (ImGuIRenderClass::GetInst()->GetCurFocusImGuiWindowID() == IMGUI_WINDOW_TYPE::VIEWPORT &&
		ImGuIRenderClass::GetInst()->GetMousePosInImGuiWindowID() == IMGUI_WINDOW_TYPE::VIEWPORT)
	{
		if (KEY_TAP(DIK_DELETE))  // ������Ʈ ����
			DeleteKeyLogic();

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_C)) // ������Ʈ ����
		{
			if (!g_selectedNodes.empty())
				g_copyModelNodeBuffer = g_selectedNodes[g_selectedNodes.size() - 1];
		}

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_V))  // ������Ʈ �ٿ��ֱ�
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

		// ���콺 �Է¿� ���� ó�� 
		CameraMoveKeyMouse(StrafeAndWalk, DisplacementMouse);

		// ����Ʈ �󿡼� ��ŷ üũ
		PickingCheckInViewPort();

	}
}

void Scene_Tool::HierachyLogic()
{
	if (ImGuIRenderClass::GetInst()->GetCurFocusImGuiWindowID() == IMGUI_WINDOW_TYPE::HIERACHY)
	{
		if (KEY_TAP(DIK_DELETE))  // ������Ʈ ����
			DeleteKeyLogic();

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_C)) // ������Ʈ ����
		{
			if (!g_selectedNodes.empty())
				g_copyModelNodeBuffer = g_selectedNodes[g_selectedNodes.size() - 1];
		}

		if (KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_V))  // ������Ʈ �ٿ��ֱ�
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
	// Ű���� �Է¿� ���� ó���� �Ѵ�.
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

	// ���콺 �Է¿� ���� ó�� 
	if (MOUSE_HOLD(1)) {
		// Ű���� �Է°��� ������ ī�޶� �̵���Ų��.
		m_Camera->Strafe((StrafeAndWalk).x);
		m_Camera->Walk((StrafeAndWalk).y);

		// ���콺 �Է°����� ī�޶� ȸ����Ų��.
		// ī�޶� ȸ�������ֱ� ���ؼ��� ���콺 �̵����� ������ �������� ��ȯ�����༭ ������ ������ָ� �ȴ�.
		dx = XMConvertToRadians(static_cast<float>((DisplacementMouse).x));
		dy = XMConvertToRadians(static_cast<float>((DisplacementMouse).y));

		m_Camera->Pitch(dy);
		m_Camera->RotateY(dx);
	}

	// ī�޶� �Է°����� ����, �ܾƿ��� ��Ų��.
	dz = (DisplacementMouse).z;
	m_Camera->Zoom(dz);

	// ī�޶��� ��ġ�� ���� �����
	m_Camera->UpdateViewMatrix();
}

void Scene_Tool::PickingCheckInViewPort()
{
	// ����Ʈ�󿡼� ���� ���콺 ��ư Ŭ�� ��
		// �߰��� ��ŷ�� ����Ǹ� �ȵǴ� ��Ȳ�� ���� ������ �߰������ش�.
		// �ִϸ��̼� ����Ǹ�ȵ�, gizmoŬ���ô� �ȵ� 
	if (MOUSE_AWAY(0) && !ImGuIRenderClass::GetInst()->IsGizmoClicked())
	{
		int tmX;
		int tmY;
		XMFLOAT3 cameraPos;

		// ī�޶� ��ġ get
		cameraPos = m_Camera->GetPosition();

		ImGuIRenderClass::GetInst()->GetMousePosInViewPort(tmX, tmY);

		// ������ ����
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

		// node�� pick ������ �ʴٸ� ��� �� ���� ��� Root Node�� ���� ��ŷ ���� ����
		for (int i = 0; i < m_allNode->size(); i++)
		{
			float distance = 0.0f;

			// �ڽ��� ��忡 ���� picking test
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
			// �̹� ��ŷ�� ��ü�� �ִٸ�? 
			if (g_selectedNodes.size() > 0)
			{
				// ��ŷ�� ��ü�� �ʱ�ȭ���ش�.
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
	// ����Ʈ ũ�� ����� �缳�� ���ִ� �ڵ� 
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
	//SetWindowText(m_hWnd, "������... ");
	//// ������ �����ϱ� ���ؼ��� RAM ���� �޸𸮸� �Ѷ��Ѷ� �о ���Ͽ� �Ű����´�.
	//// ���Ͽ� �Ű����� ���ؼ��� file stream�� �������Ѵ�.

	//FILE* pFile = nullptr;  // File ������ (File �̶�� ���� Ŀ�� ������Ʈ : ���Ͽ� ���� �ּҸ� ������� ���� )
	//_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");  // stream open, ���̳ʸ����� ���ڴ�. b �������� ��������(�ƽ�Ű)�� ����
	//assert(pFile);

	//// ���� �ڵ� 

	//// 1. ������Ʈ ���� �� �� Save
	//UINT _cnt = SceneMgrClass::GetInst()->GetSceneList()->size() - 1;
	//fwrite(&_cnt, sizeof(UINT), 1, pFile);

	//// ��� �� Save
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

	//// ���� Ž���� ����ü
	//OPENFILENAME ofn = {};


	//ofn.lStructSize = sizeof(OPENFILENAME);  // ����ü ������
	//ofn.hwndOwner = Core::GetMainHwnd();  // �θ������� �ڵ�
	//ofn.lpstrFile = strName;  // ���� ���
	//ofn.nMaxFile = sizeof(strName); // ���� ��� ũ�� 
	//ofn.lpstrFilter = "ALL\0*.*\0Project\0*.proj";  // ���� Ȯ���� ����
	//ofn.nFilterIndex = 1;  // �ʱ� ���� �ε���
	//ofn.lpstrFileTitle = nullptr;  // ���������� Ÿ��Ʋ �̸�
	//ofn.nMaxFileTitle = 0;  // �ִ� Ÿ��Ʋ �̸� ũ��

	//wstring strTileFolder = PathMgr::GetInst()->GetContentPath();
	//strTileFolder += L"project";
	//ofn.lpstrInitialDir = Core::ConvWcharTochar(strTileFolder); // �ʱ���

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//// ä�� ���� ����ü�� ������ ���̾�α� ���� (Modal ��� : �ش� â�� ��� ��Ŀ���� �������� �ٸ� â�� �������� ����) 
	//if (GetSaveFileName(&ofn))  // ������ ������ true, ��Ҹ� ������ false ��ȯ
	//{
	//	return SaveProject(Core::ConvCharToWchar(strName));
	//}

}

void Scene_Tool::LoadProject(const wstring& _strFilePath)
{
	//// ������ �����ϱ� ���ؼ��� RAM ���� �޸𸮸� �Ѷ��Ѷ� �о ���Ͽ� �Ű����´�.
	//// ���Ͽ� �Ű����� ���ؼ��� file stream�� �������Ѵ�.


	//if (m_sCurProjectPath != L"")  // �̹� �����ִ� ������Ʈ�� �ֵ�?!!
	//{
	//	DeleteSceneList();
	//	m_iCurLoadedScene = -1;
	//	m_iPrevLoadedScene = -1;
	//}

	//m_sCurProjectPath = _strFilePath;
	//ImGuIRenderClass::GetInst()->SetCurProjectPath(m_sCurProjectPath);

	//FILE* pFile = nullptr;  // File ������ (File �̶�� ���� Ŀ�� ������Ʈ : ���Ͽ� ���� �ּҸ� ������� ���� )
	//_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");  // stream open
	//assert(pFile);

	//size_t size;

	//// 1. ���� �� Load

	//UINT _sceneCount;
	//fread(&_sceneCount, sizeof(UINT), 1, pFile);

	//// ��� �� Load
	//for (UINT s = 0; s < _sceneCount; ++s)
	//{

	//	// 2. �� ��ȣ Load
	//	UINT _sceneType;
	//	fread(&_sceneType, sizeof(SCENE_TYPE), 1, pFile);

	//	size_t size;

	//	// 3. �� �̸� Load
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

	//// ���� Ž���� ����ü
	//OPENFILENAME ofn = {};


	//ofn.lStructSize = sizeof(OPENFILENAME);  // ����ü ������
	//ofn.hwndOwner = Core::GetMainHwnd();  // �θ������� �ڵ�
	//ofn.lpstrFile = strName;  // ���� ���
	//ofn.nMaxFile = sizeof(strName); // ���� ��� ũ�� 
	//ofn.lpstrFilter = "ALL\0*.*\0Project\0*.proj";  // ���� Ȯ���� ����
	//ofn.nFilterIndex = 1;  // �ʱ� ���� �ε���
	//ofn.lpstrFileTitle = nullptr;  // ���������� Ÿ��Ʋ �̸�
	//ofn.nMaxFileTitle = 0;  // �ִ� Ÿ��Ʋ �̸� ũ��

	//wstring strTileFolder = PathMgr::GetInst()->GetContentPath();
	//strTileFolder += L"project";
	//ofn.lpstrInitialDir = ConvWcharTochar(strTileFolder); // �ʱ���

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//// ä�� ���� ����ü�� ������ ���̾�α� ���� (Modal ��� : �ش� â�� ��� ��Ŀ���� �������� �ٸ� â�� �������� ����) 
	//if (GetOpenFileName(&ofn))  // ������ ������ true, ��Ҹ� ������ false ��ȯ
	//{
	//	return LoadProject(ConvCharToWchar(strName));
	//}

}

bool Scene_Tool::GetScene(UINT _sNum)
{
	// �ҷ����� Scene�� SceneMgr�� ���� �޾ƿ�
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
