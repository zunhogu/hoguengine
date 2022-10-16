#pragma once
#include "Scene.h"

#include "CameraClass.h"
#include "LightClass.h"
#include "GridClass.h"
#include "SkyDomeClass.h"
#include "ModelComp.h"
#include "TransformComp.h"
#include "MeshComp.h"
#include "MaterialComp.h"
#include "AnimationComp.h"
#include "BoneComp.h"

class Scene_Tool : public Scene
{
private:

	wstring m_sCurProjectPath;

	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjectionMatrix;

	CameraClass* m_Camera;
	LightClass* m_Light;
	GridClass* m_Grid;
	SkyDomeClass* m_SkyDome;

	// 뷰포트 상에서 카메라 이동을 위한 멤버변수들
	float m_Starfe, m_Walk;

public:
	Scene_Tool();
	~Scene_Tool();

public:
	virtual bool Initialize(int, int, HWND);
	virtual void Shutdown();

	virtual void Frame();
	virtual void Render();

	virtual void Enter();
	virtual void Exit();

	void ViewPortLogic();  // 뷰포트 관련 함수
	void HierachyLogic();
	void ResourceLogic();
	void ImGuiUpdate();

	void CameraMoveKeyBoard();  // 키보드를 이용한 카메라 이동함수
	void CameraMoveKeyMouse(XMFLOAT2 StrafeAndWalk, XMFLOAT3 DisplacementMouse);  // 마우스를 이용한 카메라 이동함수
	void PickingCheckInViewPort();  // 뷰포트 피킹 체크

	void SaveProject(const wstring& _strFilePath);
	void OpenSaveProjectDialog();
	void LoadProject(const wstring& _strRelativePath);
	void OpenLoadProjectDialog();

	bool GetScene(UINT _sType);

	void DeleteKeyLogic();
};

