#include "pch.h"
#include "TerrainComp.h"

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

void TerrainComp::Shutdown()
{
}

void TerrainComp::Render(ModelNode* node)
{
	if (ImGui::CollapsingHeader("Terrain Renderer", &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
	}
}
