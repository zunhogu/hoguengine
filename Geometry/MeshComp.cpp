#include "pch.h"
#include "MeshComp.h"
#include "Core.h"
#include "ContentBrowserPanel.h"
#include "ModelNode.h"
#include "Material.h"
#include "ResMgrClass.h"
#include "Mesh.h"

MeshPartComp::MeshPartComp()
{
}

MeshPartComp::~MeshPartComp()
{
}

void MeshPartComp::Shutdown()
{
	//for (int i = 0; i < m_vertexCount; i++)
	//	delete m_vetexArray[i];

	//if (m_vertexBuffer)
	//{
	//	m_vertexBuffer->Release();
	//	delete m_vertexBuffer;
	//	m_vertexBuffer = 0;
	//}

	//if (m_indexBuffer)
	//{
	//	m_indexBuffer->Release();
	//	delete m_indexBuffer;
	//	m_indexBuffer = 0;
	//}
}

void MeshPartComp::Render(ID3D11DeviceContext* devicecContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	devicecContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	devicecContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	devicecContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

MeshComp::MeshComp()
{
	m_componentType = COMPONENT_TYPE::MESH;
	m_meshID = "";
	m_isRender = true;
	m_isDelete = true;
}

MeshComp::MeshComp(const MeshComp& meshComp)
{
	// Copy MeshComp Info
	m_componentType = COMPONENT_TYPE::MESH;
	m_meshID = meshComp.m_meshID;
	m_isRender = true;
	m_isDelete = true;

	// Copy MeshPartComp 
	vector<MeshPartComp*> meshPartsSrc = meshComp.m_meshParts;
	for (int i = 0; i < meshPartsSrc.size(); i++)
	{
		MeshPartComp* meshPartDes = new MeshPartComp;
		MeshPartComp* meshPartSrc = meshPartsSrc[i];

		meshPartDes->SetVertexBuffer(meshPartSrc->GetVertexBuffer());
		meshPartDes->SetIndexBuffer(meshPartSrc->GetIndexBuffer());

		vector<VertexType*> vertexArraySrc = *meshPartSrc->GetVertexArray();
		vector<VertexType*>* vertexArrayDes = meshPartDes->GetVertexArray();
		for (int j = 0; j < vertexArraySrc.size(); j++)
			vertexArrayDes->push_back(vertexArraySrc[j]);

		vector<UINT> indexArraySrc = *meshPartSrc->GetIndexArray();
		vector<UINT>* indexArrayDes = meshPartDes->GetIndexArray();
		for (int j = 0; j < vertexArraySrc.size(); j++)
			indexArrayDes->push_back(indexArraySrc[j]);

		meshPartDes->SetVertexCount(meshPartSrc->GetVertexCount());
		meshPartDes->SetIndexCount(meshPartSrc->GetIndexCount());
		meshPartDes->SetMaterial(meshPartSrc->GetMaterial());

		m_meshParts.push_back(meshPartDes);
	}

}

MeshComp::~MeshComp()
{
}

void MeshComp::Shutdown()
{
	for (int i = 0; i < m_meshParts.size(); i++)
	{
		m_meshParts[i]->Shutdown();
	}
}

void MeshComp::Render(ModelNode* node)
{
	if (ImGui::CollapsingHeader("Mesh Renderer", &m_isDelete, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("IsRender", &m_isRender);

		ImGuiPayload* payload = nullptr;
		float textWidth = 180.0f;
		bool isChanged = false;
		wstring filePath = L"";

		ImGui::Dummy(ImVec2(0.0f, 4.0f));  // padding
		ImGui::Dummy(ImVec2(1.0f, 0.0f));  // padding
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();  // 텍스트 정렬
		ImGui::Text("Mesh  ");
		ImGui::SameLine();

		char buffer[255];
		strcpy(buffer, m_meshID.c_str());

		ImGui::SetNextItemWidth(200.0f);
		ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));

		isChanged = ImGui::InputText("##meshName", buffer, sizeof(buffer));
		if (isChanged)
		{
		}
		payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
		filePath = ProcessDragAndDropPayloadMesh(payload);
		if (filePath != L"")
		{
			m_meshID = Core::ConvWcharTochar(Core::GetFileName(filePath));
			m_meshParts.clear();

			for (int i = node->GetModelComps()->size()-1; i >=0; i--)
			{
				if (node->GetModelComps()->at(i)->GetCompType() == COMPONENT_TYPE::MATERIAL)
					node->GetModelComps()->erase(node->GetModelComps()->begin() + i);
			}

			Mesh* mesh = ResMgrClass::GetInst()->LoadMesh(Core::GetDevice(), Core::GetFileName(filePath), filePath);
			vector<MeshPart*> meshParts = *mesh->GetMeshParts();  // src

			MeshComp* meshComp = new MeshComp;
			for (int i = 0; i < meshParts.size(); i++)
			{
				MeshPartComp* meshPartComp = new MeshPartComp;

				// Material ID
				meshPartComp->SetMaterial(L"NONE");

				// Vertex Buffer, Index Buffer Mapping
				meshPartComp->SetVertexBuffer(meshParts[i]->GetVertexBuffer());
				meshPartComp->SetIndexBuffer(meshParts[i]->GetIndexBuffer());

				// Vertex Array, Index Array Mapping
				vector<VertexType*>* vetexArrayDes = meshPartComp->GetVertexArray();
				vector<VertexType*> vertexArraySrc = meshParts[i]->GetVertexArray();
				for (int j = 0; j < vertexArraySrc.size(); j++)
					vetexArrayDes->push_back(vertexArraySrc[j]);

				vector<UINT>* indexArrayDes = meshPartComp->GetIndexArray();
				vector<UINT> indexArraySrc = meshParts[i]->GetIndexArray();
				for (int j = 0; j < indexArraySrc.size(); j++)
					indexArrayDes->push_back(indexArraySrc[j]);

				// Veretx Count, Index Count Mapping
				meshPartComp->SetVertexCount(meshParts[i]->GetVertexCount());
				meshPartComp->SetIndexCount(meshParts[i]->GetIndexCount());

				m_meshParts.push_back(meshPartComp);

				node->CreateOBB();
			}

		}

		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		// Mesh Comp안의 요소들
		{

			ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_SpanAvailWidth;

			if (ImGui::TreeNodeEx(("Material##" + m_meshID).c_str(), treeFlag))
			{
				for (int i = 0; i < m_meshParts.size(); i++)
				{
					string partNum = "Part";
					partNum += to_string(i);

					ImGui::Text(partNum.c_str());
					ImGui::SameLine();

					char buffer2[255];
					strcpy(buffer2, Core::ConvWcharTochar(m_meshParts[i]->GetMaterial()));

					ImGui::SetNextItemWidth(220.0f);
					ImRect rect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(textWidth, ImGui::GetItemRectSize().y));
					if (ImGui::InputText(("##" + partNum).c_str(), buffer2, sizeof(buffer2)))
					{

					}
					payload = ImGuIRenderClass::DraAndDropToItem(rect, ImGui::GetMousePos(), "CONTENT_BROWSER_ITEM");
					filePath = ProcessDragAndDropPayloadMaterial(payload);
					
					if (filePath != L"")
					{

						vector<ModelComp*>* comps = node->GetModelComps();
						for (int j = comps->size()-1; j >=0; j--)
						{
							if (comps->at(j)->GetCompType() == COMPONENT_TYPE::MATERIAL)
							{
								MaterialComp* materialComp = (MaterialComp*)comps->at(j);
								if(materialComp->GetMaterialName() == m_meshParts[i]->GetMaterial())
									comps->erase(comps->begin() + j);
							}
						}

						// 새로운 materialComp 삽입 
						Material* material = ResMgrClass::GetInst()->LoadMaterial(Core::GetFileName(filePath), filePath);

						MaterialComp* materialComp = new MaterialComp;
						materialComp->SetMateriaName(material->GetMaterialName());
						materialComp->SetMaterial(material);

						m_meshParts[i]->SetMaterial(Core::GetFileName(filePath));
						comps->push_back(materialComp);
					}
				}
				ImGui::TreePop();
			}

		}
	}
}

wstring MeshComp::ProcessDragAndDropPayloadMaterial(ImGuiPayload* payload)
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

wstring MeshComp::ProcessDragAndDropPayloadMesh(ImGuiPayload* payload)
{
	wstring result = L"";

	if (!payload)
		return result;

	wstring fileRelativePath = (wchar_t*)payload->Data;

	wstring fileExtension = ContentBrowserPanel::GetFileExtension(fileRelativePath);

	if (fileExtension == L"mesh")
		result = fileRelativePath;

	return result;
}