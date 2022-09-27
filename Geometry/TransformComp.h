#pragma once
#include "ModelComp.h"

class TransformComp : public ModelComp
{
private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;
public:
	TransformComp();
	TransformComp(const TransformComp& transformComp);
	TransformComp(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale);
	~TransformComp();

	void SetPosition(XMFLOAT3 _pos) { this->m_position = _pos; }
	XMFLOAT3 GetPosition() { return this->m_position; }
	void SetRotation(XMFLOAT3 _rot) { this->m_rotation = _rot; }
	XMFLOAT3 GetRotation() { return this->m_rotation; }
	void SetScale(XMFLOAT3 _sca) { this->m_scale = _sca; }
	XMFLOAT3 GetScale() { return this->m_scale; }

	XMMATRIX GetWorldMatrix();
	XMMATRIX GetTranslationMatrix();
	XMMATRIX GetRotationMatrix();
	XMVECTOR GetQuaternion();
	XMMATRIX GetScaleMatrix();

	virtual void Render(ModelNode* node);
};

