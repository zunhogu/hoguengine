#include "pch.h"
#include "TerrainComp.h"
#include "Core.h"
#include "ContentBrowserPanel.h"
#include "ResMgrClass.h"
#include "ModelNode.h"
#include "TerrainMesh.h"
#include "Mesh.h"
#include "TransformComp.h"
#include "KeyMgrClass.h"
#include "BitMapClass.h"
#include "MaterialComp.h"

TerrainComp::TerrainComp()
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
	m_isRender = true;
	m_isWireFrame = false;
	m_isLOD = false;
	m_heightMapTexture = nullptr;
	m_isWeightEditMode = false;
	m_isHeightEditMode = false;
	m_terrainEditor = 0;
	m_isRaise = true;

	m_selected_layer = -1;
}

TerrainComp::TerrainComp(const TerrainComp& terrain)
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
}

TerrainComp::~TerrainComp()
{
}

bool TerrainComp::Initialize(ModelNode* node, wstring relativePath)
{
	bool result = true;

	if (relativePath == L"") return false;

	if (m_terrainQuad)
	{
		m_terrainQuad->Shutdown();
		delete m_terrainQuad;
		m_terrainQuad = 0;

		m_terrainMeshKey = L"";
	}

	m_terrainMesh = new TerrainMesh;
	m_terrainMeshKey = Utility::GetInst()->GetRandomKey();
	m_terrainQuad = new TerrainQuadTreeClass;

	ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), L"dirt001.dds", L"contents\\texture\\dirt001.dds");
	
	m_heightMapTexture = ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), m_terrainMeshKey, relativePath);

	result = m_terrainMesh->Initialize(Core::GetDevice(), Utility::GetInst()->ConvWcharTochar(relativePath), 10.0f);
	if (!result)
		return result;

	result = m_terrainQuad->Initialize(m_terrainMesh, Core::GetDevice());
	if (!result)
		return result;

	ResMgrClass::GetInst()->AddMesh(m_terrainMeshKey, m_terrainMesh);

	node->SetIsOnlyMeshPicking(true);

	m_terrainEditor = new TerrainEditor(m_terrainMesh->GetVertexArray(), m_terrainMesh->GetVertexCount()/3, m_terrainMesh->GetTerrainWidth(), m_terrainMesh->GetTerrainHeight());
	if (!m_terrainEditor)
		return false;

	return true;
}

void TerrainComp::Shutdown()
{
	if (m_terrainEditor)
	{
		delete m_terrainEditor;
		m_terrainEditor = 0;
	}

	if (m_terrainQuad)
	{
		m_terrainQuad->Shutdown();
		delete m_terrainQuad;
		m_terrainQuad = 0;
	}

	if (m_terrainMesh)
	{
		m_terrainMesh->Shutdown();
		delete m_terrainMesh;
		m_terrainMesh = 0;
	}
	for (TerrainLayer* layer : m_layers)
		layer->Shutdown();
}

void TerrainComp::Render(ModelNode* node)
{
	if (ImGui::CollapsingHeader("Terrain Mesh Renderer", &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
		Mesh(node);
		if (m_isHeightEditMode)
			BrushHeightMap(node);
		TextureLayer(node);
		if(m_isWeightEditMode)
			BrushWeightMap(node);
	}
}

void TerrainComp::RederTerrain(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX baseViewMatrix, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lihgtDirection, XMFLOAT3 cameraPos)
{
	if (!m_isRender) return;
	
	ID3D11ShaderResourceView* texture = ResMgrClass::GetInst()->FindTexture(L"dirt001.dds")->GetShaderResourceView();

	vector<ID3D11ShaderResourceView*> resourceViews;
	vector<XMFLOAT4> chanelFlag;
	for (int i = 0; i < m_layers.size(); i++)
	{
		if (m_layers[i]->GetMaskID() == L"") continue;


		XMFLOAT4 chanels = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		resourceViews.push_back(ResMgrClass::GetInst()->FindTexture(m_layers[i]->GetMaskID())->GetShaderResourceView());
		if (m_layers[i]->GetMaterialComp1() != nullptr)
		{
			resourceViews.push_back(m_layers[i]->GetMaterialComp1()->GetShaderResourceView());
			chanels.x = 1.0f;
		}
		else resourceViews.push_back(nullptr);

		if (m_layers[i]->GetMaterialComp2() != nullptr)
		{
			resourceViews.push_back(m_layers[i]->GetMaterialComp2()->GetShaderResourceView());
			chanels.y = 1.0f;
		}
		else resourceViews.push_back(nullptr);
		if (m_layers[i]->GetMaterialComp3() != nullptr)
		{
			resourceViews.push_back(m_layers[i]->GetMaterialComp3()->GetShaderResourceView());
			chanels.z = 1.0f;
		}
		else resourceViews.push_back(nullptr);

		chanelFlag.push_back(chanels);
	}


	// Brush Mode
	if (m_isWeightEditMode)
	{
		if (m_terrainEditor->GetBrushPosition(worldMatrix, cameraPos, viewMatrix, m_terrainEditor->GetBrush()->brushPosition))
		{
			XMStoreFloat3(&m_terrainEditor->GetBrush()->brushPosition, XMVector3TransformCoord(XMLoadFloat3(&m_terrainEditor->GetBrush()->brushPosition), worldMatrix));
			m_terrainEditor->PaintWeightMap(baseViewMatrix);
		}
	}
	else if (m_isHeightEditMode)
	{
		if (m_terrainEditor->GetBrushPosition(worldMatrix, cameraPos, viewMatrix, m_terrainEditor->GetBrush()->brushPosition))
		{
			XMStoreFloat3(&m_terrainEditor->GetBrush()->brushPosition, XMVector3TransformCoord(XMLoadFloat3(&m_terrainEditor->GetBrush()->brushPosition), worldMatrix));
			m_terrainEditor->PaintHeightMap(m_terrainQuad, m_isRaise, m_paintValue);
		}
	}
	int brushType = m_terrainEditor->GetBrush()->brushType;
	XMFLOAT3 brushPosition = m_terrainEditor->GetBrush()->brushPosition;
	float brushRange = m_terrainEditor->GetBrush()->brushRange;
	XMFLOAT3 brushColor = m_terrainEditor->GetBrush()->brushColor;

	//Core::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);

	GraphicsClass::GetInst()->RenderTerrainShaderSetParam(Core::GetDeviceContext(), m_isWireFrame, m_isLOD, worldMatrix, viewMatrix, lightDiffuseColor, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), lihgtDirection, cameraPos, resourceViews, chanelFlag, brushType, brushPosition, brushRange, brushColor);

	m_terrainQuad->Render(Core::GetDeviceContext(), worldMatrix, m_isWireFrame);
}

void TerrainComp::Mesh(ModelNode* node)
{
	ImGuiPayload* payload = nullptr;
	wstring filePath = L"";
	float textWidth = 180.0f;
	bool isChanged = false;
	ImRect rect;
	ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Terrain Mesh", treeFlag))
	{
		ImGui::Checkbox("isRender", &m_isRender);
		ImGui::SameLine();
		ImGui::Checkbox("Wire Frame", &m_isWireFrame);
		ImGui::SameLine();
		ImGui::Checkbox("Level of Detail", &m_isLOD);

		ImVec2 texturePos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
		ImGui::SetItemAllowOverlap();
		rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(70.0f, 70.0f));
		ImGui::Image((ImTextureID)m_heightMapTexture->GetShaderResourceView(), ImVec2(70.0f, 70.0f));
		ImGui::SameLine();
		ImGui::SetCursorPos(texturePos);
		ImGui::Dummy(ImVec2(70.0f, 70.0f));
		payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		filePath = Core::ProcessDragAndDropPayloadTexture(payload);
		if (filePath != L"")
		{
			if(Utility::GetInst()->GetFileExtension(filePath) == L"bmp")
				Initialize(node, filePath);
		}

		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			char buffer[255];
			ImGui::Text("Width Size :  ");

			ImGui::SetNextItemWidth(100.f);
			rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
			ImGui::SameLine();
			sprintf(buffer, "%d", m_terrainMesh->GetTerrainWidth());
			ImGui::InputText("##terrain_width", buffer, sizeof(buffer));

			ImGui::Text("Height Size : ");

			ImGui::SetNextItemWidth(100.0f);
			rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
			ImGui::SameLine();
			sprintf(buffer, "%d", m_terrainMesh->GetTerrainHeight());
			ImGui::InputText("##terrain_height", buffer, sizeof(buffer));

			if (ImGui::Button("Height Map Editor"))
			{
				if (m_isWeightEditMode) {}
				else
				{
					if (m_isHeightEditMode)
					{
						m_isHeightEditMode = false;
						m_terrainEditor->EndTerrainEditMode();
					}
					else
					{
						m_isHeightEditMode = true;
						m_terrainEditor->StartTerrainEditMode();
					}
				}
			}
			if (ImGui::Button("Save Height Map"))
			{
				if (!m_terrainMesh->SaveHeightMap(m_terrainQuad->CreateHeightMap(m_terrainMesh->GetTerrainWidth(), m_terrainMesh->GetTerrainHeight(), m_terrainMesh->GetMaximumHeight())))
				{
					MessageBox(nullptr, "Can't Save HeightMap", "Error", 0);
				}
			}

			ImGui::EndGroup();
		}
	
		ImGui::TreePop();
	}

}

void TerrainComp::TextureLayer(ModelNode* node)
{
	ImGuiPayload* payload = nullptr;
	wstring filePath = L"";
	float textWidth = 180.0f;
	bool isChanged = false;
	ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx("Texture Layer", treeFlag))
	{

		if (ImGui::Button("Add Layer"))
		{
			TerrainLayer* layer = new TerrainLayer;
			m_layers.push_back(layer);
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit Layer"))
		{
			if (m_selected_layer != -1 && !m_isWeightEditMode && !m_isHeightEditMode)
			{
				m_isWeightEditMode = true;
				m_terrainEditor->StartTerrainEditMode(m_layers[m_selected_layer]);
			}
			else if(m_selected_layer != -1 && m_isWeightEditMode)
			{	
				m_isWeightEditMode = false;
				m_terrainEditor->EndTerrainEditMode();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Up Layer"))
		{
			if (m_selected_layer == 0) {}
			else
			{
				TerrainLayer* temp = m_layers[m_selected_layer];
				m_layers[m_selected_layer] = m_layers[m_selected_layer - 1];
				m_layers[m_selected_layer - 1] = temp;
				--m_selected_layer;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Down Layer"))
		{
			if (m_selected_layer == m_layers.size()-1) {}
			else
			{
				TerrainLayer* temp = m_layers[m_selected_layer];
				m_layers[m_selected_layer] = m_layers[m_selected_layer + 1];
				m_layers[m_selected_layer + 1] = temp;
				++m_selected_layer;
			}
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
		if (ImGui::BeginTable("##TextureLayerTable", 6, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
		{
			ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_DefaultSort);
			ImGui::TableSetupColumn("Mask", ImGuiTableColumnFlags_NoResize);
			ImGui::TableSetupColumn("Red", ImGuiTableColumnFlags_NoResize);
			ImGui::TableSetupColumn("Green", ImGuiTableColumnFlags_NoResize);
			ImGui::TableSetupColumn("Blue", ImGuiTableColumnFlags_NoResize);
			ImGui::TableSetupColumn("");
			ImGui::TableHeadersRow();

			for (int i = 0; i < m_layers.size(); i++)
			{
				ImGui::TableNextRow(ImGuiTableRowFlags_None);

				string flag = "Layer" + to_string(i);

				ImGui::TableSetColumnIndex(0);
				ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
				if (ImGui::Selectable(flag.c_str(), (m_selected_layer == i), 0))
				{
					if (!m_isWeightEditMode)
					{
						if (m_selected_layer == i) m_selected_layer = -1;
						else m_selected_layer = i;
					}
				}

				ImGui::TableSetColumnIndex(1);
				ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				string label = m_layers[i]->GetMaskID() == L"" ? "NONE##Mask" + flag : Utility::GetInst()->ConvWcharTochar(m_layers[i]->GetMaskID() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
				{
					if (m_layers[i]->GetMaskID() != L"")
						ImGui::OpenPopup(("##" + label + "mask_image").c_str());
				}
				if (ImGui::BeginPopup(("##" + label + "mask_image").c_str()))
				{
					TextureClass* texture = ResMgrClass::GetInst()->FindTexture(m_layers[i]->GetMaskID());
					ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(200.0f, 200.0f));
					ImGui::EndPopup();
				}

				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM", IM_COL32(255, 255, 0, 0));
				filePath = ProcessDragAndDropPayloadTexture(payload);
				if (filePath != L"")
				{
					wstring id = Utility::GetInst()->GetFileName(filePath);
					TextureClass* texture = ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), id, filePath);
					if(m_layers[i]->SetTextureBuffer(texture))
						m_layers[i]->SetMaskID(id);
				}

				ImGui::TableSetColumnIndex(2);
				MaterialComp* materialComp1 = m_layers[i]->GetMaterialComp1();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				label = materialComp1 == nullptr ? "NONE##Material1" + flag : Utility::GetInst()->ConvWcharTochar(materialComp1->GetMaterialName() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
				{
				}

				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM", IM_COL32(255, 255, 0, 0));
				filePath = ProcessDragAndDropPayloadMaterial(payload);
				if (filePath != L"")
				{
					MaterialComp* materialComp = PushMaterialToLayer(node, i, filePath);
					m_layers[i]->SetMaterialComp1(materialComp);
				}


				ImGui::TableSetColumnIndex(3);
				MaterialComp* materialComp2 = m_layers[i]->GetMaterialComp2();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				label = materialComp2 == nullptr ? "NONE##Material" + flag : Utility::GetInst()->ConvWcharTochar(materialComp2->GetMaterialName() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
				{
				}

				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM", IM_COL32(255, 255, 0, 0));
				filePath = ProcessDragAndDropPayloadMaterial(payload);
				if (filePath != L"")
				{
					MaterialComp* materialComp = PushMaterialToLayer(node, i, filePath);
					m_layers[i]->SetMaterialComp2(materialComp);
				}


				ImGui::TableSetColumnIndex(4);
				MaterialComp* materialComp3 = m_layers[i]->GetMaterialComp3();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				label = materialComp3 == nullptr ? "NONE##Material3" + flag : Utility::GetInst()->ConvWcharTochar(materialComp3->GetMaterialName() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
				{
				}

				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM", IM_COL32(255, 255, 0, 0));
				filePath = ProcessDragAndDropPayloadMaterial(payload);
				if (filePath != L"")
				{
					MaterialComp* materialComp = PushMaterialToLayer(node, i, filePath);
					m_layers[i]->SetMaterialComp3(materialComp);
				}
				

				if (ImGui::TableSetColumnIndex(5))
				{
					if (ImGui::SmallButton(("-##" + flag + "delete_button").c_str()))
					{
						m_isWeightEditMode = false;
						m_terrainEditor->EndTerrainEditMode();
						m_layers.erase(m_layers.begin() + i);
					}
				}
			}


			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

void TerrainComp::BrushWeightMap(ModelNode* node)
{
	ImGuiPayload* payload = nullptr;
	wstring filePath = L"";
	float textWidth = 180.0f;
	bool isChanged = false;
	ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Brush##Weight", treeFlag))
	{
		int* type = &m_terrainEditor->GetBrush()->brushType;
		FLOAT* range = &m_terrainEditor->GetBrush()->brushRange;
		XMFLOAT3* color = &m_terrainEditor->GetBrush()->brushColor;
		XMFLOAT3* chanel = &m_terrainEditor->GetBrush()->chanel;

		TextureClass* texture = ResMgrClass::GetInst()->FindTexture(m_layers[m_selected_layer]->GetMaskID());
		if(texture == nullptr) ImGui::Image(nullptr, ImVec2(200.0f, 200.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(3, 3, 3, 3));
		else ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(200.0f, 200.0f), ImVec2(0,0), ImVec2(1, 1), ImVec4(1,1,1,1), ImVec4(3,3,3,3));

		ImGui::SameLine();
		ImGui::BeginGroup();
		if (ImGui::Button("Save Texture "))
		{
		}
		if (ImGui::Button("Reset Texture"))
		{
		}

		ImGui::EndGroup();

		ImGui::SetNextItemWidth(200.f);
		ImGui::Text("[Brush]");
		ImGui::SetNextItemWidth(200.f);
		ImGui::SliderInt("Type", type, 1, 2);
		ImGui::SetNextItemWidth(200.f);
		ImGui::SliderFloat("Range", range, 1.0f, 50.0f);
		ImGui::SetNextItemWidth(200.f);
		ImGui::ColorEdit3("Brush Color", (float*)color);
		ImGui::SetNextItemWidth(200.f);
		ImGui::ColorEdit3("Chanel", (float*)chanel);

		ImGui::TreePop();
	}
}

void TerrainComp::BrushHeightMap(ModelNode* node)
{
	ImGuiPayload* payload = nullptr;
	wstring filePath = L"";
	float textWidth = 180.0f;
	bool isChanged = false;
	ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Brush##Height", treeFlag))
	{
		if (m_heightMapTexture == nullptr) ImGui::Image(nullptr, ImVec2(200.0f, 200.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(3, 3, 3, 3));
		else ImGui::Image((ImTextureID)m_heightMapTexture->GetShaderResourceView(), ImVec2(200.0f, 200.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(3, 3, 3, 3));
		
		int* type = &m_terrainEditor->GetBrush()->brushType;
		FLOAT* range = &m_terrainEditor->GetBrush()->brushRange;
		XMFLOAT3* color = &m_terrainEditor->GetBrush()->brushColor;
		XMFLOAT3* chanel = &m_terrainEditor->GetBrush()->chanel;
		
		ImGui::SetNextItemWidth(200.f);
		ImGui::Text("[Brush]");
		ImGui::Checkbox("isRaise", &m_isRaise);
		ImGui::SetNextItemWidth(200.f);
		ImGui::SliderInt("Type", type, 1, 2);
		ImGui::SetNextItemWidth(200.f);
		ImGui::SliderFloat("Range", range, 1.0f, 50.0f);
		ImGui::SetNextItemWidth(200.f);
		ImGui::ColorEdit3("Brush Color", (float*)color);
		ImGui::SetNextItemWidth(200.f);
		ImGui::SliderInt("Value", &m_paintValue, 0, 20);

		ImGui::TreePop();
	}
}

void TerrainComp::UpdateReferComponent(ModelComp* comp)
{
	for (int i = 0; i < m_layers.size(); i++)
	{
		MaterialComp* materialComp = (MaterialComp*)comp;
		wstring materialName = materialComp->GetMaterialName();
		if (m_layers[i]->GetMaterialComp1() == materialComp)
			m_layers[i]->SetMaterialComp1(nullptr);
		if (m_layers[i]->GetMaterialComp2() == materialComp)
			m_layers[i]->SetMaterialComp2(nullptr);
		if (m_layers[i]->GetMaterialComp3() == materialComp)
			m_layers[i]->SetMaterialComp3(nullptr);
	}
}

wstring TerrainComp::ProcessDragAndDropPayloadTexture(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;
	wstring fileExtension = Utility::GetInst()->GetFileExtension(fileRelativePath);

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

	wstring fileExtension = Utility::GetInst()->GetFileExtension(fileRelativePath);

	if (fileExtension == L"material")
		result = fileRelativePath;

	return result;
}

MaterialComp* TerrainComp::PushMaterialToLayer(ModelNode* node, int index, wstring filePath)
{
	MaterialComp* materialComp = nullptr;

	Material* material = ResMgrClass::GetInst()->LoadMaterial(Utility::GetInst()->GetFileName(filePath), filePath);
	
	// 현재 컴포넌트에 MaterialComp가 있는지 확인
	vector<ModelComp*>* comps = node->GetModelComps();
	for (int i = 0; i < comps->size(); i++)
	{
		if (comps->at(i)->GetCompType() == COMPONENT_TYPE::MATERIAL)
		{
			MaterialComp* comp = (MaterialComp*)comps->at(i);
			if (comp->GetMaterialName() == Utility::GetInst()->GetFileName(filePath))
			{
				materialComp = comp;
				materialComp->SetReferComponent(this);
				break;
			}
		}
	}

	// 없다면 새로 만들어준다.
	if (materialComp == nullptr)
	{
		materialComp = new MaterialComp;
		materialComp->SetMateriaName(material->GetMaterialName());
		materialComp->SetMaterial(material);
		materialComp->SetReferComponent(this);
		comps->push_back(materialComp);
	}

	return materialComp;
}
