#pragma once
#pragma warning(disable : 4251) 

#include "pch.h"
#include "SceneInfo.h"
#include "ResourceItem.h"

class Panel;
class ResourceItem;

class ImGuIRenderClass
{

private:
	SINGLE(ImGuIRenderClass);

	ImGuiIO* m_io;

	UINT m_iCurFocusWindowID;
	UINT m_iCurMousePosWindowID;

	UINT m_iSelectMenuNum;  // ���õ� �޴� ��� ��ȣ

	vector<Panel*> m_vPanels;

public:
	ImGuIRenderClass();
	ImGuIRenderClass(const ImGuIRenderClass&);
	~ImGuIRenderClass();

public:
	bool Initialize(HWND hWnd);
	void Shutdown();

	// Render ImGuie UI
	void RenderMain();
	void ShowMainMenu();

	IMGUI_WINDOW_TYPE GetCurFocusImGuiWindowID();
	IMGUI_WINDOW_TYPE GetMousePosInImGuiWindowID();

	// View Port
	void GetViewPortSize(int& screenWidth, int& screenHeigth);
	bool IsViewPortResizing();
	void SuccessInitViewport();
	void GetMousePosInViewPort(int& _mX, int& _mY);
	UINT GetTransformMode();
	void SetTransformMode(UINT MODE);
	UINT GetTransformOper();
	void SetTransformOper(UINT OPER);
	bool IsGizmoClicked();

	// ��ŷ �Լ�

	// ������Ʈ �Լ�

	// �޴� �Լ�

	// Component

	// Scene Tool���� �� �� Textures �������� �Լ� 
	static ID3D11ShaderResourceView* GetTexture(const wstring& _strFilePath);

	//ImGui Drag And Drop
	static ImGuiPayload* DragAndDropToWindow(const char* type);
	static ImGuiPayload* DraAndDropToItem(ImRect rect, ImVec2 mouseCoordinate, const char* type);
	static bool BeginDragDropTargetCustom(const ImRect& bb, ImGuiID id);
	static ImGuiPayload* AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags);
};

