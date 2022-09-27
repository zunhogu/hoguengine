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

	UINT m_iSelectMenuNum;  // 선택된 메뉴 요소 번호

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

	// 피킹 함수

	// 업데이트 함수

	// 메뉴 함수

	// Component

	// Scene Tool에서 만 쓸 Textures 가져오는 함수 
	static ID3D11ShaderResourceView* GetTexture(const wstring& _strFilePath);

	//ImGui Drag And Drop
	static ImGuiPayload* DragAndDropToWindow(const char* type);
	static ImGuiPayload* DraAndDropToItem(ImRect rect, ImVec2 mouseCoordinate, const char* type);
	static bool BeginDragDropTargetCustom(const ImRect& bb, ImGuiID id);
	static ImGuiPayload* AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags);
};

