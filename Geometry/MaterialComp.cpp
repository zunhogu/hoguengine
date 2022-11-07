#include "pch.h"
#include "MaterialComp.h"
#include "Core.h"
#include "ResMgrClass.h"
#include "TextureClass.h"
#include "ContentBrowserPanel.h"
#include "ModelNode.h"

extern ImVec2 g_viewPortSize;

MaterialComp::MaterialComp()
{
	m_componentType = COMPONENT_TYPE::MATERIAL;

	m_renderTexture = nullptr;

	Initialize();
}

MaterialComp::MaterialComp(const MaterialComp& material)
{
	m_componentType = COMPONENT_TYPE::MATERIAL;

	m_materialName = material.m_materialName;

	m_material = material.m_material;

	Initialize();
}


MaterialComp::~MaterialComp()
{
}

bool MaterialComp::Initialize()
{
	bool result;

	m_renderTexture = new RenderTextureClass;
	if (!m_renderTexture)
		return false;

	float screenWidth = 100.f;
	float screenHeight = 100.f;

	result = m_renderTexture->Initialize(Core::GetDevice(), Core::GetDeviceContext(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	if (!result)
		return false;

	m_bitmap = new BitMapClass;
	if (!m_bitmap)
		return false;


	result = m_bitmap->Initialize(Core::GetDevice(),100.f, 100.f);
	if (!result)
		return false;

	return true;
}

void MaterialComp::Shutdown()
{
	if (m_bitmap)
	{
		m_bitmap->Shutdown();
		delete m_bitmap;
		m_bitmap = 0;
	}

	if (m_renderTexture)
	{
		m_renderTexture->Shutdown();
		delete m_renderTexture;
		m_renderTexture = 0;
	}

	m_material = nullptr;
}

void MaterialComp::Render(ModelNode* node)
{
	wstring collapsingHeadrName = L"Material : " + m_materialName;
	if (ImGui::CollapsingHeader(Core::ConvWcharTochar(m_materialName), &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool isChanged = false;

		ImGui::Dummy(ImVec2(0.0f, 4.0f));  // padding
		ImGui::Dummy(ImVec2(1.0f, 0.0f));  // padding
		ImGui::SameLine();

		ImGui::BeginGroup();

		ImGui::Image((ImTextureID)m_renderTexture->GetShaderResourceView(), ImVec2(100.0f, 100.0f));
		ImGui::SameLine();

		char buffer[255];
		char buffer2[255] = "Material##";
		strcpy(buffer, Core::ConvWcharTochar(m_materialName));
		strcat(buffer2, Core::ConvWcharTochar(m_materialName));

		ImGui::SetNextItemWidth(150.0f);
		isChanged = ImGui::InputText(buffer2, buffer, sizeof(buffer));
		if (isChanged)
		{
		}
		ImGui::Dummy(ImVec2(0.0f, 2.0f));


		//ImGui::SameLine();
		//ImGui::AlignTextToFramePadding();  // 텍스트 정렬
		//ImGui::Text("Shader   ");
		//ImGui::SameLine();
		//ImGui::Dummy(ImVec2(1.0f, 0.0f));
		//ImGui::SameLine();

		//char buffer3[255] = "Shader";
		//char buffer4[255] = "##Shader_";
		//strcpy(buffer, Core::ConvWcharTochar(m_materialName));
		//strcat(buffer2, Core::ConvWcharTochar(m_materialName));

		//ImGui::SetNextItemWidth(200.0f);
		//isChanged = ImGui::InputText(buffer4, buffer3, sizeof(buffer3));
		//if (isChanged)
		//{
		//}
		//ImGui::Dummy(ImVec2(0.0f, 2.0f));
		
		ImGui::EndGroup();

		// Detail
		{
			ImGuiPayload* payload = nullptr;
			float textWidth = 180.0f;
			wstring filePath = L"";
			ImVec2 texturePos;

			ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth;
			wstring treeName = L"Detail##" +  m_materialName;
			if (ImGui::TreeNodeEx(Core::ConvWcharTochar(treeName), treeFlag))
			{
				// color
				static bool alpha_preview = true;
				static bool alpha_half_preview = false;
				static bool drag_and_drop = true;
				static bool options_menu = true;
				static bool hdr = false;
				ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

				ImVec4 imAmbientColor = ImVec4(m_material->GetAmbientColor().x,
											   m_material->GetAmbientColor().y,
											   m_material->GetAmbientColor().z,
											   m_material->GetAmbientColor().w);
				ImGui::Text("AmbientColor ");
				ImGui::SameLine();
				ImGui::PushItemWidth(150.0f);
				wstring str = L"##AmbientColor_" + m_materialName;
				isChanged = ImGui::ColorEdit3(Core::ConvWcharTochar(str), (float*)&imAmbientColor, misc_flags);
				if (isChanged)
				{
					m_material->SetAmibientColor(XMFLOAT4(imAmbientColor.x, imAmbientColor.y, imAmbientColor.z, imAmbientColor.w));
				}
				ImVec4 imEmissiveColor = ImVec4(m_material->GetEmissiveColor().x,
												m_material->GetEmissiveColor().y,
												m_material->GetEmissiveColor().z,
												m_material->GetEmissiveColor().w);
				ImGui::Text("EmissiveColor");
				ImGui::SameLine();
				wstring str2 = L"##EmissiveColor_" + m_materialName;
				isChanged = ImGui::ColorEdit3(Core::ConvWcharTochar(str2), (float*)&imEmissiveColor, misc_flags);
				if (isChanged)
				{
					m_material->SetEmissiveColor(XMFLOAT4(imEmissiveColor.x, imEmissiveColor.y, imEmissiveColor.z, imEmissiveColor.w));
				}
				ImVec4 imDiffuseColor = ImVec4(m_material->GetDiffuseColor().x,
											   m_material->GetDiffuseColor().y,
											   m_material->GetDiffuseColor().z,
											   m_material->GetDiffuseColor().w);
				ImGui::Text("DiffuseColor ");
				ImGui::SameLine();
				wstring str3 = L"##DiffuseColor_" + m_materialName;
				isChanged = ImGui::ColorEdit3(Core::ConvWcharTochar(str3), (float*)&imDiffuseColor, misc_flags);
				if (isChanged)
				{
					m_material->SetDiffuseColor(XMFLOAT4(imDiffuseColor.x, imDiffuseColor.y, imDiffuseColor.z, imDiffuseColor.w));
				}
				ImVec4 imSpecularColor = ImVec4(m_material->GetSpecularColor().x,
												m_material->GetSpecularColor().y,
												m_material->GetSpecularColor().z,
												m_material->GetSpecularColor().w);
				ImGui::Text("SpecularColor");
				ImGui::SameLine();
				wstring str4 = L"##SpecularColor_" + m_materialName;
				isChanged = ImGui::ColorEdit3(Core::ConvWcharTochar(str4), (float*)&imSpecularColor, misc_flags);
				if (isChanged)
				{
					m_material->SetSpecularColor(XMFLOAT4(imSpecularColor.x, imSpecularColor.y, imSpecularColor.z, imSpecularColor.w));
				}

				ImGui::Text("Shinness     ");
				ImGui::SameLine();
				wstring str5 = L"##shinness" + m_materialName;
				float shinness = m_material->GetShinness();
				isChanged = ImGui::SliderFloat(Core::ConvWcharTochar(str5), &shinness, 0.0f, 1.0f);
				if (isChanged)
				{
					m_material->SetShinness(shinness);
				}

				//ImGui::Dummy(ImVec2(0, 10.0f));

				char buff[255];
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_CharsDecimal;

				// texture

				// Ambient Texture ////////////////////////////////////////////////
				ImGui::BeginChild("AmbientTextureLeftPane", ImVec2(220, 70));
				ImGui::Text("AmbientTexture");
				ImGui::AlignTextToFramePadding();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("Tiling ");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);   // float -> char*
				isChanged = ImGui::InputText(("##TileX" + m_material->GetAmbientTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);
				isChanged = ImGui::InputText(("##TileY" + m_material->GetAmbientTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::Dummy(ImVec2(10.0f, 10.0f));
				ImGui::SameLine();
				ImGui::Text("Offsets");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);   // float -> char*
				isChanged = ImGui::InputText(("##OffsetX" + m_material->GetAmbientTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);
				isChanged = ImGui::InputText(("##OffsetY" + m_material->GetAmbientTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
				ImGui::BeginChild("AmbientTextureRightPane", ImVec2(70.0f, 70.0f), true);
				TextureClass* texture = m_material->GetAmbientTextureID() != "NONE" ? ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetAmbientTextureID().c_str())) : ResMgrClass::GetInst()->FindTexture(L"defaultTexture");

				texturePos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
				ImGui::SetItemAllowOverlap();
				ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(70.0f, 70.0f));
				ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(70.0f, 70.0f));
				ImGui::SameLine();
				ImGui::SetCursorPos(texturePos);
				ImGui::Dummy(ImVec2(70.0f, 70.0f));
				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");

				filePath = Core::ProcessDragAndDropPayloadTexture(payload);
				if (filePath != L"")
				{
					ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(filePath), filePath);
					m_material->SetAmibentTextureID(Core::ConvWcharTochar(Core::GetFileName(filePath)));
				}
				ImGui::PopStyleVar();
				ImGui::EndChild();
				////////////////////////////////////////////////////////////////////

				// Emissive Texture ////////////////////////////////////////////////
				ImGui::BeginChild("EmissiveTextureLeftPane", ImVec2(220, 70));
				ImGui::Text("EmissiveTexture");
				ImGui::AlignTextToFramePadding();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("Tiling ");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);   // float -> char*
				isChanged = ImGui::InputText(("##TileX" + m_material->GetEmissiveTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);
				isChanged = ImGui::InputText(("##TileY" + m_material->GetEmissiveTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::Dummy(ImVec2(10.0f, 10.0f));
				ImGui::SameLine();
				ImGui::Text("Offsets");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);   // float -> char*
				isChanged = ImGui::InputText(("##OffsetX" + m_material->GetEmissiveTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);
				isChanged = ImGui::InputText(("##OffsetY" + m_material->GetEmissiveTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
				ImGui::BeginChild("EmissiveTextureRightPane", ImVec2(70.0f, 70.0f), true);
				texture = m_material->GetEmissiveTextureID() != "NONE" ? ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetEmissiveTextureID().c_str())) : ResMgrClass::GetInst()->FindTexture(L"defaultTexture");

				texturePos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
				ImGui::SetItemAllowOverlap();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(70.0f, 70.0f));
				ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(70.0f, 70.0f));
				ImGui::SameLine();
				ImGui::SetCursorPos(texturePos);
				ImGui::Dummy(ImVec2(70.0f, 70.0f));
				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");

				filePath = Core::ProcessDragAndDropPayloadTexture(payload);
				if (filePath != L"")
				{
					ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(filePath), filePath);
					m_material->SetEmissiveTextureID(Core::ConvWcharTochar(Core::GetFileName(filePath)));
				}
				
				ImGui::PopStyleVar();
				ImGui::EndChild();
				///////////////////////////////////////////////////////////////////

				// Diffuse Texture ////////////////////////////////////////////////
				ImGui::BeginChild("DiffuseTextureLeftPane", ImVec2(220, 70));
				ImGui::Text("DiffuseTexture");
				ImGui::AlignTextToFramePadding();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("Tiling ");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);   // float -> char*
				isChanged = ImGui::InputText(("##TileX" + m_material->GetDiffuseTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);
				isChanged = ImGui::InputText(("##TileY" + m_material->GetDiffuseTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::Dummy(ImVec2(10.0f, 10.0f));
				ImGui::SameLine();
				ImGui::Text("Offsets");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);   // float -> char*
				isChanged = ImGui::InputText(("##OffsetX" + m_material->GetDiffuseTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);
				isChanged = ImGui::InputText(("##OffsetY" + m_material->GetDiffuseTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
				ImGui::BeginChild("DiffuseTextureRightPane", ImVec2(70.0f, 70.0f), true);
				texture = m_material->GetDiffuseTextureID() != "NONE" ? ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetDiffuseTextureID().c_str())) : ResMgrClass::GetInst()->FindTexture(L"defaultTexture");
				
				texturePos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
				ImGui::SetItemAllowOverlap();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(70.0f, 70.0f));
				ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(70.0f, 70.0f));
				ImGui::SameLine();
				ImGui::SetCursorPos(texturePos);
				ImGui::Dummy(ImVec2(70.0f, 70.0f));
				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");

				filePath = Core::ProcessDragAndDropPayloadTexture(payload);
				if (filePath != L"")
				{
					ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(filePath), filePath);
					m_material->SetDiffuseTextureID(Core::ConvWcharTochar(Core::GetFileName(filePath)));
				}

				ImGui::PopStyleVar();
				ImGui::EndChild();
				////////////////////////////////////////////////////////////////////

				// Specular Texture ////////////////////////////////////////////////
				ImGui::BeginChild("SpecularTextureLeftPane", ImVec2(220, 70));
				ImGui::Text("SpecularTexture");
				ImGui::AlignTextToFramePadding();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("Tiling ");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);   // float -> char*
				isChanged = ImGui::InputText(("##TileX" + m_material->GetSpecularTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);
				isChanged = ImGui::InputText(("##TileY" + m_material->GetSpecularTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::Dummy(ImVec2(10.0f, 10.0f));
				ImGui::SameLine();
				ImGui::Text("Offsets");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);   // float -> char*
				isChanged = ImGui::InputText(("##OffsetX" + m_material->GetSpecularTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);
				isChanged = ImGui::InputText(("##OffsetY" + m_material->GetSpecularTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
				ImGui::BeginChild("SpecularTextureRightPane", ImVec2(70.0f, 70.0f), true);
				texture = m_material->GetSpecularTextureID() != "NONE" ? ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetSpecularTextureID().c_str())) : ResMgrClass::GetInst()->FindTexture(L"defaultTexture");
				
				texturePos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
				ImGui::SetItemAllowOverlap();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(70.0f, 70.0f));
				ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(70.0f, 70.0f));
				ImGui::SameLine();
				ImGui::SetCursorPos(texturePos);
				ImGui::Dummy(ImVec2(70.0f, 70.0f));
				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");

				filePath = Core::ProcessDragAndDropPayloadTexture(payload);
				if (filePath != L"")
				{
					ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(filePath), filePath);
					m_material->SetSpecularTextureID(Core::ConvWcharTochar(Core::GetFileName(filePath)));
				}
				ImGui::PopStyleVar();
				ImGui::EndChild();
				////////////////////////////////////////////////////////////////////

				// NormalMap Texture ////////////////////////////////////////////////
				ImGui::BeginChild("NormalMapTextureLeftPane", ImVec2(220, 70));
				ImGui::Text("NormalMapTexture");
				ImGui::AlignTextToFramePadding();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("Tiling ");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);   // float -> char*
				isChanged = ImGui::InputText(("##TileX" + m_material->GetNormalTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 1.0f);
				isChanged = ImGui::InputText(("##TileY" + m_material->GetNormalTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::Dummy(ImVec2(10.0f, 10.0f));
				ImGui::SameLine();
				ImGui::Text("Offsets");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(10.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);   // float -> char*
				isChanged = ImGui::InputText(("##OffsetX" + m_material->GetNormalTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.x);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				sprintf_s(buff, "%.2f", 0.0f);
				isChanged = ImGui::InputText(("##OffsetY" + m_material->GetNormalTextureID()).c_str(), buff, sizeof(buff), input_text_flags);
				if (isChanged) {
					//Core::ConvertData(buff, m_tiling.y);
					//SetTiling(XMFLOAT2(m_tiling.x, m_tiling.y));
				}

				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
				ImGui::BeginChild("NormalMapTextureRightPane", ImVec2(70.0f, 70.0f), true);
				texture = m_material->GetNormalTextureID() != "NONE" ? ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetNormalTextureID().c_str())) : ResMgrClass::GetInst()->FindTexture(L"defaultTexture");

				texturePos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
				ImGui::SetItemAllowOverlap();
				rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(70.0f, 70.0f));
				ImGui::Image((ImTextureID)texture->GetShaderResourceView(), ImVec2(70.0f, 70.0f));
				ImGui::SameLine();
				ImGui::SetCursorPos(texturePos);
				ImGui::Dummy(ImVec2(70.0f, 70.0f));
				payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
				filePath = Core::ProcessDragAndDropPayloadTexture(payload);
				if (filePath != L"")
				{
					ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(filePath), filePath);
					m_material->SetNormalTextureID(Core::ConvWcharTochar(Core::GetFileName(filePath)));
				}
				ImGui::PopStyleVar();
				ImGui::EndChild();
				////////////////////////////////////////////////////////////////////

				ImGui::TreePop();
			}
		}
	}
}

bool MaterialComp::RenderMaterial(ID3D11DeviceContext* deviceContext, XMMATRIX baseViewMatrix)
{
	bool result;

	m_renderTexture->RenderToTextureStart(Core::GetDeviceContext());

	// 2D 렌더링을 시작하기 전에 Z버퍼를 끈다. 
	GraphicsClass::GetInst()->TurnZBufferOff();

	result = m_bitmap->Render(Core::GetDeviceContext());
	if (!result)
		return false;

	m_material->GetAmbientTextureID();
	
	TextureClass* defaultTexture = ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), L"defaultTexture");
	ID3D11ShaderResourceView* textures[5] = { defaultTexture->GetShaderResourceView(), defaultTexture->GetShaderResourceView(), defaultTexture->GetShaderResourceView(), defaultTexture->GetShaderResourceView(), defaultTexture->GetShaderResourceView() };
	XMFLOAT4 ambientColor, emissiveColor, diffuseColor, specularColor;
	float shiness;

	ambientColor = m_material->GetAmbientColor();
	emissiveColor = m_material->GetEmissiveColor();
	diffuseColor = m_material->GetDiffuseColor();
	specularColor = m_material->GetSpecularColor();
	shiness = m_material->GetShinness();

	if (m_material->GetAmbientTextureID() != "NONE")
		textures[0] = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetAmbientTextureID().c_str()))->GetShaderResourceView();
	if (m_material->GetEmissiveTextureID() != "NONE")
		textures[1] = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetEmissiveTextureID().c_str()))->GetShaderResourceView();
	if (m_material->GetDiffuseTextureID() != "NONE")
		textures[2] = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetDiffuseTextureID().c_str()))->GetShaderResourceView();
	if (m_material->GetSpecularTextureID() != "NONE")
		textures[3] = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetSpecularTextureID().c_str()))->GetShaderResourceView();
	if (m_material->GetNormalTextureID() != "NONE")
		textures[4] = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)m_material->GetNormalTextureID().c_str()))->GetShaderResourceView();

	GraphicsClass::GetInst()->RenderMaterialShader(deviceContext, m_bitmap->GetIndexCount(), XMMatrixIdentity(), baseViewMatrix, m_renderTexture->GetOrthoMatirx(), ambientColor, emissiveColor, diffuseColor, specularColor, shiness, textures);

	// 2D 렌더링이 끝나면 3D 객체를 그리기위해서 Z버퍼를 다시 켠다.
	GraphicsClass::GetInst()->TurnZBufferOn();

	m_renderTexture->RenderToTextureEnd();

	ImGuIRenderClass::GetInst()->SetRenderToTexture(Core::GetDeviceContext());

	return true;
}

ID3D11ShaderResourceView* MaterialComp::GetShaderResourceView()
{
	return m_renderTexture->GetShaderResourceView();
}
