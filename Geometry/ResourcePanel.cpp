#include "pch.h"
#include "ResourcePanel.h"
#include "ResourceItem.h"
#include "TextureItem.h"
#include "ImGuIRenderClass.h"
#include "ContentBrowserPanel.h"
#include "Core.h"

// window focus
extern IMGUI_WINDOW_TYPE g_focusedWindow;
extern IMGUI_WINDOW_TYPE g_iCurMousePosWindowID;

// resource
wstring g_pickedResourceID = L"";
extern RESOURCE_TYPE g_rosourceType;
extern wstring g_resourceRelativePath;

ResourcePanel::ResourcePanel()
{
	m_type = IMGUI_WINDOW_TYPE::RESOURCE;
	m_isRender = false;
}

ResourcePanel::ResourcePanel(vector<ResourceItem*>* resources)
{
	m_type = IMGUI_WINDOW_TYPE::RESOURCE;
	m_isRender = false;
	m_vResources = resources;
}

ResourcePanel::~ResourcePanel()
{
}

void ResourcePanel::Render()
{
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정
	//if (ImGui::Begin("Resource"))
	//{

	//	if (IMGUI_WINDOW_FOCUS_CHECK)
	//	{
	//		g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::RESOURCE;
	//		if (ImGui::IsMouseClicked(0))
	//			g_focusedWindow = IMGUI_WINDOW_TYPE::RESOURCE;
	//	}

	//	ShowResource();

	//}
	//ImGui::End();
	//ImGui::PopStyleVar();   // padding 값 설정s
}

void ResourcePanel::ShowResource()
{
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
		| ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_SizingFixedFit;
	if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
	{

		if (ImGui::BeginTabItem("Model"))
		{
			if (ImGui::BeginTable("#ModelTable", 2, flags))
			{
				ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort);
				ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupScrollFreeze(1, 1); // Make row always visible
				ImGui::TableHeadersRow();

				static int prevSelected_row1 = -1;
				for (int row = 0; row < m_vResources->size(); row++)
				{
					ResourceItem* item = m_vResources->at(row);
					if (item->GetResourceType() != RESOURCE_TYPE::MODEL) continue;

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::Selectable(item->GetID(), item->GetIsClicked(), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
					{
						g_rosourceType = RESOURCE_TYPE::MODEL;
						g_pickedResourceID = Core::ConvCharToWchar(item->GetID());
						m_vResources->at(row)->SetIsClicked(true);
						if (prevSelected_row1 != -1)
							m_vResources->at(prevSelected_row1)->SetIsClicked(false);
						if (prevSelected_row1 == row)
						{
							g_pickedResourceID = L"";
							prevSelected_row1 = -1;
						}
						else
							prevSelected_row1 = row;
					}
					if (ImGui::BeginDragDropSource())
					{
						wstring id = Core::ConvCharToWchar(item->GetPath());
						ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", id.c_str(), (wcslen(id.c_str()) + 1) * sizeof(wchar_t));
						ImGui::Text(item->GetID());
						ImGui::EndDragDropSource();
					}

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(item->GetPath());

				}

				ImGui::EndTable();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Texture"))
		{
			if (ImGui::BeginTable("#TextureTable", 3, flags))
			{
				ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort);
				ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupColumn("View", ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
				ImGui::TableHeadersRow();

				static int prevSelected_row2 = -1;
				for (int row = 0; row < m_vResources->size(); row++)
				{
					TextureItem* item = (TextureItem*)m_vResources->at(row);
					if (item->GetResourceType() != RESOURCE_TYPE::TEXTURE) continue;

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::Selectable(item->GetID(), item->GetIsClicked(), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
					{
						g_rosourceType = RESOURCE_TYPE::TEXTURE;
						g_pickedResourceID = Core::ConvCharToWchar(item->GetID());
						m_vResources->at(row)->SetIsClicked(true);
						if (prevSelected_row2 != -1)
							m_vResources->at(prevSelected_row2)->SetIsClicked(false);
						if (prevSelected_row2 == row)
						{
							g_pickedResourceID = L"";
							prevSelected_row2 = -1;
						}
						else
							prevSelected_row2 = row;
					}
					if (ImGui::BeginDragDropSource())
					{
						wstring id = Core::ConvCharToWchar(item->GetPath());
						ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", id.c_str(), (wcslen(id.c_str()) + 1) * sizeof(wchar_t));
						ImGui::Text(item->GetID());
						ImGui::EndDragDropSource();
					}

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(item->GetPath());

					float textureWidth = 200.f;
					float textureHeight = 200.f;

					char buff1[128] = "View##";
					char buff2[128];
					char buff3[128] = "TexturePopUp";
					sprintf_s(buff2, "%d", row);
					strcat_s(buff1, buff2);
					strcat_s(buff3, buff2);

					ImGui::TableSetColumnIndex(2);
					if (ImGui::SmallButton(buff1))
					{
						ImGui::OpenPopup(buff3);
					}
					if (ImGui::BeginPopup(buff3))
					{
						ID3D11ShaderResourceView* texture = item->GetTextureView();
						ImGui::Image((ImTextureID)texture, ImVec2(textureWidth, textureHeight), ImVec2(0, 0), ImVec2(1, 1));
						ImGui::EndPopup();
					}
				}

				ImGui::EndTable();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Sound"))
		{
			if (ImGui::BeginTable("#SoundTable", 3, flags))
			{
				ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort);
				ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupColumn("Play", ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
				ImGui::TableHeadersRow();

				static int prevSelected_row3 = -1;
				for (int row = 0; row < m_vResources->size(); row++)
				{
					ResourceItem* item = m_vResources->at(row);

					if (item->GetResourceType() != RESOURCE_TYPE::SOUND) continue;

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::Selectable(item->GetID(), item->GetIsClicked(), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
					{
						g_rosourceType = RESOURCE_TYPE::SOUND;
						g_pickedResourceID = Core::ConvCharToWchar(item->GetID());
						m_vResources->at(row)->SetIsClicked(true);
						if (prevSelected_row3 != -1)
							m_vResources->at(prevSelected_row3)->SetIsClicked(false);
						if (prevSelected_row3 == row)
						{
							g_pickedResourceID = L"";
							prevSelected_row3 = -1;
						}
						else
							prevSelected_row3 = row;
					}
					if (ImGui::BeginDragDropSource())
					{
						wstring id = Core::ConvCharToWchar(item->GetPath());
						ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", id.c_str(), (wcslen(id.c_str()) + 1) * sizeof(wchar_t));
						ImGui::Text(item->GetID());
						ImGui::EndDragDropSource();
					}

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(item->GetPath());

					char buff1[128] = "Play##";
					char buff2[128];
					sprintf_s(buff2, "%d", row);
					strcat_s(buff1, buff2);
					ImGui::TableSetColumnIndex(2);
					if (ImGui::SmallButton(buff1))
					{

					}
				}

				ImGui::EndTable();
			}
			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();

	ProcessDragAndDropPayload(ImGuIRenderClass::DragAndDropToWindow("CONTENT_BROWSER_ITEM"));
}

void ResourcePanel::ProcessDragAndDropPayload(ImGuiPayload* payload)
{
	if (payload == nullptr)
		return;

	wstring fileRelativePath = (wchar_t*)payload->Data;

	wstring fileExtension = ContentBrowserPanel::GetFileExtension(fileRelativePath);

	//// Resource 패널에서 모델 resource는 drag and dorp 되지 않게 한다. 
	//for (int i = 0; i < IM_ARRAYSIZE(g_modelExtension); i++)
	//{
	//	if (fileExtension == g_modelExtension[i])
	//	{
	//		// Model에 대한 Resource ID를 가져옴 
	//		g_resourceRelativePath = fileRelativePath;

	//		g_rosourceType = RESOURCE_TYPE::MODEL;
	//		return;
	//	}
	//}
	for (int i = 0; i < IM_ARRAYSIZE(g_textureExtension); i++)
	{
		if (fileExtension == g_textureExtension[i])
		{
			// Model에 대한 Resource ID를 가져옴 
			g_resourceRelativePath = fileRelativePath;

			g_rosourceType = RESOURCE_TYPE::TEXTURE;
			return;
		}
	}
}
