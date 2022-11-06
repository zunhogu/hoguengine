#include "pch.h"
#include "ImGuIRenderClass.h"
#include "Panel.h"
#include "ViewPortPanel.h"
#include "HierachyPanel.h"
#include "InspectorPanel.h"
#include "ScenePanel.h"
#include "ContentBrowserPanel.h"
#include "Core.h"
#include "RenderTextureClass.h"


static bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size);

WNDPROC oldProc;

// viewport
ImVec2 g_viewPortSize = ImVec2(0.0f, 0.0f);  // 뷰포트 크기
ImVec2 g_viewPortPos = ImVec2(0.0f, 0.0f);  // 뷰포트 위치
ImVec2 g_prevViewPortSize = ImVec2(0.0f, 0.0f);  // 뷰포트 이전 크기
ImVec2 g_mousePos = ImVec2(0.0f, 0.0f);   // 전체 윈도우 안에서 마우스 좌표
ImVec2 g_mousePosInViewPort = ImVec2(0.0f, 0.0f);  // 뷰포트 안에서의 마우스 좌표 
bool g_isViewPortResizing = false;  // 뷰 포트 리사이징 되었는가?

//window
IMGUI_WINDOW_TYPE g_focusedWindow = IMGUI_WINDOW_TYPE::END;
IMGUI_WINDOW_TYPE g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::END;

// gizmo
extern ImGuizmo::MODE g_gizmoMode;
extern ImGuizmo::OPERATION g_gizmoOper;
bool g_isGizmoClicked = false;

// resource
extern wstring g_resourceRelativePath;
extern RESOURCE_TYPE g_rosourceType;
extern wstring g_pickedResourceID;

// component
COMPONENT_TYPE g_component = COMPONENT_TYPE::END;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
	{
		return true;
	}

	switch (umessage)
	{
	case WM_KILLFOCUS:
		HWND hwnd = GetActiveWindow();
		hwnd;
	}

	return CallWindowProc(oldProc, hwnd, umessage, wparam, lparam);
}


ImGuIRenderClass::ImGuIRenderClass()
{
	m_io = 0;
	m_iSelectMenuNum = -1;
}

ImGuIRenderClass::ImGuIRenderClass(const ImGuIRenderClass&)
{
}

ImGuIRenderClass::~ImGuIRenderClass()
{
}

bool ImGuIRenderClass::Initialize(float screendWidth, float screeHeight, HWND hWnd)
{
	bool result;

	oldProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)WndProc);

	// ImGuiWin32 초기화는 윈도우 핸들러를 파라미터로 받기때문에 ShowWindows 함수 이후에 호출해야한다.
	// 또 ImGuiD3D 초기화는 D3D Device와 D3D Context를 생성한 후에 호출해야한다.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_io = &(ImGui::GetIO()); (void)m_io;

	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	result = ImGui_ImplWin32_Init(hWnd);
	if (!result) {
		return false;
	}
	result = ImGui_ImplDX11_Init(Core::GetDevice(), Core::GetDeviceContext());
	if (!result) {
		return false;
	}


	ViewPortPanel* viewPortPanel = new ViewPortPanel();
	result = viewPortPanel->Initialize(Core::GetDevice(), Core::GetDeviceContext(), screendWidth, screeHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, "Could not initialize the viewport render to texture object.", "Error", MB_OK);
		return false;
	}

	Panel* hierahcyPanel = new HierachyPanel();
	Panel* inspectorPanel = new InspectorPanel();
	Panel* scenePanel = new ScenePanel();
	Panel* contentBrowserPanel = new ContentBrowserPanel();
	m_vPanels.push_back(scenePanel);
	m_vPanels.push_back(hierahcyPanel);
	m_vPanels.push_back(inspectorPanel);
	m_vPanels.push_back(contentBrowserPanel);
	m_vPanels.push_back(viewPortPanel);

	return true;
}

void ImGuIRenderClass::Shutdown()
{

	if (m_io)
	{
		delete m_io;
		m_io = 0;
	}

	// ImGui DXD11 해제, ImGui Win32 해제
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuIRenderClass::RenderMain()
{
	// Ui가 화면 가장 앞으로 와야하므로 화면이 렌더링 되고 난뒤 ui를 렌더링한다.
	// 이때 DX11_NewFrame이 Win32_NewFrame보다 먼저 호출되어야한다.
	// Frame을 만든다.
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui Window를 담을 Window
	{

		// 뷰포트에 대한 윈도우 imgui 윈도우 크기와 위치를 고정한다.
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		static bool no_titlebar = false;
		static bool no_scrollbar = false;
		static bool no_menu = false;
		static bool no_move = false;
		static bool no_resize = false;
		static bool no_collapse = false;
		static bool no_close = false;
		static bool no_nav = false;
		static bool no_background = false;
		static bool no_bring_to_front = false;
		static bool unsaved_document = false;

		bool p_open = true;

		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
		if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
		if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
		if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
		if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
		if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정

		if (ImGui::Begin("Engine", &p_open, window_flags)) {
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ShowMainMenu();
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

		};

		ImGuiID dockSpace2 = ImGui::GetID("dockSpace2");
		ImGui::DockSpace(dockSpace2);
		ImGui::End();
		ImGui::PopStyleVar();   // padding 값 설정


	}

	// panel rendering code ///////////
	for (int i = 0; i < m_vPanels.size(); i++)
	{
		m_vPanels[i]->Render();
	}
	/////////////////////////////

	// Draw Data를 모은다.
	ImGui::Render();

	// 매 프레임마다 rendertargetview를 deviceContext와 연결해줘야하나보다. 아니면 imgui 윈도우가 메인 윈도우 밖으로 나가면 메인 윈도우가 지워진다. 왜그러는지는 모르겠다...
	Core::GetDeviceContext()->OMSetRenderTargets(1, Core::GetRenderTargetView(), NULL);
	
	// 모은 DrawData로 렌더링을 한다.
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}

void ImGuIRenderClass::ShowMainMenu()
{

	if (ImGui::MenuItem("New", "Ctrl+N"))
	{
		m_iSelectMenuNum = 0;
	}
	if (ImGui::MenuItem("Save", "Ctrl+S"))
	{
		m_iSelectMenuNum = 1;
	}
	if (ImGui::MenuItem("Save As..", "Ctrl+LShift+S"))
	{
		m_iSelectMenuNum = 2;
	}
	if (ImGui::MenuItem("Open", "Ctrl+O"))
	{
		m_iSelectMenuNum = 3;
	}

	ImGui::Separator();
}

IMGUI_WINDOW_TYPE ImGuIRenderClass::GetCurFocusImGuiWindowID()
{
	return g_focusedWindow;
}

IMGUI_WINDOW_TYPE ImGuIRenderClass::GetMousePosInImGuiWindowID()
{
	return g_iCurMousePosWindowID;
}

void ImGuIRenderClass::GetViewPortSize(int& screenWidth, int& screenHeigth)
{
	screenWidth = (int)g_viewPortSize.x; screenHeigth = (int)g_viewPortSize.y;
}

bool ImGuIRenderClass::IsViewPortResizing()
{
	return g_isViewPortResizing;
}

void ImGuIRenderClass::SuccessInitViewport()
{
	g_isViewPortResizing = false;
}

void ImGuIRenderClass::GetMousePosInViewPort(int& _mX, int& _mY)
{
	_mX = (int)g_mousePosInViewPort.x; _mY = (int)g_mousePosInViewPort.y;
}

UINT ImGuIRenderClass::GetTransformMode()
{
	return (UINT)g_gizmoMode;
}

void ImGuIRenderClass::SetTransformMode(UINT MODE)
{
	g_gizmoMode = (ImGuizmo::MODE)MODE;
}

UINT ImGuIRenderClass::GetTransformOper()
{
	return (UINT)g_gizmoOper;
}

void ImGuIRenderClass::SetTransformOper(UINT OPER)
{
	if (OPER == 0)
		g_gizmoOper = ImGuizmo::OPERATION::TRANSLATE;
	else if (OPER == 1)
		g_gizmoOper = ImGuizmo::OPERATION::ROTATE;
	else if (OPER == 2)
		g_gizmoOper = ImGuizmo::OPERATION::SCALE;
}

bool ImGuIRenderClass::IsGizmoClicked()
{
	return g_isGizmoClicked;
}

ID3D11ShaderResourceView* ImGuIRenderClass::GetShaderResourceView(const wstring& _strFilePath)
{
	HRESULT result;

	//텍스처 로드
	WCHAR ext[_MAX_EXT];
	ScratchImage image;

	_wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0) {
		result = DirectX::LoadFromDDSFile(_strFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else if (_wcsicmp(ext, L".tga") == 0) {
		result = DirectX::LoadFromTGAFile(_strFilePath.c_str(), nullptr, image);
	}
	else {
		result = DirectX::LoadFromWICFile(_strFilePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}

	ID3D11ShaderResourceView* resourceView;
	DirectX::CreateShaderResourceView(Core::GetDevice(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), &resourceView);
	return resourceView;
}

// Drag And Drop
ImGuiPayload* ImGuIRenderClass::DragAndDropToWindow(const char* type)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
	{
		const ImGuiPayload* payload = AcceptDragDropPayload(type, 0);
		if (payload)
			return (ImGuiPayload*)payload;
		ImGui::EndDragDropTarget();
	}

	return nullptr;
}

ImGuiPayload* ImGuIRenderClass::DraAndDropToItem(ImRect rect, ImVec2 mouseCoordinate, const char* type, ImU32 col)
{
	ImVec2 min = rect.Min;  // item cordinate
	ImVec2 max = rect.Max;  // item size

	ImVec2 mouseCrdInWindow;
	mouseCrdInWindow.x = mouseCoordinate.x - min.x;
	mouseCrdInWindow.y = mouseCoordinate.y - min.y;

	if (mouseCrdInWindow.x <= 0) { mouseCrdInWindow.x = 0; }
	if (mouseCrdInWindow.y <= 0) { mouseCrdInWindow.y = 0; }
	if (mouseCrdInWindow.x >= max.x) { mouseCrdInWindow.x = max.x; }
	if (mouseCrdInWindow.y >= max.y) { mouseCrdInWindow.y = max.y; }

	if (0.0f < mouseCrdInWindow.x && mouseCrdInWindow.x < max.x && 0.0f < mouseCrdInWindow.y && mouseCrdInWindow.y < max.y)
	{
		ImVec2 vMin = ImGui::GetItemRectMin();
		ImVec2 vMax = ImGui::GetItemRectMax();
		ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, col);

		if (ImGui::IsMouseReleased(0))
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload)
				return (ImGuiPayload*)payload;
			else
				return nullptr;
		}
	}

	return nullptr;
}

bool ImGuIRenderClass::BeginDragDropTargetCustom(const ImRect& bb, ImGuiID id)
{
	ImGuiContext& g = *GImGui;
	if (!g.DragDropActive)
		return false;

	ImGuiWindow* window = g.CurrentWindow;
	ImGuiWindow* hovered_window = g.HoveredWindowUnderMovingWindow;
	if (hovered_window == NULL || window->RootWindowDockTree != hovered_window->RootWindowDockTree)
		return false;
	IM_ASSERT(id != 0);
	if (!IsMouseHoveringRect(bb.Min, bb.Max) || (id == g.DragDropPayload.SourceId))
		return false;
	if (window->SkipItems)
		return false;

	IM_ASSERT(g.DragDropWithinTarget == false);
	g.DragDropTargetRect = bb;
	g.DragDropTargetId = id;
	g.DragDropWithinTarget = true;
	return true;
}

ImGuiPayload* ImGuIRenderClass::AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiPayload& payload = g.DragDropPayload;
	IM_ASSERT(g.DragDropActive);                        // Not called between BeginDragDropTarget() and EndDragDropTarget() ?
	IM_ASSERT(payload.DataFrameCount != -1);            // Forgot to call EndDragDropTarget() ?
	if (type != NULL && !payload.IsDataType(type))
		return NULL;

	// Accept smallest drag target bounding box, this allows us to nest drag targets conveniently without ordering constraints.
	// NB: We currently accept NULL id as target. However, overlapping targets requires a unique ID to function!
	const bool was_accepted_previously = (g.DragDropAcceptIdPrev == g.DragDropTargetId);
	ImRect r = g.DragDropTargetRect;
	float r_surface = r.GetWidth() * r.GetHeight();
	if (r_surface <= g.DragDropAcceptIdCurrRectSurface)
	{
		g.DragDropAcceptFlags = flags;
		g.DragDropAcceptIdCurr = g.DragDropTargetId;
		g.DragDropAcceptIdCurrRectSurface = r_surface;
	}

	// Render default drop visuals
	// FIXME-DRAGDROP: Settle on a proper default visuals for drop target.
	payload.Preview = was_accepted_previously;
	flags |= (g.DragDropSourceFlags & ImGuiDragDropFlags_AcceptNoDrawDefaultRect); // Source can also inhibit the preview (useful for external sources that lives for 1 frame)
	if (!(flags & ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		ImGui::GetForegroundDrawList()->AddRect(r.Min, r.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);

	g.DragDropAcceptFrameCount = g.FrameCount;
	payload.Delivery = was_accepted_previously && !IsMouseDown(g.DragDropMouseButton); // For extern drag sources affecting os window focus, it's easier to just test !IsMouseDown() instead of IsMouseReleased()
	if (!payload.Delivery && !(flags & ImGuiDragDropFlags_AcceptBeforeDelivery))
		return NULL;

	return &payload;
}

static bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImVec2 pos_before = window->DC.CursorPos;

	ImGui::PushID(str_id);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));
	bool ret = ImGui::Selectable("##Selectable", selected, flags | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap);
	ImGui::PopStyleVar();

	ImGuiID id = window->GetID("##Input");
	bool temp_input_is_active = ImGui::TempInputIsActive(id);
	bool temp_input_start = ret ? ImGui::IsMouseDoubleClicked(0) : false;

	if (temp_input_start)
		ImGui::SetActiveID(id, window);

	if (temp_input_is_active || temp_input_start)
	{
		ImVec2 pos_after = window->DC.CursorPos;
		window->DC.CursorPos = pos_before;
		ImRect a(ImVec2(0, 0), ImVec2(100, 20));

		ret = ImGui::TempInputText(ImGui::GetCurrentContext()->LastItemData.Rect, id, "##Input", buf, (int)buf_size, 0);
		window->DC.CursorPos = pos_after;
	}
	else
	{
		window->DrawList->AddText(pos_before, ImGui::GetColorU32(ImGuiCol_Text), buf);

	}

	ImGui::PopID();
	return ret;
}

ViewPortPanel* ImGuIRenderClass::GetViewPortPanel()
{
	ViewPortPanel* result = nullptr;
	for (int i = 0; i < m_vPanels.size(); i++)
	{
		if (m_vPanels[i]->GetPanelType() == IMGUI_WINDOW_TYPE::VIEWPORT)
			result = (ViewPortPanel*)m_vPanels[i];
	}

	return result;
}

void ImGuIRenderClass::RenderToTextureStart(ID3D11DeviceContext* deviceContext) 
{
	ViewPortPanel* viewport = GetViewPortPanel();
	
	// RTT가 렌더링 타겟이 되도록한다.
	viewport->GetRenderTexture()->SetRenderTarget(deviceContext);

	// RTT를 초기화한다.
	viewport->GetRenderTexture()->ClearRenderTarget(deviceContext, 0.0f, 0.0f, 0.0f, 1.0f);
}

void ImGuIRenderClass::SetRenderToTexture(ID3D11DeviceContext* deviceContext)
{
	ViewPortPanel* viewport = GetViewPortPanel();

	// RTT가 렌더링 타겟이 되도록한다.
	viewport->GetRenderTexture()->SetRenderTarget(deviceContext);
}

void ImGuIRenderClass::RenderToTextureEnd()
{
	// 렌더타깃을 다시 백버퍼로 돌린다.
	Core::GetInst()->SetBackBufferRenderTarget();

	// 뷰포트도 원래대로 돌린다.
	Core::GetInst()->ResetViewport();
}