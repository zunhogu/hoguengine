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

	// ����Ʈ �󿡼� ī�޶� �̵��� ���� ���������
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

	void ViewPortLogic();  // ����Ʈ ���� �Լ�
	void HierachyLogic();
	void ResourceLogic();
	void ImGuiUpdate();

	void CameraMoveKeyBoard();  // Ű���带 �̿��� ī�޶� �̵��Լ�
	void CameraMoveKeyMouse(XMFLOAT2 StrafeAndWalk, XMFLOAT3 DisplacementMouse);  // ���콺�� �̿��� ī�޶� �̵��Լ�
	void PickingCheckInViewPort();  // ����Ʈ ��ŷ üũ

	void SaveProject(const wstring& _strFilePath);
	void OpenSaveProjectDialog();
	void LoadProject(const wstring& _strRelativePath);
	void OpenLoadProjectDialog();

	bool GetScene(UINT _sType);

	void DeleteKeyLogic();
};

