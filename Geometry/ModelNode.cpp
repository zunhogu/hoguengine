#include "pch.h"
#include "ModelNode.h"
#include "ModelComp.h"
#include "ModelInfoComp.h"
#include "TransformComp.h"
#include "MeshComp.h"
#include "TextureClass.h"
#include "Core.h"
#include "ResMgrClass.h"
#include "AnimationComp.h"
#include "TerrainComp.h"
#include "TerrainMesh.h"

ModelNode* g_currNode = nullptr;

ModelNode::ModelNode()
{
	m_parentindex = -1;
	m_treeDepth = 0;
    m_isOpenedInHierachy = false;
    m_isClickedHierachy = false;
	m_isOnlyMeshPicking = false;
}

ModelNode::ModelNode(const ModelNode& node, vector<ModelNode*> path)
{
	// Copy node's Info
	m_isOpenedInHierachy = false;
	m_isClickedHierachy = false;
	m_treeDepth = node.m_treeDepth;
	m_parentindex = node.m_parentindex;
	m_OBB = node.m_OBB;

	// Copy node's Components
	for (int i = 0; i < node.m_modelComps.size(); i++)
	{
		ModelComp* compSrc = node.m_modelComps[i];
		COMPONENT_TYPE type = compSrc->GetCompType();

		switch (type)
		{
			case COMPONENT_TYPE::MODELINFO:
			{
				ModelInfoComp* compDes = new ModelInfoComp(*(ModelInfoComp*)compSrc);
				m_modelComps.push_back(compDes);
			}
			break;
			case COMPONENT_TYPE::TRANSFORM:
			{
				TransformComp* compDes = new TransformComp(*(TransformComp*)compSrc);
				m_modelComps.push_back(compDes);
			}
			break;
			case COMPONENT_TYPE::MESH:
			{
				MeshComp* compDes = new MeshComp(*(MeshComp*)compSrc);
				m_modelComps.push_back(compDes);
			}
			break;
			case COMPONENT_TYPE::MATERIAL:
			{
				MaterialComp* compDes = new MaterialComp(*(MaterialComp*)compSrc);
				m_modelComps.push_back(compDes);
			}
			break;
			case COMPONENT_TYPE::BONE:
				break;
			case COMPONENT_TYPE::ANIMATION:
			{
				AnimationComp* compDes = new AnimationComp(*(AnimationComp*)compSrc);
				m_modelComps.push_back(compDes);
			}
				break;
			case COMPONENT_TYPE::END:
				break;
			default:
				break;
		}
	}
	
	path.push_back(this);
	// Copy node's Path To Root
	for (int i = 0; i < path.size(); i++)
		m_pathToRootNode.push_back(path[i]);

	// Copy node's Child Nodes
	for (int i = 0; i < node.m_childNodes.size(); i++)
		m_childNodes.push_back(new ModelNode(*node.m_childNodes[i], path));
}

ModelNode::~ModelNode()
{
}

void ModelNode::Shutdown()
{
	for (int i = 0; i < m_modelComps.size(); i++)
		m_modelComps[i]->Shutdown();

	for (int i = 0; i < m_childNodes.size(); i++)
		m_childNodes[i]->Shutdown();
}

void ModelNode::SetTranslationChild(XMFLOAT3 translation)
{
	static XMFLOAT3 prevTransltion = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3 deltaTranslation = XMFLOAT3(translation.x - prevTransltion.x, translation.y - prevTransltion.y, translation.z - prevTransltion.z);

	XMMATRIX translationMatrix = XMMatrixTranslation(deltaTranslation.x, deltaTranslation.y, deltaTranslation.z);

	for (int i = 0; i < m_childNodes.size(); i++)
	{
		m_childNodes[i]->CalculateTranslation(translationMatrix);
	}

	prevTransltion = translation;
}

void ModelNode::CalculateTranslation(XMMATRIX translationMat)
{
	TransformComp* transformComp = GetModelTransformComp();
	XMVECTOR postion = XMLoadFloat3(&transformComp->GetPosition());

	postion = XMVector3TransformCoord(postion, translationMat);

	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, postion);

	transformComp->SetPosition(newPos);
}

void ModelNode::SetRotationChild(XMFLOAT3 rotation)
{
	static XMFLOAT3 prevRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//cout << rotation.x << ", " << rotation.y << ", " << rotation.z << endl;

	XMFLOAT3 deltaRotation = XMFLOAT3(rotation.x - prevRotation.x, rotation.y - prevRotation.y, rotation.z - prevRotation.z);

	XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(deltaRotation.x));
	XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(deltaRotation.y));
	XMMATRIX rotZ = XMMatrixRotationZ(XMConvertToRadians(deltaRotation.z));

	XMMATRIX rotationMatrix = rotX * rotY * rotZ;

	for (int i = 0; i < m_childNodes.size(); i++)
	{
		m_childNodes[i]->CalculateRotation(rotationMatrix);
	}

	prevRotation = rotation;
}

void ModelNode::CalculateRotation(XMMATRIX rotationMatrix)
{
	TransformComp* transformComp = GetModelTransformComp();
	XMVECTOR rotate = XMLoadFloat3(&transformComp->GetRotation());

	rotate = XMVector3TransformCoord(rotate, rotationMatrix);

	XMFLOAT3 newRot;
	XMStoreFloat3(&newRot, rotate);


	transformComp->SetRotation(newRot);
}

void ModelNode::SetScaleChild(XMFLOAT3 scale)
{
	static XMFLOAT3 prevScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	
	XMFLOAT3 deltaScale = XMFLOAT3(scale.x / prevScale.x, scale.y / prevScale.y, scale.z / prevScale.z);
	cout << deltaScale.x << ", " << deltaScale.y << ", " << deltaScale.z << endl;

	XMMATRIX scaleMatrix = XMMatrixScaling(deltaScale.x, deltaScale.y, deltaScale.z);

	for (int i = 0; i < m_childNodes.size(); i++)
	{
		m_childNodes[i]->CalculateScale(scaleMatrix);
	}

	prevScale = scale;
}

void ModelNode::CalculateScale(XMMATRIX scaleMatrix)
{
	TransformComp* transformComp = GetModelTransformComp();
	XMVECTOR scale = XMLoadFloat3(&transformComp->GetScale());

	//XMFLOAT4X4 mat;
	//XMStoreFloat4x4(&mat, scaleMatrix);

	//cout << mat._11 << ", " << mat._22 <<", "<< mat._33 << endl;

	scale = XMVector3TransformCoord(scale, scaleMatrix);

	XMFLOAT3 newSca;
	XMStoreFloat3(&newSca, scale);

	transformComp->SetScale(newSca);
}


ModelInfoComp* ModelNode::GetModelInfoComp()
{
    for (int i = 0; i < m_modelComps.size(); i++)
    {
        if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::MODELINFO)
            return (ModelInfoComp*)m_modelComps[i];
    }
    return nullptr;
}

TransformComp* ModelNode::GetModelTransformComp()
{
    for (int i = 0; i < m_modelComps.size(); i++)
    {
        if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::TRANSFORM)
            return (TransformComp*)m_modelComps[i];
    }
    return nullptr;
}

MeshComp* ModelNode::GetMeshComp()
{
	MeshComp* meshComp = nullptr;
	for (int i = 0; i < m_modelComps.size(); i++)
	{
		if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::MESH)
		{
			meshComp = (MeshComp*)m_modelComps[i];
			return meshComp;
		}
	}

	return nullptr;
}

ModelComp* ModelNode::GetModelComp(COMPONENT_TYPE type)
{
    for (int i = 0; i < m_modelComps.size(); i++)
    {
        if (m_modelComps[i]->GetCompType() == type)
            return m_modelComps[i];
    }
    return nullptr;
}

MaterialComp* ModelNode::GetMaterialComp(wstring id)
{
	MaterialComp* materialComp = nullptr;
	for (int i = 0; i < m_modelComps.size(); i++)
	{
		if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::MATERIAL)
		{
			materialComp = (MaterialComp*)m_modelComps[i];
			if (materialComp->GetMaterialName() == id)
				return materialComp;
		}
	}

	return nullptr;
}

AnimationComp* ModelNode::GetAnimationComp()
{
	for (int j = 0; j < m_modelComps.size(); j++)
	{
		if (m_modelComps[j]->GetCompType() == COMPONENT_TYPE::ANIMATION)
			return (AnimationComp*)m_modelComps[j];
	}
	return nullptr;
}

TerrainComp* ModelNode::GetTerrainComp()
{
	for (int j = 0; j < m_modelComps.size(); j++)
	{
		if (m_modelComps[j]->GetCompType() == COMPONENT_TYPE::TERRAIN)
			return (TerrainComp*)m_modelComps[j];
	}
	return nullptr;
}

ModelNode* ModelNode::FindChildeNode(wstring nodeName)
{
	static ModelNode* node = nullptr;

	for (int i = 0; i < m_modelComps.size(); i++)
	{
		if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::MODELINFO)
		{
			ModelInfoComp* modelInfoComp = (ModelInfoComp*)m_modelComps[i];
			if (nodeName == modelInfoComp->GetModelName())
			{
				node = this;
				break;
			}
		}
	}

	if (node == nullptr)
	{
		for (int i = 0; i < m_childNodes.size(); i++)
		{
			m_childNodes[i]->FindChildeNode(nodeName);
		}
	}

	return node;
}

void ModelNode::Render(XMMATRIX viewMatirx, XMFLOAT3 cameraPos, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightPos)
{

	ModelInfoComp* modelInfoComp = GetModelInfoComp();
	if (modelInfoComp->IsActive())
	{
		// transform component //////
		TransformComp* transform = GetModelTransformComp();

		XMMATRIX worldMatrix = GetWorldMatrix();

		for (int i = 0; i < m_modelComps.size(); i++)
		{
			COMPONENT_TYPE type = m_modelComps[i]->GetCompType();

			switch (type)
			{
			case COMPONENT_TYPE::MODELINFO:
				break;
			case COMPONENT_TYPE::TRANSFORM:
				break;
			case COMPONENT_TYPE::MESH:
			{
				bool isRender;
				float radius = 1.0f;

				// 씬에 있는 모델의 위치 가져오기 
				XMFLOAT3 postion = GetModelTransformComp()->GetPosition();

				// 프러스텀 진행할지 검사
				isRender = CollisionClass::GetInst()->CheckCube(postion.x, postion.y, postion.z, radius);

				if (isRender)
					RenderMesh((MeshComp*)m_modelComps[i], worldMatrix, viewMatirx, cameraPos, lightDiffuseColor, lightPos);
			}
				break;
			case COMPONENT_TYPE::MATERIAL:
				break;
			case COMPONENT_TYPE::BONE:
				break;
			case COMPONENT_TYPE::ANIMATION:
			{
			}
				break;
			case COMPONENT_TYPE::TERRAIN:
			{
				TerrainComp* comp = (TerrainComp*)m_modelComps[i];

				comp->RederMesh(worldMatrix, viewMatirx, lightDiffuseColor, lightPos);
			}
				break;
			case COMPONENT_TYPE::END:
				break;
			default:
				break;
			}
		}
	}

	for (int j = 0; j < m_childNodes.size(); j++)
	{
		m_childNodes[j]->Render(viewMatirx, cameraPos, lightDiffuseColor, lightPos);
	}
}

void ModelNode::RenderMesh(MeshComp* meshComp, XMMATRIX worldMatrix, XMMATRIX viewMatirx, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos)
{
	bool stop = false;
	XMFLOAT4 ambientColor, emissiveColor, diffuseColor, specularColor;
	TextureClass* defaultTexture = ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), L"defaultTexture");
	TextureClass* ambientTexture = defaultTexture;
	TextureClass* emissiveTexture = defaultTexture;
	TextureClass* diffuseTexture = defaultTexture;
	TextureClass* specularTexture = defaultTexture;
	TextureClass* normalTexture = defaultTexture;
	float shiness;

	if (meshComp != nullptr)
	{

		ModelBuffer* modelBuffer = nullptr;
		XMMATRIX boneScale = XMMatrixIdentity();
		XMMATRIX* boneMatrixArray = &XMMatrixIdentity();
		UINT skinning = 0;

		// animation component
		AnimationComp* animationComp = GetRootNode()->GetAnimationComp();
		if (animationComp != nullptr)
		{
			modelBuffer = animationComp->GetModelBuffer();
			animationComp->UpdateAnimation(modelBuffer);
			boneScale = modelBuffer->GetBoneScale();
			boneMatrixArray = modelBuffer->GetBoneMatrixArray();
			skinning = animationComp->IsSkinning();
		}


		if (meshComp->GetIsRender())
		{
			vector<MeshPartComp*> meshParts = *meshComp->GetMeshParts();
			for (int i = 0; i < meshParts.size(); i++)
			{
				// 입력 조립기
				meshParts[i]->Render(Core::GetDeviceContext());

				// Material 정보 불러오기
				MaterialComp* materialComp = GetMaterialComp(meshParts[i]->GetMaterial());

				if (materialComp == nullptr)
					return;
				
				Material* material = materialComp->GetMaterial();

				ambientColor = material->GetAmbientColor();
				emissiveColor = material->GetEmissiveColor();
				diffuseColor = material->GetDiffuseColor();
				specularColor = material->GetSpecularColor();
				shiness = material->GetShinness();

				if(material->GetAmbientTextureID() != "NONE")
					ambientTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)material->GetAmbientTextureID().c_str()));
				if (material->GetEmissiveTextureID() != "NONE")
					emissiveTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)material->GetEmissiveTextureID().c_str()));
				if (material->GetDiffuseTextureID() != "NONE")
					diffuseTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)material->GetDiffuseTextureID().c_str()));
				if (material->GetSpecularTextureID() != "NONE")
					specularTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)material->GetSpecularTextureID().c_str()));
				if (material->GetNormalTextureID() != "NONE")
					normalTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)material->GetNormalTextureID().c_str()));

				GraphicsClass::GetInst()->RenderModel(meshParts[i]->GetIndexCount(), worldMatrix, viewMatirx,
													cameraPos, lightColor, lightPos,
													ambientColor, emissiveColor, diffuseColor, specularColor, shiness,
													ambientTexture->GetTexture(),
													emissiveTexture->GetTexture(),
													diffuseTexture->GetTexture(),
													specularTexture->GetTexture(),
													normalTexture->GetTexture(),
													boneScale, boneMatrixArray, skinning);
			}
		}
	}
}

XMMATRIX ModelNode::GetWorldMatrix()
{
	TransformComp* transformComp = GetModelTransformComp();

	XMMATRIX worldMatrix = XMMatrixIdentity();
	for (int i = m_pathToRootNode.size()-1; i >=0; i--)
		worldMatrix = XMMatrixMultiply(worldMatrix, m_pathToRootNode[i]->GetModelTransformComp()->GetWorldMatrix());


	return worldMatrix;
}

XMMATRIX ModelNode::GetWorldMatrixExceptThis()
{
	TransformComp* transformComp = GetModelTransformComp();

	XMMATRIX worldMatrix = XMMatrixIdentity();
	for (int i = m_pathToRootNode.size() - 2; i >= 0; i--)
	{
		worldMatrix = XMMatrixMultiply(worldMatrix, m_pathToRootNode[i]->GetModelTransformComp()->GetWorldMatrix());
	}
	return worldMatrix;
}

XMMATRIX ModelNode::GetTranslationMatrix()
{
	TransformComp* transformComp = GetModelTransformComp();

	XMMATRIX translationMatrix = XMMatrixIdentity();
	for (int i = 0; i < m_pathToRootNode.size(); i++)
		translationMatrix = XMMatrixMultiply(translationMatrix, m_pathToRootNode[i]->GetModelTransformComp()->GetTranslationMatrix());

	return translationMatrix;
}

XMMATRIX ModelNode::GetRotationMatrix()
{
	TransformComp* transformComp = GetModelTransformComp();

	XMMATRIX rotationMatrix = XMMatrixIdentity();
	for (int i = 0; i < m_pathToRootNode.size(); i++)
		rotationMatrix = XMMatrixMultiply(rotationMatrix, m_pathToRootNode[i]->GetModelTransformComp()->GetRotationMatrix());

	return rotationMatrix;
}

XMMATRIX ModelNode::GetScaleMatrix()
{
	TransformComp* transformComp = GetModelTransformComp();

	XMMATRIX scaleMatrix = XMMatrixIdentity();
	for (int i = 0; i < m_pathToRootNode.size(); i++)
		scaleMatrix = XMMatrixMultiply(scaleMatrix, m_pathToRootNode[i]->GetModelTransformComp()->GetScaleMatrix());

	return scaleMatrix;
}

XMVECTOR ModelNode::GetQuaternion()
{
	return XMQuaternionRotationMatrix(GetRotationMatrix());
}

void ModelNode::CreateOBB()
{
	// 모든 노드 순회하면서 Max값과 Min값을 가져와야함
	XMVECTOR max = XMVectorSet(-INFINITY, -INFINITY, -INFINITY, 1.0f);
	XMVECTOR min = XMVectorSet(INFINITY, INFINITY, INFINITY, 1.0f);

	CalculateMinMax(max, min);

	// OBB의 경우 국소공간에서 계산해줬기때문에 국소공간안의 점이 된다.
	// 지역공간과 세계공간의 중심을 따로 계산해 셰이더에는 지역공간 위치를 넣어준다.
	XMFLOAT3 localCenterValue;
	XMFLOAT3 extentsValue;

	XMStoreFloat3(&localCenterValue, 0.5f * (min + max));
	XMStoreFloat3(&extentsValue, 0.5f * (max - min));

	BoundingBox AABB;

	// AABB 박스 생성
	AABB.Center = localCenterValue;
	AABB.Extents = extentsValue;

	// OBB 박스 생성
	DirectX::BoundingOrientedBox::CreateFromBoundingBox(m_OBB, AABB);

}

void ModelNode::CalculateMinMax(XMVECTOR& max, XMVECTOR& min)
{
	// 현재 노드의 component를 순회하면 min max 값을 계산
	for (int i = 0; i < m_modelComps.size(); i++)
	{
		if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::MESH)
		{
			MeshComp* meshComp = (MeshComp*)m_modelComps[i];
			vector<MeshPartComp*> meshPartComp = *meshComp->GetMeshParts();
			for (int j = 0; j < meshPartComp.size(); j++)
			{
				vector<VertexType*> vertexes = *meshPartComp[j]->GetVertexArray();
				for (int k = 0; k < vertexes.size(); k++)
				{
					XMFLOAT3 P3(vertexes[k]->position.x, vertexes[k]->position.y, vertexes[k]->position.z);

					XMMATRIX worldMatrix = GetModelTransformComp()->GetWorldMatrix();
					//XMVECTOR P = XMVector3TransformCoord(XMVectorSet(P3.x, P3.y, P3.z, 1.0f), worldMatrix);

					XMVECTOR P = XMLoadFloat3(&P3);

					max = XMVectorMax(max, P);
					min = XMVectorMin(min, P);
				}
			}
		}
	}

	//// 자식 노드도 순회하며 Min max값 계산
	//for (int i = 0; i < m_childNodes.size(); i++)
	//{
	//	m_childNodes[i]->CalculateMinMax(max, min);
	//}
}

bool ModelNode::CheckPicking(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis)
{

	// ray, rayorigin : World Space
	// Box : Local Space

	bool result = false;

	BoundingOrientedBox* tempBox = new BoundingOrientedBox();
	tempBox->Center = m_OBB.Center;
	tempBox->Extents = m_OBB.Extents;
	tempBox->Orientation = m_OBB.Orientation;

	TransformComp* transformComp = GetModelTransformComp();

	XMMATRIX worldMatrix = GetWorldMatrix();
	XMMATRIX translationMatrix = GetTranslationMatrix();
	XMMATRIX scaleMaitrx = GetScaleMatrix();
	XMVECTOR quternion = GetQuaternion();

	// 1차 피킹은 다른 오브젝트와의 거리를 알아야하기때문에 같은 공간인 월드공간상에서 진행한다. 
	XMStoreFloat3(&tempBox->Center, XMVector3TransformCoord(XMLoadFloat3(&tempBox->Center), worldMatrix));
	XMStoreFloat3(&tempBox->Extents, XMVector3TransformNormal(XMLoadFloat3(&tempBox->Extents), scaleMaitrx * translationMatrix));
	XMStoreFloat4(&tempBox->Orientation, quternion);

	// first picking ( picking to obb)
	result = tempBox->Intersects(rayOrigin, rayDir, _dis);
	
	if (result || m_isOnlyMeshPicking)
	{
		// second picking
		ModelNode* node = CheckPickingMesh(rayOrigin, rayDir);

		if (node != nullptr)
			result = true;
		else
			result = false;
	}

	return result;

}

bool ModelNode::CheckPickingOBB(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis)
{
	return m_OBB.Intersects(rayOrigin, rayDir, _dis);
}

ModelNode* ModelNode::CheckPickingMesh(XMVECTOR rayOrigin, XMVECTOR rayDir) {

	// 2차 피킹은 로컬공간에서 진행한다.
	float tmin = INFINITY;
	g_currNode = nullptr;

	ModelNode* node = CheckPickingTriangle(rayOrigin, rayDir, tmin);

	return node;
}

ModelNode* ModelNode::CheckPickingTriangle(XMVECTOR rayOrigin, XMVECTOR rayDir, float& tmin)
{
	XMVECTOR  A, B, C;


	// 현재 노드의 component를 순회하면 min max 값을 계산
	for (int i = 0; i < m_modelComps.size(); i++)
	{
		if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::MESH)
		{
			TransformComp* transformComp = GetModelTransformComp();

			XMMATRIX worldMatrix = GetWorldMatrix();
			XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

			rayOrigin = XMVector3TransformCoord(rayOrigin, invWorldMatrix);  // World Space to Local Space
			rayDir = XMVector3TransformNormal(rayDir, invWorldMatrix);  // World Space to Local Space

			rayDir = XMVector3Normalize(rayDir);

			MeshComp* meshComp = (MeshComp*)m_modelComps[i];
			vector<MeshPartComp*> meshPartComp = *meshComp->GetMeshParts();
			for (int j = 0; j < meshPartComp.size(); j++)
			{
				int indexCount = meshPartComp[j]->GetIndexCount();
				vector<VertexType*> vertexArray = *meshPartComp[j]->GetVertexArray();
				vector<UINT> indexArray = *meshPartComp[j]->GetIndexArray();
				for (int k = 0; k < indexCount / 3; k++)
				{
					XMFLOAT4 plane;

					int i0 = indexArray[k * 3 + 0];
					int i1 = indexArray[k * 3 + 1];
					int i2 = indexArray[k * 3 + 2];

					// 삼각형의 정점들
					A = XMLoadFloat3(&vertexArray[i0]->position);
					B = XMLoadFloat3(&vertexArray[i1]->position);
					C = XMLoadFloat3(&vertexArray[i2]->position);

					float t = 0.0f;
					if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, A, B, C, t))
					{
						if (t < tmin)
						{
							tmin = t;
							g_currNode = this;
						}
					}
				}
			}
		}
		else if (m_modelComps[i]->GetCompType() == COMPONENT_TYPE::TERRAIN)
		{
			TerrainComp* terrainComp = (TerrainComp*)m_modelComps[i];
			TerrainMesh* terrainMesh = (TerrainMesh*)ResMgrClass::GetInst()->FindMesh(terrainComp->GetTerrainMeshID());
			
			XMMATRIX worldMatrix = GetWorldMatrix();
			XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

			rayOrigin = XMVector3TransformCoord(rayOrigin, invWorldMatrix);  // World Space to Local Space
			rayDir = XMVector3TransformNormal(rayDir, invWorldMatrix);  // World Space to Local Space

			rayDir = XMVector3Normalize(rayDir);


			TerrainVertexType* vertexArray = terrainMesh->GetVertexArray();
			int vertexCount = terrainMesh->GetVertexCount();

			for (int k = 0; k < vertexCount / 6; k++)
			{
				XMFLOAT4 plane;

				int i0 = k * 6 + 0;
				int i1 = k * 6 + 2;
				int i2 = k * 6 + 4;
				
				// 삼각형의 정점들
				A = XMLoadFloat3(&vertexArray[i0].position);
				B = XMLoadFloat3(&vertexArray[i1].position);
				C = XMLoadFloat3(&vertexArray[i2].position);

				float t = 0.0f;
				if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, A, B, C, t))
				{
					g_currNode = this;
					return this;
				}
			}
		}
	}

	//// 자식 노드도 순회하며 Min max값 계산
	//for (int i = 0; i < m_childNodes.size(); i++)
	//{
	//	m_childNodes[i]->CheckPickingTriangle(rayOrigin, rayDir, tmin);
	//}

	return g_currNode;
}

void ModelNode::Clear()
{
	for (int i = 0; i < m_modelComps.size(); i++)
		m_modelComps[i]->Clear();
}


void ModelNode::SetIsOpenedHierachyAllTrueToRootPath()
{
	for (int i = 0; i < m_pathToRootNode.size(); i++)
	{
		m_pathToRootNode[i]->SetIsOpenedHierachy(true);
	}
}

void ModelNode::SetIsClicekdHierachyAllFalse()
{
    m_isClickedHierachy = false;
    for (int i = 0; i < m_childNodes.size(); i++)
        m_childNodes[i]->SetIsClicekdHierachyAllFalse();
}

vector<ModelNode*> ModelNode::GetAllChildNodes()
{
	vector<ModelNode*> result;

	NodeCircuit(result);

	return result;
}

void ModelNode::NodeCircuit(vector<ModelNode*>& arr)
{
	// 자식 노드 순회
	for (int i = 0; i < m_childNodes.size(); i++)
	{
		arr.push_back(m_childNodes[i]);
		m_childNodes[i]->NodeCircuit(arr);
	}
}

ModelNode* ModelNode::GetRootNode()
{
	return m_pathToRootNode[0];
}