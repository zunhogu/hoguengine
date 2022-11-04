#include "pch.h"
#include "TerrainComp.h"
#include "Core.h"
#include "ContentBrowserPanel.h"
#include "ResMgrClass.h"
#include "ModelNode.h"
#include "TerrainMesh.h"
#include "Mesh.h"
#include "TransformComp.h"

TerrainComp::TerrainComp()
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
	m_isRender = true;
	m_isWireFrame = false;
	m_isLOD = false;
	m_heightMapTexture = nullptr;
	m_isBrushMode = true;
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
	m_terrainMeshKey = Core::GetRandomKey();
	m_terrainQuad = new TerrainQuadTreeClass;

	ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), L"dirt01.dds", L"contents\\texture\\dirt01.dds");
	
	m_heightMapTexture = ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), m_terrainMeshKey, relativePath);

	result = m_terrainMesh->Initialize(Core::GetDevice(), Core::ConvWcharTochar(relativePath), 10.0f);
	if (!result)
		return result;

	result = m_terrainQuad->Initialize(m_terrainMesh, Core::GetDevice());
	if (!result)
		return result;

	ResMgrClass::GetInst()->AddMesh(m_terrainMeshKey, m_terrainMesh);

	node->SetIsOnlyMeshPicking(true);

	return true;
}

void TerrainComp::Shutdown()
{
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
	for (MaterialLayer* layer : m_layers)
		layer->Shutdown();
}

void TerrainComp::Render(ModelNode* node)
{
	if (ImGui::CollapsingHeader("Terrain Mesh Renderer", &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
		Mesh(node);
		TextureLayer(node);
		Brush(node);
	}
}

void TerrainComp::RederTerrain(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lihgtDirection, XMFLOAT3 cameraPos)
{
	if (!m_isRender) return;
	
	ID3D11ShaderResourceView* texture = ResMgrClass::GetInst()->FindTexture(L"dirt01.dds")->GetTexture();

	vector<ID3D11ShaderResourceView*> resourceViews;
	vector<XMFLOAT4> chanelFlag;
	for (int i = 0; i < m_layers.size(); i++)
	{
		if (m_layers[i]->GetMaskID() == L"") continue;

		XMFLOAT4 chanels = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		resourceViews.push_back(ResMgrClass::GetInst()->FindTexture(m_layers[i]->GetMaskID())->GetTexture());
		if (m_layers[i]->GetMaterialComp1() != nullptr)
		{
			resourceViews.push_back(m_layers[i]->GetMaterialComp1()->GetShaderResourceView());
			chanels.x = 1.0f;
		}

		if (m_layers[i]->GetMaterialComp2() != nullptr)
		{
			resourceViews.push_back(m_layers[i]->GetMaterialComp2()->GetShaderResourceView());
			chanels.y = 1.0f;
		}

		if (m_layers[i]->GetMaterialComp3() != nullptr)
		{
			resourceViews.push_back(m_layers[i]->GetMaterialComp3()->GetShaderResourceView());
			chanels.z = 1.0f;
		}
		chanelFlag.push_back(chanels);
	}


	UINT brushType = 1;
	XMFLOAT3 brushPosition = XMFLOAT3();
	FLOAT brushRange = 20.0f;
	XMFLOAT3 brushColor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// Brush Mode
	if (m_isBrushMode)
	{
		brushPosition = GetBrushPosition(worldMatrix, cameraPos, viewMatrix);
		//cout << "X : " << brushPosition.x<< ", Z : " << brushPosition.z << endl;
	}

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
		ImGui::Image((ImTextureID)m_heightMapTexture->GetTexture(), ImVec2(70.0f, 70.0f));
		ImGui::SameLine();
		ImGui::SetCursorPos(texturePos);
		ImGui::Dummy(ImVec2(70.0f, 70.0f));
		payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		filePath = Core::ProcessDragAndDropPayloadTexture(payload);
		if (filePath != L"")
		{
			if(Core::GetFileExtension(filePath) == L"bmp")
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

				static int selected_item = -1;

				ImGui::TableSetColumnIndex(0);
				ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
				if (ImGui::Selectable(flag.c_str(), (selected_item == i), 0))
				{

				}

				ImGui::TableSetColumnIndex(1);
				ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				string label = m_layers[i]->GetMaskID() == L"" ? "NONE##Mask" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaskID() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (selected_item == i), 0))
				{
					if (m_layers[i]->GetMaskID() != L"")
						ImGui::OpenPopup(("##" + label + "mask_image").c_str());
				}
				if (ImGui::BeginPopup(("##" + label + "mask_image").c_str()))
				{
					TextureClass* texture = ResMgrClass::GetInst()->FindTexture(m_layers[i]->GetMaskID());
					ImGui::Image((ImTextureID)texture->GetTexture(), ImVec2(200.0f, 200.0f));
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
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				label = m_layers[i]->GetMaterialComp1() == nullptr ? "NONE##Material1" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaterialComp1()->GetMaterialName() + L"##") + flag;
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
					for (int j = 0; j < comps->size(); j++)
					{
						if (comps->at(j) == m_layers[i]->GetMaterialComp1())
							comps->erase(comps->begin() + j);
					}


					Material* material = ResMgrClass::GetInst()->LoadMaterial(Core::GetFileName(filePath), filePath);

					bool find = false;
					vector<ModelComp*>* modelComps = node->GetModelComps();
					for (int i = 0; i < modelComps->size(); i++)
					{
						if (modelComps->at(i)->GetCompType() == COMPONENT_TYPE::MATERIAL)
						{
							MaterialComp* comp = (MaterialComp*)modelComps->at(i);
							if (material == comp->GetMaterial())
								find = true;
						}
					}

					if (!find)
					{
						MaterialComp* materialComp = new MaterialComp;
						materialComp->SetMateriaName(material->GetMaterialName());
						materialComp->SetMaterial(material);

						comps->push_back(materialComp);
						m_layers[i]->SetMaterialComp1(materialComp);
					}
				}


				ImGui::TableSetColumnIndex(3);
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				label = m_layers[i]->GetMaterialComp2() == nullptr ? "NONE##Material" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaterialComp2()->GetMaterialName() + L"##") + flag;
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
					for (int j = 0; j < comps->size(); j++)
					{
						if (comps->at(j) == m_layers[i]->GetMaterialComp2())
							comps->erase(comps->begin() + j);
					}

					Material* material = ResMgrClass::GetInst()->LoadMaterial(Core::GetFileName(filePath), filePath);

					bool find = false;
					vector<ModelComp*>* modelComps = node->GetModelComps();
					for (int i = 0; i < modelComps->size(); i++)
					{
						if (modelComps->at(i)->GetCompType() == COMPONENT_TYPE::MATERIAL)
						{
							MaterialComp* comp = (MaterialComp*)modelComps->at(i);
							if (material == comp->GetMaterial())
								find = true;
						}
					}

					if (!find)
					{
						MaterialComp* materialComp = new MaterialComp;
						materialComp->SetMateriaName(material->GetMaterialName());
						materialComp->SetMaterial(material);

						comps->push_back(materialComp);
						m_layers[i]->SetMaterialComp2(materialComp);
					}
				}


				ImGui::TableSetColumnIndex(4);
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				label = m_layers[i]->GetMaterialComp3() == nullptr ? "NONE##Material3" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaterialComp3()->GetMaterialName() + L"##") + flag;
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
					for (int j = 0; j < comps->size(); j++)
					{
						if (comps->at(j) == m_layers[i]->GetMaterialComp3())
							comps->erase(comps->begin() + j);
					}

					Material* material = ResMgrClass::GetInst()->LoadMaterial(Core::GetFileName(filePath), filePath);

					bool find = false;
					vector<ModelComp*>* modelComps = node->GetModelComps();
					for (int i = 0; i < modelComps->size(); i++)
					{
						if (modelComps->at(i)->GetCompType() == COMPONENT_TYPE::MATERIAL)
						{
							MaterialComp* comp = (MaterialComp*)modelComps->at(i);
							if (material == comp->GetMaterial())
								find = true;
						}
					}

					if (!find)
					{
						MaterialComp* materialComp = new MaterialComp;
						materialComp->SetMateriaName(material->GetMaterialName());
						materialComp->SetMaterial(material);

						comps->push_back(materialComp);
						m_layers[i]->SetMaterialComp3(materialComp);
					}
				}

				if (ImGui::TableSetColumnIndex(5))
				{
					if (ImGui::SmallButton(("-##" + flag + "delete_button").c_str()))
						m_layers.erase(m_layers.begin() + i);
				}
			}


			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

void TerrainComp::Brush(ModelNode* node)
{
	ImGuiPayload* payload = nullptr;
	wstring filePath = L"";
	float textWidth = 180.0f;
	bool isChanged = false;
	ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Brush", treeFlag))
	{
		ImGui::Checkbox("IsBrush", &m_isBrushMode);

		ImGui::TreePop();
	}
}

XMFLOAT3 TerrainComp::GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix)
{
	int tmX, tmY;
	XMFLOAT3 result;
	XMVECTOR A, B, C;

	ImGuIRenderClass::GetInst()->GetMousePosInViewPort(tmX, tmY);

	// 반직선 생성
	CollisionClass::GetInst()->SetRay(tmX, tmY, viewMatrix, Core::GetProjectionMatrix(), cameraPos);
	XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);

	float minDistance = INFINITY;

	XMVECTOR rayOrigin, rayDir;

	// Picking Test
	rayOrigin = CollisionClass::GetInst()->GetRayOrigin();  // World Sapce
	rayDir = CollisionClass::GetInst()->GetRayDir();  // View Space
	rayDir = XMVector3TransformNormal(rayDir, invViewMatrix);  // View Space to World Sapce

	XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

	XMVECTOR rayOriginTemp, rayDirTemp;

	rayOriginTemp = XMVector3TransformCoord(rayOrigin, invWorldMatrix);  // World Space to Local Space
	rayDirTemp = XMVector3TransformNormal(rayDir, invWorldMatrix);  // World Space to Local Space

	rayDirTemp = XMVector3Normalize(rayDirTemp);

	TerrainVertexType* vertexArray = m_terrainMesh->GetVertexArray();
	int vertexCount = m_terrainMesh->GetVertexCount();
	for (int k = 0; k < vertexCount / 3; k++)
	{
		XMFLOAT4 plane;

		int i0 = k * 3 + 0;
		int i1 = k * 3 + 1;
		int i2 = k * 3 + 2;

		// 삼각형의 정점들
		A = XMLoadFloat3(&vertexArray[i0].position);
		B = XMLoadFloat3(&vertexArray[i1].position);
		C = XMLoadFloat3(&vertexArray[i2].position);

		float t = 0.0f;
		if (CollisionClass::GetInst()->CheckPickingTriangle(rayOriginTemp, rayDirTemp, A, B, C, t))
		{
			cout << "p" << endl;
		}
	}

	XMStoreFloat3(&result, XMVectorAdd(rayOrigin, rayDir * 2));

	return result;
}

wstring TerrainComp::ProcessDragAndDropPayloadTexture(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;
	wstring fileExtension = Core::GetFileExtension(fileRelativePath);

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

	wstring fileExtension = Core::GetFileExtension(fileRelativePath);

	if (fileExtension == L"material")
		result = fileRelativePath;

	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// Material Layer //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
MaterialLayer::MaterialLayer()
	: m_maskID(L""), m_material1(nullptr), m_material2(nullptr), m_material3(nullptr)
{
}

MaterialLayer::~MaterialLayer()
{
}

MaterialLayer::MaterialLayer(const MaterialLayer&)
{
}

void MaterialLayer::Shutdown()
{
}
