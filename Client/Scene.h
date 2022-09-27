#pragma once

// ���� ���� : CScene�� CObject�� ��ü������ ���� ���ϰ� �̷��ְ� �ִٴ���~~�� �� �� ����
// CObject.h�� include �ϸ� COjbect ����Ǹ� CScene�� �ٽ� ������ �Ǽ� ������ �ӵ� �����ϱ� ������ ���漱�� ��� 
// �ٽ� ���ϸ� ����� ����� ���� �����ϴ� �� �����ϰ� �ϴ� ���� 
class Object;  // ���漱���� Ŭ���� Ÿ�� ���Ұ�, ������ Ÿ������ ���

// �ϳ��� Scene�ȿ� �������� Object���� �ִµ�, �ȿ� �ִ� 32���� Object�� �׷��� ������ ����
class Scene
{
private:
	vector<Object*> m_arrObj[(UINT)GROUP_TYPE::END];  // ������Ʈ�� ���� �� ������ ���͸� �׷� ������ŭ ���� 
	wstring m_strName; // Scene �̸�
public:
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return this->m_strName; }

	void Frame();
	void Render();

	// �Ʒ� ���� ���� �Լ��� �θ� Ŭ���������� ���� �����ʰ� �������̽��� ���Ҹ� �ϴ� ���������Լ� ��� 
	virtual void Enter() = 0;  // �ش� Scene ���� �� ȣ��   
	virtual void Exit() = 0;  // �ش� Scene Ż�� �� ȣ��


protected:
	void AddObject(Object* _pObj, GROUP_TYPE _eType)
	{
		m_arrObj[(UINT)_eType].push_back(_pObj);
	}

public:
	Scene();
	Scene(const Scene&);
	// ��� �ڽ� Scene�� �θ� CScene�� �����ͷ� �����Ѵ�.
	// ���� �� �����͸� delete �ϸ� ��ĳ���õ� �θ��� �Ҹ��ڸ� ����Ǿ �ڽ��� �Ҹ��ڴ� ����ȵ�, ���� �θ� �Ҹ��ڸ� �������̵� �������
	// �̷��� �ϸ� �ڽ� Ŭ���� �Ҹ��� ������ �θ� Ŭ���� �Ҹ��� ����ȴ�. 
	virtual ~Scene();
};

