#include "pch.h"
#include "TransformComp.h"
#include "Core.h"

TransformComp::TransformComp()
{
	m_componentType = COMPONENT_TYPE::TRANSFORM;
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

TransformComp::TransformComp(const TransformComp& transformComp)
{
	m_componentType = COMPONENT_TYPE::TRANSFORM;
	m_position = transformComp.m_position;
	m_rotation = transformComp.m_rotation;
	m_scale = transformComp.m_scale;

}

TransformComp::TransformComp(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	m_componentType = COMPONENT_TYPE::TRANSFORM;

	m_position = position;
	m_rotation = rotation;
	m_scale = scale;
}

TransformComp::~TransformComp()
{
}

XMMATRIX TransformComp::GetWorldMatrix()
{
	// 이동행렬 생성
	XMMATRIX translateMatrix;
	translateMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	// 회전행렬 생성
	XMMATRIX rotationMatrix;

	// 쿼터니언을 사용하여 회전행렬을 만들어주자.
	// Roll: x축 기준으로 회전, Pitch: y축 기준으로 회전,  Yaw : z축 기준으로 회전
	XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	XMMATRIX rotZ = XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

	//XMVECTOR vec = XMVectorSet(XMConvertToRadians(m_rotation.x), XMConvertToRadians(m_rotation.y), XMConvertToRadians(m_rotation.z), 1.0f);
	//XMVECTOR quternion = XMQuaternionRotationRollPitchYawFromVector(vec);
	//rotationMatrix = XMMatrixRotationQuaternion(quternion);

	rotationMatrix = rotX * rotY * rotZ;

	// 비례행렬 생성
	XMMATRIX scaleMatrix;
	scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	// 비례, 회전, 이동 행렬 순으로 곱하여 월드행렬을 구해준다.
	return scaleMatrix * rotationMatrix * translateMatrix;
}

XMMATRIX TransformComp::GetTranslationMatrix()
{
	return XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

XMMATRIX TransformComp::GetRotationMatrix()
{
	XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	XMMATRIX rotZ = XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

	return  rotX * rotY * rotZ;
}

XMVECTOR TransformComp::GetQuaternion()
{
	return XMQuaternionRotationMatrix(GetRotationMatrix());
}

XMMATRIX TransformComp::GetScaleMatrix()
{
	return XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
}

void TransformComp::Render(ModelNode* node)
{
	char buff[255];   // ImGui InputTextBox에 들어갈 문자열 버퍼의 크기
	bool IsChanged;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_CharsDecimal;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Dummy(ImVec2(1.0f, 10.0f));
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(5.f, 0.0f));
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_position.x);   // float -> char*
		IsChanged = ImGui::InputText("##PosX", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_position.x);
			SetPosition(XMFLOAT3(m_position.x, m_position.y, m_position.z));
		}
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_position.y);
		IsChanged = ImGui::InputText("##PosY", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_position.y);
			SetPosition(XMFLOAT3(m_position.x, m_position.y, m_position.z));


		}
		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_position.z);
		IsChanged = ImGui::InputText("##PosZ", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_position.z);
			SetPosition(XMFLOAT3(m_position.x, m_position.y, m_position.z));
		}

		ImGui::Dummy(ImVec2(1.0f, 10.0f));
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();

		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(5.f, 0.0f));
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_rotation.x);   // float -> char*
		IsChanged = ImGui::InputText("##RotX", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_rotation.x);
			SetRotation(XMFLOAT3(m_rotation.x, m_rotation.y, m_rotation.z));
		}
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_rotation.y);
		IsChanged = ImGui::InputText("##RotY", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_rotation.y);
			SetRotation(XMFLOAT3(m_rotation.x, m_rotation.y, m_rotation.z));
		}
		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_rotation.z);
		IsChanged = ImGui::InputText("##RotZ", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_rotation.z);
			SetRotation(XMFLOAT3(m_rotation.x, m_rotation.y, m_rotation.z));
		}

		ImGui::Dummy(ImVec2(1.0f, 10.0f));
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(26.f, 0.0f));
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_scale.x);   // float -> char*
		IsChanged = ImGui::InputText("##ScaX", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_scale.x);
			SetScale(XMFLOAT3(m_scale.x, m_scale.y, m_scale.z));
		}
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_scale.y);
		IsChanged = ImGui::InputText("##ScaY", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_scale.y);
			SetScale(XMFLOAT3(m_scale.x, m_scale.y, m_scale.z));

		}
		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushItemWidth(50.0f);
		sprintf_s(buff, "%.2f", m_scale.z);
		IsChanged = ImGui::InputText("##ScaZ", buff, sizeof(buff), input_text_flags);
		if (IsChanged) {
			Core::ConvertData(buff, m_scale.z);
			SetScale(XMFLOAT3(m_scale.x, m_scale.y, m_scale.z));
		}
	}
}
