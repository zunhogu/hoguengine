#pragma once

#include "define.h"

class ModelNode;

class ModelComp
{
private:
protected:
	COMPONENT_TYPE m_componentType;

	vector<ModelComp*> m_referComponents;  // �ش� ������Ʈ�� ������ �ٸ� ������Ʈ
	
	bool m_isDelete;
public:
	ModelComp();
	virtual ~ModelComp();

public:
	virtual void Shutdown() {}

	COMPONENT_TYPE GetCompType() { return m_componentType; }
	void SetCompType(COMPONENT_TYPE type) { m_componentType = type; }

	virtual void Render(ModelNode* node) {}
	virtual void Clear() {}

	vector<ModelComp*>* GetReferComponent() { return &m_referComponents; }
	void SetReferComponent(ModelComp* comp);
	virtual void UpdateReferComponent(ModelComp* comp) {};  // �ڱ��ڽ��� �������� ���� ������Ʈ���� �˸���.

	bool* GetIsDelete() { return &m_isDelete; }
	void SetIsDelete(bool tf) { m_isDelete = tf; }
};

