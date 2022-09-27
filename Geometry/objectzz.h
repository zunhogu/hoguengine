//#pragma once
//
//#include "Mesh.h"
//#include "TextureClass.h"
//
//#include "ModelComp.h"
//#include "TransformComp.h"
//#include "MeshComp.h"
//#include "MaterialComp.h"
//#include "AnimationComp.h"
//#include "BoneComp.h"
//
//
//class ModelNode;
//
//class Object {
//protected:
//	GROUP_TYPE m_ieType;
//	wstring m_modelName;
//
//	ModelNode* m_modelNode;
//public:
//	Object();
//	Object(const Object& src);
//	virtual ~Object();
//
//	virtual void Intiailize(GROUP_TYPE _eType, wstring modelKey);
//	virtual void Shutdown() {}
//
//	GROUP_TYPE GetObjectGroupType() { return this->m_ieType; }
//	void SetObjectGroupType(UINT _type) { this->m_ieType = (GROUP_TYPE)_type; }
//
//	wstring GetModelName() { return this->m_modelName; }
//	void SetModelName(wstring modelName) { this->m_modelName = modelName; }
//
//	ModelNode* GetModelNode() { return m_modelNode; }
//	void SetModelNode(ModelNode* nodes) { m_modelNode = nodes; }
//
//	XMFLOAT3 GetPosition();
//	XMFLOAT3 GetRotation();
//	XMFLOAT3 GetScale();
//
//	XMMATRIX GetWorldMatrix();
//	XMMATRIX GetTranslationMatrix();
//	XMMATRIX GetRotationMatrix();
//	XMVECTOR GetQuaternion();
//	XMMATRIX GetScaleMatrix();
//
//	virtual void Save(FILE* _pFile) {}
//	virtual Object* Load(FILE* _pFile) { return nullptr; }
//
//	virtual void Frame();
//	virtual void Render();
//	virtual void Render(XMMATRIX viewMatirx, XMFLOAT3 cameraPos, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightPos);
//
//	virtual bool CheckPicking(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis);
//	virtual bool CheckPickingOBB(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis);
//	virtual bool CheckPickingMesh(XMVECTOR rayOrigin, XMVECTOR rayDir);
//
//};
//
