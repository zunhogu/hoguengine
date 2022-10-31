#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "InspectorPanel.h"
#include "ModelComp.h"
#include "TransformComp.h"
#include "MeshComp.h"
#include "AnimationComp.h"
#include "BoneComp.h"
#include "ImGuIRenderClass.h"
#include "ContentBrowserPanel.h"
#include "Core.h"
#include "ModelNode.h"
#include "ModelComp.h"
#include "TerrainComp.h"

// Selected Model
extern vector<ModelNode*> g_selectedNodes;

// window
extern IMGUI_WINDOW_TYPE g_focusedWindow;
extern IMGUI_WINDOW_TYPE g_iCurMousePosWindowID;

// gizmo
extern ImGuizmo::MODE g_gizmoMode;

// resource
extern wstring g_resourceRelativePath;
extern RESOURCE_TYPE g_rosourceType;

// component
extern COMPONENT_TYPE g_component;

InspectorPanel::InspectorPanel()
{
	m_type = IMGUI_WINDOW_TYPE::INSPECTOR;
	m_isRender = false;
}

InspectorPanel::InspectorPanel(vector<ResourceItem*>* resourceItems)
{
	m_type = IMGUI_WINDOW_TYPE::INSPECTOR;
	m_isRender = false;
	m_vResources = resourceItems;
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::Shutdown()
{
}

void InspectorPanel::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정
	if (ImGui::Begin("Inspector"))
	{

		if (IMGUI_WINDOW_FOCUS_CHECK)
		{
			g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::INSPECTOR;
			if (ImGui::IsMouseClicked(0))
				g_focusedWindow = IMGUI_WINDOW_TYPE::INSPECTOR;
		}

		// popup 
		ShowPopUp();

		if (g_selectedNodes.size() > 0)
		{
			ModelNode* lastSelectedNode = g_selectedNodes[g_selectedNodes.size() - 1];
			vector<ModelComp*>* modelComps = lastSelectedNode->GetModelComps();

			// Rendering 
			for (int i = 0; i < modelComps->size(); i++)
				modelComps->at(i)->Render(lastSelectedNode);

			// Checking Delete Component
			for (int i = 0; i < modelComps->size(); i++)
			{
				if ((*modelComps->at(i)->GetIsDelete()) == false)
				{
					modelComps->at(i)->Shutdown();
					modelComps->erase(modelComps->begin() + i);
					break;
				}
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();   // padding 값 설정s
}

void InspectorPanel::ShowPopUp()
{
	char* buff = "InspectorPopup";

	if (ImGui::IsMouseClicked(1) &&  g_iCurMousePosWindowID == IMGUI_WINDOW_TYPE::INSPECTOR)
		ImGui::OpenPopup(buff);

	if (ImGui::BeginPopup(buff))
	{
		ModelNode* selectedNode = nullptr;
		if (!g_selectedNodes.empty())
			selectedNode = g_selectedNodes[g_selectedNodes.size() - 1];

		ImGui::SetCursorPosX(5.0f);
		if (ImGui::MenuItem("MeshRenderer"))
		{
			if (selectedNode != nullptr && selectedNode->GetMeshComp() == nullptr && selectedNode->GetTerrainComp() == nullptr)
			{
				MeshComp* meshComp = new MeshComp();
				selectedNode->AddModelComp(meshComp);
			}

		}
		ImGui::SetCursorPosX(5.0f);
		if (ImGui::MenuItem("Collider")) {}

		ImGui::SetCursorPosX(5.0f);
		if (ImGui::MenuItem("TerrainRenderer")) 
		{
			if (selectedNode != nullptr && selectedNode->GetMeshComp() == nullptr && selectedNode->GetTerrainComp() == nullptr)
			{
				TerrainComp* terrain = new TerrainComp;
				terrain->Initialize(selectedNode, L"contents\\texture\\heightmap01.bmp");
				selectedNode->AddModelComp(terrain);
			}
		}
		ImGui::EndPopup();
	}
}