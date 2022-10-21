#include "pch.h"
#include "TerrainComp.h"

TerrainComp::TerrainComp()
{
}

TerrainComp::TerrainComp(const TerrainComp&)
{
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
