#include "pch.h"
#include "TerrainComp.h"
#include "Core.h"

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
	float textWidth = 180.0f;
	bool isChanged = false;

	if (ImGui::CollapsingHeader("Terrain Renderer", &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Height Map");

		ImGui::SetNextItemWidth(200.0f);
		ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
		ImGui::SameLine();
		ImGui::InputText("##meshName", (char*)m_heightMapName.c_str(), sizeof(m_heightMapName));
	}
}
