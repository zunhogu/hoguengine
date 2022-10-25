#include "pch.h"
#include "TerrainComp.h"
#include "Core.h"
#include "ContentBrowserPanel.h"
#include "ResMgrClass.h"
#include "ModelNode.h"

TerrainComp::TerrainComp()
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
}

TerrainComp::TerrainComp(const TerrainComp& terrain)
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
}

TerrainComp::~TerrainComp()
{
}

void TerrainComp::Initialize(TextureClass* m_heightMap, TextureClass* m_colorMap)
{
	m_heightMapName = Core::ConvWcharTochar(m_heightMap->GetKey());
}

void TerrainComp::Shutdown()
{
}

void TerrainComp::Render(ModelNode* node)
{
	ImGuiPayload* payload = nullptr;
	wstring filePath = L"";
	float textWidth = 180.0f;
	bool isChanged = false;

	if (ImGui::CollapsingHeader("Terrain Mesh Renderer", &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx("Terrain Mesh", treeFlag))
		{
			ImGui::Text("Height Map");

			ImGui::SetNextItemWidth(200.0f);
			ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
			ImGui::SameLine();
			ImGui::InputText("##meshName", (char*)m_heightMapName.c_str(), sizeof(m_heightMapName));

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Texture Layer", treeFlag))
		{
			//ImGui::Text("Layer"); 
			//ImGui::SameLine();
			//ImGui::Dummy(ImVec2(20.f, 0.0f));
			//ImGui::SameLine();
			//ImGui::Text("Chanel");
			//ImGui::SameLine();
			//ImGui::Dummy(ImVec2(20.f,0.0f));
			//ImGui::SameLine();

			if (ImGui::Button("Add Layer"))
			{
				MaterialLayer* layer = new MaterialLayer;
				m_layers.push_back(layer);
			}
			// color
			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;

			enum ContentsType { CT_Text, CT_Button, CT_SmallButton, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
			static float inner_width_with_scroll = 0.0f;
			static bool outer_size_enabled = true;
			static ImVec2 outer_size_value = ImVec2(330.0f, ImGui::GetTextLineHeightWithSpacing() * 12);

			static ImGuiTableFlags flags =
				ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

			const float inner_width_to_use = (flags & ImGuiTableFlags_ScrollX) ? inner_width_with_scroll : 0.0f;
			if (ImGui::BeginTable("##TextureLayerTable", 5, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
			{
				ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_DefaultSort);
				ImGui::TableSetupColumn("Mask");
				ImGui::TableSetupColumn("Material");
				ImGui::TableSetupColumn("Chanel");
				ImGui::TableSetupColumn("");
				ImGui::TableHeadersRow();

				for (int i=0; i<m_layers.size(); i++)
				{
					ImGui::TableNextRow(ImGuiTableRowFlags_None);

					string flag = "Layer" + to_string(i);

					static int selected_item = -1;

					ImGui::TableSetColumnIndex(0);
					ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
					if (ImGui::Selectable(flag.c_str(), (selected_item == i), 0))
					{

					}
					
					
					ImGui::TableSetColumnIndex(1);
					ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y));
					string label = m_layers[i]->GetMaskID() == L"" ? "NONE##Mask" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaskID()+L"##") + flag;
					if (ImGui::Selectable(label.c_str(), (selected_item == i), 0))
					{
						if(m_layers[i]->GetMaskID() != L"")
							ImGui::OpenPopup(("##"+label+"mask_image").c_str());
					}
					if (ImGui::BeginPopup(("##" + label + "mask_image").c_str()))
					{
						TextureClass* texture = ResMgrClass::GetInst()->FindTexture(m_layers[i]->GetMaskID());
						ImGui::Image((ImTextureID)texture->GetTexture(), ImVec2(70.0f, 70.0f));
						ImGui::EndPopup();
					}

					payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM", IM_COL32(255, 255, 0, 0));
					filePath = ProcessDragAndDropPayloadTexture(payload);
					if (filePath != L"")
					{
						wstring id = Core::GetFileName(filePath);
						ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), id, filePath);
						m_layers[i]->SetMaskID(id);
					}

					ImGui::TableSetColumnIndex(2);
					rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y));
					label = m_layers[i]->GetMaterialComp() == nullptr ? "NONE##Material" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaterialComp()->GetMaterialName() + L"##") + flag;
					if (ImGui::Selectable(label.c_str(), (selected_item == i), 0))
					{

					}

					payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM", IM_COL32(255, 255, 0, 0));
					filePath = ProcessDragAndDropPayloadMaterial(payload);
					if (filePath != L"")
					{
						wstring id = Core::GetFileName(filePath);

						vector<ModelComp*>* comps = node->GetModelComps();

						// 기존의 materialComp 삭제
						for(int j=0; j<comps->size(); j++)
						{
							if(comps->at(j) == m_layers[i]->GetMaterialComp())
								comps->erase(comps->begin() + j);
						}
						
						Material* material = ResMgrClass::GetInst()->LoadMaterial(Core::GetFileName(filePath), filePath);

						MaterialComp* materialComp = new MaterialComp;
						materialComp->SetMateriaName(material->GetMaterialName());
						materialComp->SetMaterial(material);
						
						comps->push_back(materialComp);
						m_layers[i]->SetMaterialComp(materialComp);
					}

					ImGui::TableSetColumnIndex(3);
					ImVec4 chanel = ImVec4(m_layers[i]->GetChanel().x, m_layers[i]->GetChanel().y, m_layers[i]->GetChanel().z, m_layers[i]->GetChanel().w);
					ImGui::SetNextItemWidth(120.f);
					isChanged = ImGui::ColorEdit4(("##"+flag + "color").c_str(), (float*)&chanel, misc_flags);
					if (isChanged)
						m_layers[i]->SetChanel(XMFLOAT4(chanel.x, chanel.y, chanel.z, chanel.w));
					
					if (ImGui::TableSetColumnIndex(4))
					{
						if (ImGui::SmallButton(("@##" + flag + "show_button").c_str()))
							m_layers.erase(m_layers.begin() + i);
						ImGui::SameLine();
						if (ImGui::SmallButton(("-##" + flag + "delete_button").c_str()))
							m_layers.erase(m_layers.begin() + i);
					}
				}
				

				ImGui::EndTable();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Brush", treeFlag))
		{
			ImGui::TreePop();
		}
	}
}

wstring TerrainComp::ProcessDragAndDropPayloadTexture(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;
	wstring fileExtension = ContentBrowserPanel::GetFileExtension(fileRelativePath);

	for (int i = 0; i < IM_ARRAYSIZE(g_textureExtension); i++)
	{
		if (fileExtension == g_textureExtension[i])
		{
			result = fileRelativePath;
			break;
		}
	}

	return result;
}

wstring TerrainComp::ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;

	wstring fileExtension = ContentBrowserPanel::GetFileExtension(fileRelativePath);

	if (fileExtension == L"material")
		result = fileRelativePath;

	return result;
}

MaterialLayer::MaterialLayer()
	: m_maskID(L""), m_material(nullptr), m_chanel(0.0f, 0.0f, 0.0f, 0.0f)
{
}

MaterialLayer::~MaterialLayer()
{
}

MaterialLayer::MaterialLayer(const MaterialLayer&)
{
}
