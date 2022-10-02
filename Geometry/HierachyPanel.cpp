#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "HierachyPanel.h"
#include "ImGuIRenderClass.h"
#include "ContentBrowserPanel.h"
#include "SceneMgrClass.h"
#include "Scene.h"
#include "ModelNode.h"
#include "ModelInfoComp.h"
#include "Prefab.h"
#include "ResMgrClass.h"
using namespace std;

extern vector<ModelNode*> g_selectedNodes;

extern IMGUI_WINDOW_TYPE g_focusedWindow;
extern IMGUI_WINDOW_TYPE g_iCurMousePosWindowID;

HierachyPanel::HierachyPanel()
{
	m_type = IMGUI_WINDOW_TYPE::HIERACHY;
	m_isRender = false;

	m_close_node_img = ImGuIRenderClass::GetTexture(L"default\\texture\\triangle_icon2.png");
	m_open_node_img = ImGuIRenderClass::GetTexture(L"default\\texture\\triangle_icon1.png");
}

HierachyPanel::~HierachyPanel()
{
}

void HierachyPanel::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정

	if (ImGui::Begin("Hierachy")) {
		if (IMGUI_WINDOW_FOCUS_CHECK)
		{
			g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::HIERACHY;
			if (ImGui::IsMouseClicked(0))
				g_focusedWindow = IMGUI_WINDOW_TYPE::HIERACHY;
		}

		// resource panel에서 drag and drop 되는지 검사
		ImGuiPayload* payload = ImGuIRenderClass::DragAndDropToWindow("CONTENT_BROWSER_ITEM");
		ProcessDragAndDropPayload(payload);

		// popup 
		ShowPopUp();

		// 씬 안에 있는 모든 Root Node들 
		Scene* scene = SceneMgrClass::GetInst()->GetCurScene();
		vector<ModelNode*> nodes = *scene->GetAllNode();

		for (size_t i = 0; i < nodes.size(); ++i)
		{
			ModelNode* modelNodes = nodes[i];

			// picking Node Update
			NodeUpdate(nodes[i]);

			// Rendering
			ShowNodeTree(modelNodes, i);
		}

	}
	ImGui::End();
	ImGui::PopStyleVar();   // padding 값 설정s
}

void HierachyPanel::NodeUpdate(ModelNode* node)
{
	if (g_selectedNodes.size() <= 0)
	{
		node->SetIsClicekdHierachyAllFalse();
		node->Clear();
	}
	else if (g_selectedNodes.size() > 0)
	{
		ModelNode* lastPickedNode = g_selectedNodes[g_selectedNodes.size() - 1];

		// 현재 노드에 대한 Update
		if (node == lastPickedNode)
		{
			//// 해당 노드의 루트까지의 경로를 open 상태로 전환
			//node->SetIsOpenedHierachyAllTrueToRootPath();

			// 그렇지 않으면 노드를 unclick 상태로 전환
			node->SetIsClicekdHierachyAllFalse();

			// 해당 노드를 click 상태로 전환
			node->SetIsClickedHierachy(true);
		}
		else
		{
			// 그렇지 않으면 노드를 unclick 상태로 전환
			node->SetIsClickedHierachy(false);
		}

		// 자식 노드에 대한 Update
		vector<ModelNode*>* childNodes = node->GetChildNodes();
		for (int j = 0; j < childNodes->size(); j++)
			NodeUpdate(childNodes->at(j));
	}

}

void HierachyPanel::ShowNodeTree(ModelNode* modelNode, int num)
{
	ModelInfoComp* comp = modelNode->GetModelInfoComp();
	string nodeName = Core::ConvWcharTochar(comp->GetModelName());
	
	nodeName += ("##" + to_string(num));

	int depth = modelNode->GetTreeDepth();
	int depth_size = 10.f;
	ImGui::Dummy(ImVec2(depth * depth_size, 0.0f));
	ImGui::SameLine();

	vector<ModelNode*> chilNodes = *modelNode->GetChildNodes();
	if (chilNodes.size() > 0)  // 자식 노드가 하나라도 있다면 
	{
		ID3D11ShaderResourceView* buttonImg;
		bool open = modelNode->GetIsOpenedHierachy();
		if (open)
			buttonImg = m_open_node_img;
		else
			buttonImg = m_close_node_img;
		ImVec2 buttonSize = ImVec2(ImGui::GetFontSize() / 1.5f, ImGui::GetFontSize() / 1.5f);
		ImVec2 buttonPos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());

		ImGui::SetCursorPos(buttonPos);
		ImGui::Dummy(buttonSize);
		ImGui::SameLine();
		ImGui::SetItemAllowOverlap();  // dummy is overlaped with button to check hovering on button

		// arrow clicked
		if (ImGui::IsItemClicked(0))
		{
			if (open)
				modelNode->SetIsOpenedHierachy(false);
			else
				modelNode->SetIsOpenedHierachy(true);
		}
		ImGui::SetCursorPos(buttonPos);
		ImGui::Image((ImTextureID)buttonImg, buttonSize);
		ImGui::SetItemAllowOverlap();
		ImGui::SameLine();
	}
	else
	{
		ImGui::Dummy(ImVec2(ImGui::GetFontSize() / 1.8f, 0.0f));
		ImGui::SameLine();
	}

	if (ImGui::Selectable(nodeName.c_str(), modelNode->GetIsClickedHierachy(), ImGuiSelectableFlags_SpanAllColumns))
	{
		modelNode->SetIsClicekdHierachyAllFalse();
		modelNode->SetIsClickedHierachy(true);
		// 이미 피킹된 객체가 있다면? 
		if (g_selectedNodes.size() > 0)
		{
			// 피킹된 객체를 초기화해준다.
			for (int i = 0; i < g_selectedNodes.size(); i++)
				g_selectedNodes[i]->Clear();
		}

		g_selectedNodes.push_back(modelNode);
	}

	if (g_iCurMousePosWindowID == IMGUI_WINDOW_TYPE::HIERACHY && !ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		modelNode->SetIsClicekdHierachyAllFalse();

	if (modelNode->GetIsOpenedHierachy())
	{
		// 자식 노드 순회
		for (int i = 0; i < chilNodes.size(); i++)
		{
			ShowNodeTree(chilNodes[i], num);
		}
	}
}

void HierachyPanel::ShowPopUp()
{
	char* buff = "HierachyPopup";

	if (ImGui::IsMouseClicked(1) && g_iCurMousePosWindowID == IMGUI_WINDOW_TYPE::HIERACHY)
		ImGui::OpenPopup(buff);

	if (ImGui::BeginPopup(buff))
	{
		static string items[] = { "Cube", "Sphere", "Cone", "Cylinder", "Plane", "Empthy Obejct" };

		if (ImGui::BeginMenu("3D Object")) {
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				ImGui::SetCursorPosX(5.0f);
				string item = items[i];
				if (ImGui::MenuItem(item.c_str()))
				{
					wstring path = L"";
					if (item == "Cube")
						path = L"default\\fbx\\id_cube.fbx";
					else if (item == "Sphere")
						path = L"default\\fbx\\id_sphere.fbx";
					else if (item == "Cone")
						path = L"default\\fbx\\id_cone.fbx";
					else if (item == "Cylinder")
						path = L"default\\fbx\\id_cylinder.fbx";
					else if (item == "Plane")
						path = L"default\\fbx\\id_plane.fbx";
					else if (item == "Empthy Obejct")
						path = L"Emphty_Object";

				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Camera")) {}
		if (ImGui::MenuItem("Light")) {}
		ImGui::EndPopup();
	}
}

void HierachyPanel::ProcessDragAndDropPayload(ImGuiPayload* payload)
{
	if (payload == nullptr)
		return;

	wstring fileRelativePath = (wchar_t*)payload->Data;
	wstring fileFileName = Core::GetFileName(fileRelativePath);
	wstring fileExtension = ContentBrowserPanel::GetFileExtension(fileRelativePath);

	for (int i = 0; i < IM_ARRAYSIZE(g_modelExtension); i++)
	{
		if (fileExtension == g_modelExtension[i])
		{

			Scene* currScene = SceneMgrClass::GetInst()->GetCurScene();

			Prefab* prefab = ResMgrClass::GetInst()->LoadPrefab(fileFileName, fileRelativePath);

			currScene->AddNodeToScene(new ModelNode(*prefab->GetModelNode()));

			return;
		}
	}
}
