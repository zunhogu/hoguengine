//#include "pch.h"
//
//#include "Object.h"
//#include "ResMgrClass.h"
//#include "Core.h"
//#include "SceneMgrClass.h"
//#include "Prefab.h"
//#include "ModelNode.h"
//#include "ModelInfoComp.h"
//#include "TransformComp.h"
//#include "MeshComp.h"
//
//Object::Object()
//{
//}
//
//Object::Object(const Object& src)
//{
//}
//
//Object::~Object()
//{
//}
//
//void Object::Intiailize(GROUP_TYPE _eType, wstring modelKey)
//{
//	Prefab* prefab = ResMgrClass::GetInst()->LoadPrefab(modelKey);
//	SetModelNode(prefab->GetModelNodes());
//}
//
//XMFLOAT3 Object::GetPosition()
//{
//	//return m_modelInfo->GetTransformComp()->GetWorldPosition();
//	return XMFLOAT3();
//}
//
//XMFLOAT3 Object::GetRotation()
//{
//	//return m_modelInfo->GetTransformComp()->GetWorldRotation();
//	return XMFLOAT3();
//}
//
//XMFLOAT3 Object::GetScale()
//{
//	//return m_modelInfo->GetTransformComp()->GetWorldScale();
//	return XMFLOAT3();
//}
//
//XMMATRIX Object::GetWorldMatrix()
//{
//	//return m_modelInfo->GetTransformComp()->GetWorldMatrix();
//	return XMMATRIX();
//}
//
//XMMATRIX Object::GetTranslationMatrix()
//{
//	//XMFLOAT3 pos = m_modelInfo->GetTransformComp()->GetWorldPosition();
//	//return XMMatrixTranslation(pos.x, pos.y, pos.z);
//	return XMMATRIX();
//}
//
//XMMATRIX Object::GetRotationMatrix()
//{
//	//XMFLOAT3 rot = m_modelInfo->GetTransformComp()->GetWorldRotation();
//	//XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(rot.x));
//	//XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(rot.y));
//	//XMMATRIX rotZ = XMMatrixRotationZ(XMConvertToRadians(rot.z));
//
//	//return  rotX * rotY * rotZ;
//	return XMMATRIX();
//}
//
//XMVECTOR Object::GetQuaternion()
//{
//	//return XMQuaternionRotationMatrix(GetRotationMatrix());
//	return XMVECTOR();
//}
//
//XMMATRIX Object::GetScaleMatrix()
//{
//	//XMFLOAT3 sca = m_modelInfo->GetTransformComp()->GetWorldScale();
//	//return XMMatrixScaling(sca.x, sca.y, sca.z);
//	return XMMATRIX();
//}
//
//
//void Object::Frame()
//{
//}
//
//void Object::Render()
//{
//}
//
//void Object::Render(XMMATRIX viewMatirx, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos)
//{
//	//if (m_modelInfo->GetIsActive())
//	//{
//	//	bool stop = false;
//	//	XMFLOAT4 ambientColor, emissiveColor, diffuseColor, specularColor;
//	//	TextureClass* ambientTexture = nullptr;
//	//	TextureClass* emissiveTexture = nullptr;
//	//	TextureClass* diffuseTexture = nullptr;
//	//	TextureClass* specularTexture = nullptr;
//	//	TextureClass* normalTexture = nullptr;
//	//	float shiness;
//
//	//	// transform component //////
//	//	TransformComp* transform = m_modelInfo->GetTransformComp();
//	//	XMMATRIX worldMatrix = transform->GetWorldMatrix();
//
//	//	// mesh component
//	//	MeshComp* meshComp = m_modelInfo->GetMeshComp();
//	//	if (meshComp != nullptr)
//	//	{
//	//		ModelBuffer* modelBuffer = meshComp->GetModelBuffer();
//
//	//		// animation component
//	//		//AnimationComp* animationComp = m_modelInfo->GetAnimationComp();
//	//		//UpdateBone(animationComp->GetAnimationControllder(), animationComp->GetSkeleton(), modelBuffer);
//
//	//		XMMATRIX boneScale = modelBuffer->GetBoneScale();
//	//		XMMATRIX* boenMAtrixArray = modelBuffer->GetBoneMatrixArray();
//	//		UINT skinning = modelBuffer->GetBoneSkinning();
//
//	//		if (meshComp->GetIsRender())
//	//		{
//	//			vector<MeshPart*> meshParts = *meshComp->GetMeshParts();
//	//			for (int i = 0; i < meshParts.size(); i++)
//	//			{
//	//				vector<MaterialPart*> materialParts = *meshParts[i]->GetMaterialParts();
//	//				for (int j = 0; j < materialParts.size(); j++)
//	//				{
//	//					// 입력 조립기
//	//					materialParts.at(j)->Render(GraphicsClass::GetInst()->GetDeviceContext());
//
//	//					// Material 정보 불러오기
//	//					ambientColor = materialParts[j]->GetMaterial()->GetAmbientColor();
//	//					emissiveColor = materialParts[j]->GetMaterial()->GetEmissiveColor();
//	//					diffuseColor = materialParts[j]->GetMaterial()->GetDiffuseColor();
//	//					specularColor = materialParts[j]->GetMaterial()->GetSpecularColor();
//	//					shiness = materialParts[j]->GetMaterial()->GetShinness();
//
//	//					ambientTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)materialParts[j]->GetMaterial()->GetAmbientTextureID().c_str()));
//	//					if (ambientTexture == nullptr) 
//	//						stop = true;
//	//					emissiveTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)materialParts[j]->GetMaterial()->GetEmissiveTextureID().c_str()));
//	//					if (emissiveTexture == nullptr) 
//	//						stop = true;
//	//					diffuseTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)materialParts[j]->GetMaterial()->GetDiffuseTextureID().c_str()));
//	//					if (diffuseTexture == nullptr) 
//	//						stop = true;
//	//					specularTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)materialParts[j]->GetMaterial()->GetSpecularTextureID().c_str()));
//	//					if (specularTexture == nullptr) 
//	//						stop = true;
//	//					normalTexture = ResMgrClass::GetInst()->FindTexture(Core::ConvCharToWchar((char*)materialParts[j]->GetMaterial()->GetNormalTextureID().c_str()));
//	//					if (normalTexture == nullptr) 
//	//						stop = true;
//
//	//					if (stop) 
//	//						return;
//	//					GraphicsClass::GetInst()->Render(materialParts.at(j)->GetIndexCount(), worldMatrix, viewMatirx,
//	//						cameraPos, lightColor, lightPos,
//	//						ambientColor, emissiveColor, diffuseColor, specularColor, shiness,
//	//						ambientTexture->GetTexture(),
//	//						emissiveTexture->GetTexture(),
//	//						diffuseTexture->GetTexture(),
//	//						specularTexture->GetTexture(),
//	//						normalTexture->GetTexture(),
//	//						boneScale, boenMAtrixArray, skinning);
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//}
//
//
//bool Object::CheckPicking(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis)
//{
//	bool result = false;
//	//BoundingOrientedBox box = m_modelInfo->GetMeshComp()->GetOBB();
//
//	//BoundingOrientedBox* tempBox = new BoundingOrientedBox();
//	//tempBox->Center = box.Center;
//	//tempBox->Extents = box.Extents;
//	//tempBox->Orientation = box.Orientation;
//
//	//// 1차 피킹은 다른 오브젝트와의 거리를 알아야하기때문에 같은 공간인 월드공간상에서 진행한다. 
//	//XMStoreFloat3(&tempBox->Center, XMVector3TransformCoord(XMLoadFloat3(&tempBox->Center), GetWorldMatrix()));
//	//XMStoreFloat3(&tempBox->Extents, XMVector3TransformNormal(XMLoadFloat3(&tempBox->Extents), GetScaleMatrix() * GetTranslationMatrix()));
//	//XMStoreFloat4(&tempBox->Orientation, GetQuaternion());
//
//
//	//// first picking ( picking to obb)
//	//result = tempBox->Intersects(rayOrigin, rayDir, _dis);
//	//if (!result)
//	//	return false;
//	//else
//	//{
//	//	// second picking
//	//	XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, GetWorldMatrix());
//
//	//	rayOrigin = XMVector3TransformCoord(rayOrigin, invWorldMatrix);  // World Space to Local Space
//	//	rayDir = XMVector3TransformNormal(rayDir, invWorldMatrix);  // World Space to Local Space
//
//	//	rayDir = XMVector3Normalize(rayDir);
//
//	//	result = CheckPickingMesh(rayOrigin, rayDir);
//	//}
//
//	return result;
//}
//
//bool Object::CheckPickingOBB(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis)
//{
//	//BoundingOrientedBox box = m_modelInfo->GetMeshComp()->GetOBB();  // box in local space
//	//return box.Intersects(rayOrigin, rayDir, _dis);
//	return false;
//}
//
//bool Object::CheckPickingMesh(XMVECTOR rayOrigin, XMVECTOR rayDir) {
//
//	//// 2차 피킹은 로컬공간에서 진행한다.
//	//float tmin = INFINITY;
//	//XMVECTOR  A, B, C;
//
//	//MeshComp* meshComp = m_modelInfo->GetMeshComp();
//	//vector<MeshPart*> meshes = *meshComp->GetMeshParts();
//	//for (int i = 0; i < meshes.size(); i++)
//	//{
//	//	vector<MaterialPart*> parts = *meshes[i]->GetMaterialParts();
//	//	for (int j = 0; j < parts.size(); j++)
//	//	{
//	//		int indexCount = parts[j]->GetIndexCount();
//	//		vector<VertexType*> vertexArray = *parts[j]->GetVertexArray();
//	//		vector<UINT> indexArray = *parts[j]->GetIndexArray();
//	//		for (int k = 0; k < indexCount / 3; k++)
//	//		{
//	//			XMFLOAT4 plane;
//
//	//			int i0 = indexArray[k * 3 + 0];
//	//			int i1 = indexArray[k * 3 + 1];
//	//			int i2 = indexArray[k * 3 + 2];
//
//	//			// 삼각형의 정점들
//	//			A = XMLoadFloat3(&vertexArray[i0]->position);
//	//			B = XMLoadFloat3(&vertexArray[i1]->position);
//	//			C = XMLoadFloat3(&vertexArray[i2]->position);
//
//	//			float t = 0.0f;
//	//			if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, A, B, C, t))
//	//			{
//	//				if (t < tmin)
//	//				{
//	//					tmin = t;
//	//				}
//	//				return true;  // 삼각형중 하나라도 intersect되면 true 반환
//	//			}
//	//		}
//	//	}
//	//}
//
//	//// tmin값에 변화가 생겼다면 삼각형과 반직선이 교차한것으로 본다.
//	//if (tmin != INFINITY)
//	//{
//	//	return true;
//	//}
//
//	//return false;
//	return false;
//}