#include "../pch.h"
#include "ScenePanel.h"

// window focus
extern IMGUI_WINDOW_TYPE g_focusedWindow;
extern IMGUI_WINDOW_TYPE g_iCurMousePosWindowID;

// gizmo
ImGuizmo::OPERATION g_gizmoOper = ImGuizmo::TRANSLATE;
ImGuizmo::MODE g_gizmoMode = ImGuizmo::WORLD;

ScenePanel::ScenePanel()
{
	m_type = IMGUI_WINDOW_TYPE::SCENE;
	m_isRender = false;
}

ScenePanel::~ScenePanel()
{
}

void ScenePanel::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정
	if (ImGui::Begin("Scene"))
	{

		if (IMGUI_WINDOW_FOCUS_CHECK)
		{
			g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::SCENE;
			if (ImGui::IsMouseClicked(0))
				g_focusedWindow = IMGUI_WINDOW_TYPE::SCENE;
		}

		ShowGizmo();

	}
	ImGui::End();
	ImGui::PopStyleVar();   // padding 값 설정s
}

void ScenePanel::ShowGizmo()
{
	if (ImGui::CollapsingHeader("Gizmo", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static int mode = 1;
		static int operation_mode = 0;

		if (g_gizmoOper == ImGuizmo::TRANSLATE)
			operation_mode = 0;
		else if (g_gizmoOper == ImGuizmo::ROTATE)
			operation_mode = 1;
		else if (g_gizmoOper == ImGuizmo::SCALE)
			operation_mode = 2;


		ImGui::RadioButton("Local", &mode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("World", &mode, 1);

		switch (mode)
		{
		case 0:
			g_gizmoMode = ImGuizmo::LOCAL;
			break;
		case 1:
			g_gizmoMode = ImGuizmo::WORLD;
			break;
		}

		ImGui::RadioButton("Translate", &operation_mode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Rotation", &operation_mode, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Scale", &operation_mode, 2);

		switch (operation_mode)
		{
		case 0:
			g_gizmoOper = ImGuizmo::TRANSLATE;
			break;
		case 1:
			g_gizmoOper = ImGuizmo::ROTATE;
			break;
		case 2:
			g_gizmoOper = ImGuizmo::SCALE;
			break;
		}
	}
}
#include "../pch.h"