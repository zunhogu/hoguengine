#pragma once
#include "Core.h"

// ���� ���� : CScene�� CObject�� ��ü������ ���� ���ϰ� �̷��ְ� �ִٴ���~~�� �� �� ����
// CObject.h�� include �ϸ� COjbect ����Ǹ� CScene�� �ٽ� ������ �Ǽ� ������ �ӵ� �����ϱ� ������ ���漱�� ��� 
// �ٽ� ���ϸ� ����� ����� ���� �����ϴ� �� �����ϰ� �ϴ� ���� 

class ModelNode;

class Scene
{
protected:
	wstring m_strName; // Scene �̸�

	vector<ModelNode*>* m_allNode;  // �� �ȿ� �ִ� ��� ���

	HWND m_hWnd;
	int m_iScreenWidth, m_iScreenHeight;

public:
	Scene();
	Scene(const Scene&);
	// ��� �ڽ� Scene�� �θ� CScene�� �����ͷ� �����Ѵ�.
	// ���� �� �����͸� delete �ϸ� ��ĳ���õ� �θ��� �Ҹ��ڸ� ����Ǿ �ڽ��� �Ҹ��ڴ� ����ȵ�, ���� �θ� �Ҹ��ڸ� �������̵� �������
	// �̷��� �ϸ� �ڽ� Ŭ���� �Ҹ��� ������ �θ� Ŭ���� �Ҹ��� ����ȴ�. 
	virtual ~Scene();

public:
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return this->m_strName; }

	virtual bool Initialize(int, int, HWND) = 0;
	virtual void Shutdown() = 0;

	virtual void Frame() = 0;
	virtual void Render() = 0;

	// �Ʒ� ���� ���� �Լ��� �θ� Ŭ���������� ���� �����ʰ� �������̽��� ���Ҹ� �ϴ� ���������Լ� ��� 
	virtual void Enter() = 0;  // �ش� Scene ���� �� ȣ��   
	virtual void Exit() = 0;  // �ش� Scene Ż�� �� ȣ��


	virtual void SaveObjects(FILE* _pFile);
	virtual void LoadObjects(FILE* _pFile);

	virtual void AddNodeToScene(ModelNode* node);
	virtual void DeleteNode(ModelNode* node);

	virtual vector<ModelNode*>* GetAllNode() { return m_allNode; }
	virtual void SetAllNode(vector<ModelNode*>* allnode) { m_allNode = allnode; }
};

