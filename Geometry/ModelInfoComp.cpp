#include "pch.h"
#include "ModelInfoComp.h"
#include "Core.h"

ModelInfoComp::ModelInfoComp()
{
	m_componentType = COMPONENT_TYPE::MODELINFO;
	m_modelName = L"Empty Object";
	m_isActive = true;
	m_isFrustumCulling = true;
}

ModelInfoComp::ModelInfoComp(const ModelInfoComp& modelInfoComp)
{
	m_componentType = COMPONENT_TYPE::MODELINFO;
	m_modelName = modelInfoComp.m_modelName;
	m_isActive = true;
	m_isFrustumCulling = true;
}

ModelInfoComp::~ModelInfoComp()
{
}

void ModelInfoComp::Render(ModelNode* node)
{
	if (ImGui::CollapsingHeader("ModelInfo", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool isChanged = false;


		ImGui::Checkbox("IsActive", &m_isActive);
		ImGui::SameLine();
		ImGui::Checkbox("IsFrustum", &m_isFrustumCulling);

		ImGui::Dummy(ImVec2(0.0f, 4.0f));  // padding
		ImGui::Dummy(ImVec2(1.0f, 0.0f));  // padding
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();  // 텍스트 정렬
		ImGui::Text("Name  ");
		ImGui::SameLine();

		char buffer[255];
		strcpy(buffer, Core::ConvWcharTochar(m_modelName));

		isChanged = ImGui::InputText("##name", buffer, sizeof(buffer));  // ##을 붙이면 뒤에 쓰여지는 글자 name은 invisible 
		if (isChanged)
		{
			SetModelName(Core::ConvCharToWchar(buffer));
		}
		ImGui::Dummy(ImVec2(0.0f, 2.0f));
	}
}
