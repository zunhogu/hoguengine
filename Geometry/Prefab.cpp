#include "pch.h"
#include "Prefab.h"
#include "Core.h"
#include "ResMgrClass.h"
#include "ModelNode.h"
#include "ModelInfoComp.h"
#include "TransformComp.h"
#include "Mesh.h"
#include "MeshComp.h"
#include "Material.h"

Prefab::Prefab()
{
}

Prefab::Prefab(const Prefab& prefab)
{
}

Prefab::~Prefab()
{
}

bool Prefab::Initialize(const wstring& _strFilePath)
{
    LoadPrefab(_strFilePath);
    return false;
}

bool Prefab::LoadPrefab(const wstring& _strFilePath)
{
    ifstream fin;

    fin.open(_strFilePath.c_str());
    if (fin.fail())
        return false;

    while (fin.peek() != EOF)
    {
        string buffer;
        fin >> buffer;
        ModelNode* node = nullptr;
        if (buffer == "Node")
        {
            fin >> buffer;
            fin >> buffer;
            node = new ModelNode();

            ModelInfoComp* modelInfoComp = new ModelInfoComp();
            modelInfoComp->SetModelName(Utility::GetInst()->Utility::GetInst()->ConvCharToWchar((char*)buffer.c_str()));
            node->AddModelComp(modelInfoComp);
            
            //TransformComp* transformComp = new TransformComp(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
            //node->AddModelComp(transformComp);
            
            while (1)
            {
                if (buffer == "NODE_END")
                    break;
                fin >> buffer;
                if (buffer == "Parent_Node")
                {
                    int parentNodeIndex = -1;
                    fin >> buffer;  // :
                    fin >> buffer;  // parent node name
					for (int i = 0; i < m_modelNodes.size(); i++)
					{
						if (m_modelNodes[i]->GetModelInfoComp()->GetModelName() == Utility::GetInst()->Utility::GetInst()->ConvCharToWchar((char*)buffer.c_str()))
                            parentNodeIndex = i;
					}
          
                    if (parentNodeIndex != -1)
                    {
                        node->SetParentNodeIndex(parentNodeIndex);
                        m_modelNodes[parentNodeIndex]->AddChildeNode(node);
                    }
                }
                else if (buffer == "Depth")
                {
                    fin >> buffer;
                    fin >> buffer;
                    node->SetTreeDepth(stoi(buffer));
                }
                else if (buffer == "Transform")
                {
                    fin >> buffer;  // :

                    XMFLOAT3 position, rotation, scale;
                    fin >> position.x;
                    fin >> position.y;
                    fin >> position.z;
                    fin >> rotation.x;
                    fin >> rotation.y;
                    fin >> rotation.z;
                    fin >> scale.x;
                    fin >> scale.y;
                    fin >> scale.z;

                    TransformComp* transformComp = new TransformComp;
                    //transformComp->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
                    //transformComp->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
                    //transformComp->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));

                    position.x = position.x / 100;
                    position.y = position.y / 100;
                    position.z = position.z / 100;

					transformComp->SetPosition(position);
					transformComp->SetRotation(rotation);
					transformComp->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));


                    node->AddModelComp(transformComp);
                }
                else if (buffer == "Type")
                {
                    fin >> buffer;
                    fin >> buffer;
                    if (buffer == "ROOT")
                    {
                    }
                    else if (buffer == "BONE")
                    {
                    }
                    else if (buffer == "MESH")
                    {
                        fin >> buffer;
                        fin >> buffer;
                        fin >> buffer;  // relativePath;

                        wstring relativePath = Utility::GetInst()->Utility::GetInst()->ConvCharToWchar((char*)buffer.c_str());
                        relativePath = L"contents\\assets\\" + relativePath;
                        Mesh* mesh = ResMgrClass::GetInst()->LoadMesh(Core::GetDevice(), Utility::GetInst()->GetFileName(relativePath), relativePath);

                        MeshComp* meshComp = new MeshComp;

                        meshComp->SetMeshID(Utility::GetInst()->ConvWcharTochar(Utility::GetInst()->GetFileName(relativePath)));

                        // Mesh component를 채워넣기 위해서 Mesh Resource를 살펴본다.
                        vector<MeshPart*> meshParts = *mesh->GetMeshParts();  // src
                        vector<MeshPartComp*>* meshPartComps = meshComp->GetMeshParts();  // des
                        for (int i = 0; i < meshParts.size(); i++)
                        {
                            MeshPartComp* meshPartComp = new MeshPartComp;
                          
                            // Material ID
                            meshPartComp->SetMaterial(meshParts[i]->GetMaterialID());

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

                            meshPartComps->push_back(meshPartComp);
                        }
                        node->AddModelComp(meshComp);

                        vector<MeshPart*> mehsParts = *mesh->GetMeshParts();  // src
                        for (int i = 0; i < meshParts.size(); i++)
                        {
                            // Material Resource를 통해 Material Component도 채워넣는다.
                            Material* material = ResMgrClass::GetInst()->FindMaterial(meshParts[i]->GetMaterialID());

                            MaterialComp* materialComp = new MaterialComp;
                            materialComp->SetMateriaName(material->GetMaterialName());
                            materialComp->SetMaterial(material);

                            node->AddModelComp(materialComp);
                        }

                    }
                }
            }
            m_modelNodes.push_back(node);
        }
    }
    if(!(m_modelNodes.size() < 0))
        m_modelNode = m_modelNodes[0];

    // Node에 대한 추가작업
    
    // node가 Root면 해당 prefab 파일의 이름을 노드 이름으로 설정한다.
    wstring name = Utility::GetInst()->GetFileName(_strFilePath);
    wstring only_file_name = name.substr(0, name.find(L'.'));
    m_modelNode->GetModelInfoComp()->SetModelName(only_file_name);

    // 각 노드별 Setting
    for (int i = 0; i < m_modelNodes.size(); i++)
    {
        ModelNode* node = m_modelNodes[i];
        int parentIdx = node->GetParentNodeIndex();
        node->AddPathToRootNode(node);
        while (1)
        {
            if (parentIdx == -1)
                break;
            else
            {
                node->AddPathToRootNode(m_modelNodes[parentIdx]);
                parentIdx = m_modelNodes[parentIdx]->GetParentNodeIndex();
            }
        }

        node->CreateOBB();
    }

    return false;
}
