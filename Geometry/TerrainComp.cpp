#include "pch.h"
#include "TerrainComp.h"
#include "Core.h"
#include "ContentBrowserPanel.h"
#include "ResMgrClass.h"
#include "ModelNode.h"

TerrainComp::TerrainComp()
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
	m_terrainMesh = new TerrainMesh;
	m_terrainQuad = new TerrainQuadTreeClass;
}

TerrainComp::TerrainComp(const TerrainComp& terrain)
{
	m_componentType = COMPONENT_TYPE::TERRAIN;
}

TerrainComp::~TerrainComp()
{
}

bool TerrainComp::Initialize()
{
	bool result = true;
	
	result = m_terrainMesh->Initialize(Core::GetDevice());
	if (!result)
		return result;

	result = m_terrainQuad->Initialize(m_terrainMesh, Core::GetDevice());
	if (!result)
		return result;

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

void TerrainComp::RederMesh(XMMATRIX worldMatrix, XMMATRIX viewMatrix)
{
	XMMATRIX newWorldMatrix = XMMatrixMultiply(m_terrainMesh->GetWorldMatrix(), worldMatrix);

	GraphicsClass::GetInst()->RenderGridShaderSetParam(Core::GetDeviceContext(), newWorldMatrix, viewMatrix);

	m_terrainQuad->Render(Core::GetDeviceContext(), newWorldMatrix);
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
		char buffer[255];

		ImGui::Text("Width ");

		ImGui::SetNextItemWidth(100.f);
		rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		ImGui::SameLine();
		sprintf(buffer, "%d", m_terrainMesh->GetTerrainWidth());
		isChanged = ImGui::InputText("##terrain_width", buffer, sizeof(buffer));
		if (isChanged)
			m_terrainMesh->SetTerrainWidth(atoi(buffer));

		ImGui::SameLine();
		ImGui::Text("Height");

		ImGui::SetNextItemWidth(100.0f);
		rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		ImGui::SameLine();
		sprintf(buffer, "%d", m_terrainMesh->GetTerrainWidth());
		isChanged = ImGui::InputText("##terrain_height", buffer, sizeof(buffer));
		if (isChanged)
			m_terrainMesh->SetTerrainHeight(atoi(buffer));
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
				ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y));
				string label = m_layers[i]->GetMaskID() == L"" ? "NONE##Mask" + flag : Core::ConvWcharTochar(m_layers[i]->GetMaskID() + L"##") + flag;
				if (ImGui::Selectable(label.c_str(), (selected_item == i), 0))
				{
					if (m_layers[i]->GetMaskID() != L"")
						ImGui::OpenPopup(("##" + label + "mask_image").c_str());
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
					for (int j = 0; j < comps->size(); j++)
					{
						if (comps->at(j) == m_layers[i]->GetMaterialComp())
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
				isChanged = ImGui::ColorEdit4(("##" + flag + "color").c_str(), (float*)&chanel, misc_flags);
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
		ImGui::TreePop();
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


////////////////////////////////////////////////////////////////////////////////////////////
// Material Layer //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
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

void MaterialLayer::Shutdown()
{
}


////////////////////////////////////////////////////////////////////////////////////////////
// TERRAIN MESH ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
TerrainMesh::TerrainMesh()
	: m_terrainHeight(100), m_terrainWidth(100)
{
}

TerrainMesh::TerrainMesh(int width, int height)
{
	m_terrainWidth = width;
	m_terrainHeight = height;
}

TerrainMesh::~TerrainMesh()
{
}

TerrainMesh::TerrainMesh(const TerrainMesh&)
{
}

bool TerrainMesh::Initialize(ID3D11Device* device)
{
	m_worldMatrix = XMMatrixTranslation(-m_terrainWidth / 2, 0.0f, -m_terrainHeight / 2);

	bool result = true;
	result = InitializeBuffers(device);
	if (!result)
		return false;
}

void TerrainMesh::Shutdown()
{
	ShutdownBuffers();
}

int TerrainMesh::GetVertexCount()
{
	return m_vertexCount;
}

void TerrainMesh::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
	return;
}

bool TerrainMesh::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float positionX, positionZ;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 12;

	// Create the vertex array.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
		return false;

	// Initialize the index to the vertex array.
	index = 0;

	// Load the vertex and index arrays with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{

			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper left.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;
		}
	}

	return true;
}

void TerrainMesh::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}
}
