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

TerrainComp::TerrainComp()
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
	m_isRender = true;
	m_isWireFrame = false;
	m_isLOD = false;
	m_heightMapTexture = nullptr;
	m_isEditMode = false;

	// Brush 
	m_selected_layer = -1;
	m_brush.brushType = 0;
	m_brush.brushPosition = XMFLOAT3();
	m_brush.brushRange = 20.0f;
	m_brush.brushColor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_brush.chanel = XMFLOAT3();

	m_computeShader = 0;
	m_structureBuffer = 0;
	m_input = 0;
	m_output = 0;

	CreateComputeShader();
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


	// Brush
	TerrainVertexType* vertexArray = m_terrainMesh->GetVertexArray();
	int size = m_terrainMesh->GetVertexCount() / 3;
	InputDesc* m_input = new InputDesc[size];
	m_output = new OutputDesc[size];
	for (int i = 0; i < size; i++)
	{
		UINT index0 = i* 3 + 0;
		UINT index1 = i * 3 + 1;
		UINT index2 = i * 3 + 2;

		m_input[i].v0 = vertexArray[index0].position;
		m_input[i].v1 = vertexArray[index1].position;
		m_input[i].v2 = vertexArray[index2].position;

		m_input[i].index = i;
	}
	m_structureBuffer = new StructureBuffer(m_input, sizeof(InputDesc), size, sizeof(OutputDesc), size);

	m_alphaMapBoard = new RenderTextureClass;
	if (!m_alphaMapBoard)
		return false;

	float screenWidth = 100.f;
	float screenHeight = 100.f;

	result = m_alphaMapBoard->Initialize(Core::GetDevice(), Core::GetDeviceContext(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	if (!result)
		return false;

	m_bitmap = new BitMapClass;
	if (!m_bitmap)
		return false;

	result = m_bitmap->Initialize(Core::GetDevice(), 100.f, 100.f);
	if (!result)
		return false;

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
		if(m_isEditMode)
			Brush(node);
	}
}

void TerrainComp::RederTerrain(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX baseViewMatrix, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lihgtDirection, XMFLOAT3 cameraPos)
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

	// Brush Mode
	if (m_isEditMode)
	{
		if (GetBrushPosition(worldMatrix, cameraPos, viewMatrix, m_brush.brushPosition))
		{
			XMStoreFloat3(&m_brush.brushPosition, XMVector3TransformCoord(XMLoadFloat3(&m_brush.brushPosition), worldMatrix));
			PaintBrush(baseViewMatrix);
		}
	}

	GraphicsClass::GetInst()->RenderTerrainShaderSetParam(Core::GetDeviceContext(), m_isWireFrame, m_isLOD, worldMatrix, viewMatrix, lightDiffuseColor, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), lihgtDirection, cameraPos, resourceViews, chanelFlag, m_brush.brushType, m_brush.brushPosition, m_brush.brushRange, m_brush.brushColor);

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
		ImGui::SameLine();
		if (ImGui::Button("Edit Layer"))
		{
			if (m_selected_layer != -1 && !m_isEditMode)
			{
				m_isEditMode = true;
				m_brush.brushType = 1;
			}
			else if(m_selected_layer != -1 && m_isEditMode)
			{	
				m_isEditMode = false;
				m_brush.brushType = 0;
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
					if (!m_isEditMode)
					{
						if (m_selected_layer == i) m_selected_layer = -1;
						else m_selected_layer = i;
					}
				}

				ImGui::TableSetColumnIndex(1);
				ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(60.f, ImGui::GetItemRectSize().y));
				string label = m_layers[i]->GetMaskID() == L"" ? "NONE##Mask" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaskID() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
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
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
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
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
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
				if (ImGui::Selectable(label.c_str(), (m_selected_layer == i), 0))
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
		int* type = &m_brush.brushType;
		FLOAT* range = &m_brush.brushRange;
		XMFLOAT3* color = &m_brush.brushColor;
		XMFLOAT3* chanel = &m_brush.chanel;

		TextureClass* texture = ResMgrClass::GetInst()->FindTexture(m_layers[m_selected_layer]->GetMaskID());
		if(texture == nullptr)
			ImGui::Image(nullptr, ImVec2(200.0f, 200.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(3, 3, 3, 3));
		else
			ImGui::Image((ImTextureID)texture->GetTexture(), ImVec2(200.0f, 200.0f), ImVec2(0,0), ImVec2(1, 1), ImVec4(1,1,1,1), ImVec4(3,3,3,3));

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

		if(m_alphaMapBoard->GetShaderResourceView() != nullptr)
			ImGui::Image((ImTextureID)m_alphaMapBoard->GetShaderResourceView(), ImVec2(200.0f, 200.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(3, 3, 3, 3));
		
		ImGui::TreePop();
	}
}

bool TerrainComp::GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix, XMFLOAT3& position)
{
	bool result = false;
	int tmX, tmY;
	XMVECTOR A, B, C;

	ImGuIRenderClass::GetInst()->GetMousePosInViewPort(tmX, tmY);

	// 반직선 생성
	CollisionClass::GetInst()->SetRay(tmX, tmY, viewMatrix, Core::GetProjectionMatrix(), cameraPos);
	XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

	XMVECTOR rayOrigin, rayDir;

	// Picking Test
	rayOrigin = CollisionClass::GetInst()->GetRayOrigin();  // World Sapce
	rayDir = CollisionClass::GetInst()->GetRayDir();  // View Space
	rayDir = XMVector3TransformNormal(rayDir, invViewMatrix);  // View Space to World Sapce
	rayDir = XMVector3Normalize(rayDir);

	rayOrigin = XMVector3TransformCoord(rayOrigin, invWorldMatrix);  // World Space to Local Space
	rayDir = XMVector3TransformNormal(rayDir, invWorldMatrix);  // World Space to Local Space
	rayDir = XMVector3Normalize(rayDir);

	XMFLOAT3 pos, dir;
	XMStoreFloat3(&pos, rayOrigin);
	XMStoreFloat3(&dir, rayDir);

	// 셰이더와 상수버퍼 설정
	m_computeShader->SetShader();
	int triangleSize = m_terrainMesh->GetVertexCount() / 3;
	m_computeShader->SetConstantBuffer(pos, triangleSize, dir);

	// CS에서 사용하는 구조체 버퍼를 설정한다.
	Core::GetDeviceContext()->CSSetShaderResources(0, 1, &m_structureBuffer->GetSRV());  // 입력자원의 경우 셰이더 자원 뷰를 생성한다.
	Core::GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_structureBuffer->GetUAV(), nullptr);  // 출력 자원의 경우 순서 없는 접근 뷰를 생성한다.

	UINT x = ceil((float)triangleSize / 1024.0f);

	// Dispatch 함수를 통해 스레드 그룹들을 3차원 격자 형태로 구성한다. 
	Core::GetDeviceContext()->Dispatch(x, 1, 1);	

	// 셰이더가 실행되고 나면 출력값을 복사해온다.
	m_structureBuffer->Copy(m_output, sizeof(OutputDesc) * triangleSize);

	float minDistance = FLT_MAX;
	int minIndex = -1;

	for (UINT i = 0; i < triangleSize; i++)
	{
		OutputDesc temp = m_output[i];
		if (temp.picked)
		{
			if (minDistance > temp.distance)
			{
				minDistance = temp.distance;
				minIndex = i;
			}
		}
	}

	if (minIndex >= 0)
	{
		XMStoreFloat3(&position, XMVectorAdd(rayOrigin, rayDir * minDistance));
		result = true;
	}
	
	return result;

}

void TerrainComp::CreateComputeShader()
{
	wstring filePath = PathMgr::GetInst()->GetContentPath();
	wstring path;
	
	path = filePath + L"contents\\shader\\ComputeShader.hlsl";
	m_computeShader = new ComputePickingShader(path);

}


void TerrainComp::PaintBrush(XMMATRIX baseViewMatrix)
{
	if (MOUSE_HOLD(0))
	{
		TextureClass* alphaMap = ResMgrClass::GetInst()->FindTexture(m_layers[m_selected_layer]->GetMaskID());

		XMFLOAT2 uv = CalculateUV(m_brush.brushPosition, m_terrainMesh->GetTerrainWidth(), m_terrainMesh->GetTerrainHeight());
		float range = m_brush.brushRange / (m_terrainMesh->GetTerrainWidth());

		m_alphaMapBoard->RenderToTextureStart(Core::GetDeviceContext());

		GraphicsClass::GetInst()->TurnZBufferOff();

		m_bitmap->Render(Core::GetDeviceContext());

		GraphicsClass::GetInst()->RenderTerrainPaintShader(Core::GetDeviceContext(), m_bitmap->GetIndexCount(), XMMatrixIdentity(), baseViewMatrix, m_alphaMapBoard->GetOrthoMatirx(), m_brush.brushType, uv, range, m_brush.chanel, alphaMap->GetTexture());

		Core::GetDeviceContext()->CopyResource((ID3D11Resource*)alphaMap->GetTexture(), (ID3D11Resource*)m_alphaMapBoard->GetShaderResourceView());

		GraphicsClass::GetInst()->TurnZBufferOn();

		m_alphaMapBoard->RenderToTextureEnd();

		ImGuIRenderClass::GetInst()->SetRenderToTexture(Core::GetDeviceContext());
	}
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

XMFLOAT2 TerrainComp::CalculateUV(XMFLOAT3 position, float width, float heigth)
{
	float percentX, percentY;

	if (position.x < 0)
		position.x = 0;

	if (position.z < 0)
		position.y = 0;

	if (position.x > width)
		position.x = width;

	if (position.z > heigth)
		position.y = heigth;

	// Terrain 사이즈를 기준으로 현재위치의 비율값 즉 uv를 구한다.
	percentX = position.x / width;
	percentY = 1.0f - (position.z / heigth);

	return XMFLOAT2(percentX, percentY);
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
