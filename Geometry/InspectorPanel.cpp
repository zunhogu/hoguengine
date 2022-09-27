#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "InspectorPanel.h"
#include "ModelComp.h"
#include "TransformComp.h"
#include "MeshComp.h"
#include "AnimationComp.h"
#include "BoneComp.h"
#include "ImGuIRenderClass.h"
#include "ResourcePanel.h"
#include "TextureItem.h"
#include "ContentBrowserPanel.h"
#include "Core.h"
#include "ModelNode.h"
#include "ModelComp.h"

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
			{
				modelComps->at(i)->Render(lastSelectedNode);
			}

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

void InspectorPanel::ShowTransformComponent(TransformComp* transformComp)
{
}

bool InspectorPanel::ShowMeshComponent(MeshComp* meshComp)
{
	if (ImGui::CollapsingHeader("Mesh Renderer", meshComp->GetIsDelete(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!(*meshComp->GetIsDelete()))
		{
			meshComp->Shutdown();
			return false;
		}

		bool IsChanged;

		char buffer[256];
		float textWidth = 180.0f;
		ImGuiPayload* payload = nullptr;
		strcpy(buffer, meshComp->GetMeshID().c_str());
		ImGui::Dummy(ImVec2(0.0f, 4.0f));  // padding
		ImGui::Dummy(ImVec2(1.0f, 0.0f));  // padding
		ImGui::SameLine();
		ImGui::Text("Mesh");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(textWidth);
		ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		IsChanged = ImGui::InputText("##MeshID", buffer, sizeof(buffer));
		if (IsChanged)
		{

		}

		ImGui::SameLine();
		bool isRender = meshComp->GetIsRender();
		if (ImGui::Checkbox("Render", &isRender))
			meshComp->SetIsRender(isRender);

		char buff[255];
		ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_CharsDecimal;

		vector<MeshPartComp*> meshParts = *meshComp->GetMeshParts();

		//for (int i = 0; i < meshParts.size(); i++)
		//{
		//	if (ImGui::TreeNode(meshParts[i]->GetMeshName()))
		//	{
		//		vector <MaterialPart*> materialParts = *meshParts[i]->GetMaterialParts();
		//		for (int j = 0; j < materialParts.size(); j++)
		//		{
		//			if (ImGui::TreeNode(materialParts[j]->GetMaterialName()))
		//			{
		//				MaterialComp* material = materialParts[j]->GetMaterial();

		//				// color
		//				static bool alpha_preview = true;
		//				static bool alpha_half_preview = false;
		//				static bool drag_and_drop = true;
		//				static bool options_menu = true;
		//				static bool hdr = false;
		//				ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

		//				XMFLOAT4 ambientColor = material->GetAmbientColor();
		//				XMFLOAT4 emissiveColor = material->GetEmissiveColor();
		//				XMFLOAT4 diffuseColor = material->GetDiffuseColor();
		//				XMFLOAT4 specularColor = material->GetSpecularColor();
		//				float shinness = material->GetShinness();

		//				ImVec4 imAmbientColor = ImVec4(ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w);
		//				ImGui::Text("AmbientColor ");
		//				ImGui::SameLine();
		//				ImGui::PushItemWidth(150.0f);
		//				IsChanged = ImGui::ColorEdit3("##AmbientColor", (float*)&imAmbientColor, misc_flags);
		//				if (IsChanged)
		//				{
		//					material->SetAmibientColor(XMFLOAT4(imAmbientColor.x, imAmbientColor.y, imAmbientColor.z, imAmbientColor.w));
		//				}
		//				ImVec4 imEmissiveColor = ImVec4(emissiveColor.x, emissiveColor.y, emissiveColor.z, emissiveColor.w);
		//				ImGui::Text("EmissiveColor");
		//				ImGui::SameLine();
		//				IsChanged = ImGui::ColorEdit3("##EmissiveColor", (float*)&imEmissiveColor, misc_flags);
		//				if (IsChanged)
		//				{
		//					material->SetEmissiveColor(XMFLOAT4(imEmissiveColor.x, imEmissiveColor.y, imEmissiveColor.z, imEmissiveColor.w));
		//				}
		//				ImVec4 imDiffuseColor = ImVec4(diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w);
		//				ImGui::Text("DiffuseColor ");
		//				ImGui::SameLine();
		//				IsChanged = ImGui::ColorEdit3("##DiffuseColor", (float*)&imDiffuseColor, misc_flags);
		//				if (IsChanged)
		//				{
		//					material->SetDiffuseColor(XMFLOAT4(imDiffuseColor.x, imDiffuseColor.y, imDiffuseColor.z, imDiffuseColor.w));
		//				}
		//				ImVec4 imSpecularColor = ImVec4(specularColor.x, specularColor.y, specularColor.z, specularColor.w);
		//				ImGui::Text("SpecularColor");
		//				ImGui::SameLine();
		//				IsChanged = ImGui::ColorEdit3("##SpecularColor", (float*)&imSpecularColor, misc_flags);
		//				if (IsChanged)
		//				{
		//					material->SetSpecularColor(XMFLOAT4(imSpecularColor.x, imSpecularColor.y, imSpecularColor.z, imSpecularColor.w));
		//				}

		//				ImGui::Text("Shinness     ");
		//				ImGui::SameLine();
		//				IsChanged = ImGui::SliderFloat("##shinness", &shinness, 0.0f, 1.0f);
		//				if (IsChanged)
		//				{
		//					material->SetShinness(shinness);
		//				}


		//				// texture
		//				float textureWidth = 200.f;
		//				float textureHeight = 200.f;
		//				float textWidth = 100.0f;
		//				wstring fileName = L"";
		//				ImGuiPayload* payload = nullptr;

		//				ImGui::PushItemWidth(textWidth);
		//				ImGui::Text("AmbeintTexture  "); ImGui::SameLine();
		//				ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		//				strcpy(buff, material->GetAmbientTextureID().c_str());
		//				IsChanged = ImGui::InputText("##AmbientTexture", buff, sizeof(buff), input_text_flags);

		//				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		//				fileName = ProcessDragAndDropPayloadTexture(payload);
		//				if (fileName != L"")
		//				{
		//					material->SetAmibentTextureID(ImGuIRenderClass::ConvWcharTochar(fileName));
		//					IsChanged = true;
		//				}

		//				if (IsChanged)
		//				{
		//				}
		//				ImGui::SameLine();
		//				if (ImGui::SmallButton("View##ambeint"))
		//				{
		//					ImGui::OpenPopup("ambientview");
		//				}
		//				if (ImGui::BeginPopup("ambientview"))
		//				{
		//					TextureItem* item = (TextureItem*)GetResourceItem((char*)material->GetAmbientTextureID().c_str());
		//					ID3D11ShaderResourceView* texture = item->GetTextureView();
		//					ImGui::Image((ImTextureID)texture, ImVec2(textureWidth, textureHeight), ImVec2(0, 0), ImVec2(1, 1));
		//					ImGui::EndPopup();
		//				}


		//				ImGui::Text("EmissiveTexture "); ImGui::SameLine();
		//				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		//				strcpy(buff, material->GetEmissiveTextureID().c_str());
		//				IsChanged = ImGui::InputText("##EmissiveTexture", buff, sizeof(buff), input_text_flags);

		//				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		//				fileName = ProcessDragAndDropPayloadTexture(payload);
		//				if (fileName != L"")
		//				{
		//					material->SetEmissiveTextureID(ImGuIRenderClass::ConvWcharTochar(fileName));
		//					IsChanged = true;
		//				}
		//				if (IsChanged)
		//				{
		//				}
		//				ImGui::SameLine();
		//				if (ImGui::SmallButton("View##emissive"))
		//				{
		//					ImGui::OpenPopup("emissiveview");
		//				}
		//				if (ImGui::BeginPopup("emissiveview"))
		//				{
		//					TextureItem* item = (TextureItem*)GetResourceItem((char*)material->GetEmissiveTextureID().c_str());
		//					ID3D11ShaderResourceView* texture = item->GetTextureView();
		//					ImGui::Image((ImTextureID)texture, ImVec2(textureWidth, textureHeight), ImVec2(0, 0), ImVec2(1, 1));
		//					ImGui::EndPopup();
		//				}

		//				ImGui::Text("DiffuseTexture  "); ImGui::SameLine();
		//				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		//				strcpy(buff, material->GetDiffuseTextureID().c_str());
		//				IsChanged = ImGui::InputText("##DiffuseTexture", buff, sizeof(buff), input_text_flags);

		//				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		//				fileName = ProcessDragAndDropPayloadTexture(payload);
		//				if (fileName != L"")
		//				{
		//					material->SetDiffuseTextureID(ImGuIRenderClass::ConvWcharTochar(fileName));
		//					IsChanged = true;
		//				}
		//				if (IsChanged)
		//				{
		//				}
		//				ImGui::SameLine();
		//				if (ImGui::SmallButton("View##diffuse"))
		//				{
		//					ImGui::OpenPopup("diffuseview");
		//				}
		//				if (ImGui::BeginPopup("diffuseview"))
		//				{
		//					TextureItem* item = (TextureItem*)GetResourceItem((char*)material->GetDiffuseTextureID().c_str());
		//					ID3D11ShaderResourceView* texture = item->GetTextureView();
		//					ImGui::Image((ImTextureID)texture, ImVec2(textureWidth, textureHeight), ImVec2(0, 0), ImVec2(1, 1));
		//					ImGui::EndPopup();
		//				}

		//				ImGui::Text("SpecularTexture "); ImGui::SameLine();
		//				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		//				strcpy(buff, material->GetSpecularTextureID().c_str());
		//				IsChanged = ImGui::InputText("##SpecularTexture", buff, sizeof(buff), input_text_flags);

		//				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		//				fileName = ProcessDragAndDropPayloadTexture(payload);
		//				if (fileName != L"")
		//				{
		//					material->SetSpecularTextureID(ImGuIRenderClass::ConvWcharTochar(fileName));
		//					IsChanged = true;
		//				}
		//				if (IsChanged)
		//				{

		//				}
		//				ImGui::SameLine();
		//				if (ImGui::SmallButton("View##specular"))
		//				{
		//					ImGui::OpenPopup("specularview");
		//				}
		//				if (ImGui::BeginPopup("specularview"))
		//				{
		//					TextureItem* item = (TextureItem*)GetResourceItem((char*)material->GetSpecularTextureID().c_str());
		//					ID3D11ShaderResourceView* texture = item->GetTextureView();
		//					ImGui::Image((ImTextureID)texture, ImVec2(textureWidth, textureHeight), ImVec2(0, 0), ImVec2(1, 1));
		//					ImGui::EndPopup();
		//				}

		//				ImGui::Text("NormalMapTexture"); ImGui::SameLine();
		//				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		//				strcpy(buff, material->GetNormalTextureID().c_str());
		//				IsChanged = ImGui::InputText("##NormalMapTexture", buff, sizeof(buff), input_text_flags);

		//				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		//				fileName = ProcessDragAndDropPayloadTexture(payload);
		//				if (fileName != L"")
		//				{
		//					material->SetNormalTextureID(ImGuIRenderClass::ConvWcharTochar(fileName));
		//					IsChanged = true;
		//				}
		//				if (IsChanged)
		//				{
		//				}
		//				ImGui::SameLine();
		//				if (ImGui::SmallButton("View##normal"))
		//				{
		//					ImGui::OpenPopup("normalmapview");
		//				}
		//				if (ImGui::BeginPopup("normalmapview"))
		//				{
		//					TextureItem* item = (TextureItem*)GetResourceItem((char*)material->GetNormalTextureID().c_str());
		//					ID3D11ShaderResourceView* texture = item->GetTextureView();
		//					ImGui::Image((ImTextureID)texture, ImVec2(textureWidth, textureHeight), ImVec2(0, 0), ImVec2(1, 1));
		//					ImGui::EndPopup();
		//				}

		//				ImGui::TreePop();
		//			}
		//		}
		//		ImGui::TreePop();
		//	}
		//}
	}

	return true;
}

void InspectorPanel::ShowPopUp()
{
	char* buff = "InspectorPopup";

	if (ImGui::IsMouseClicked(1) &&  g_iCurMousePosWindowID == IMGUI_WINDOW_TYPE::INSPECTOR)
		ImGui::OpenPopup(buff);

	if (ImGui::BeginPopup(buff))
	{
		ImGui::SetCursorPosX(5.0f);
		if (ImGui::MenuItem("MeshRenderer"))
		{
			g_component = COMPONENT_TYPE::MESH;
		}
		ImGui::SetCursorPosX(5.0f);
		if (ImGui::MenuItem("Collider")) {}
		ImGui::EndPopup();
	}
}

ResourceItem* InspectorPanel::GetResourceItem(char* id)
{
	for (int i = 0; i < m_vResources->size(); i++)
	{
		ResourceItem* item = m_vResources->at(i);
		if (strcmp(item->GetID(), id) == 0)
		{
			return item;
		}
	}
}