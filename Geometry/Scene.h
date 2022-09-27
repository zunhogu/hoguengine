#pragma once
#include "Core.h"

// 전반 선언 : CScene은 CObject를 구체적으로 알지 못하고 이런애가 있다더라~~만 알 수 있음
// CObject.h를 include 하면 COjbect 변경되면 CScene도 다시 컴파일 되서 컴파일 속도 감소하기 때문에 전방선언 사용 
// 다시 말하면 헤더가 헤더를 서로 참조하는 걸 방지하고 하는 이유 

class ModelNode;

class Scene
{
protected:
	wstring m_strName; // Scene 이름

	vector<ModelNode*>* m_allNode;  // 씬 안에 있는 모든 노드

	HWND m_hWnd;
	int m_iScreenWidth, m_iScreenHeight;

public:
	Scene();
	Scene(const Scene&);
	// 모든 자식 Scene을 부모 CScene의 포인터로 관리한다.
	// 실제 이 포인터를 delete 하면 업캐스팅된 부모의 소멸자만 실행되어서 자식의 소멸자는 실행안됨, 따라서 부모 소멸자를 오버라이딩 해줘야함
	// 이렇게 하면 자식 클래스 소멸자 실행후 부모 클래스 소멸자 실행된다. 
	virtual ~Scene();

public:
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return this->m_strName; }

	virtual bool Initialize(int, int, HWND) = 0;
	virtual void Shutdown() = 0;

	virtual void Frame() = 0;
	virtual void Render() = 0;

	// 아래 씬에 대한 함수는 부모 클래스에서는 정의 하지않고 인터페이스의 역할만 하는 순수가상함수 사용 
	virtual void Enter() = 0;  // 해당 Scene 진입 시 호출   
	virtual void Exit() = 0;  // 해당 Scene 탈출 시 호출


	virtual void SaveObjects(FILE* _pFile);
	virtual void LoadObjects(FILE* _pFile);

	virtual void AddNodeToScene(ModelNode* node);
	virtual void DeleteNode(ModelNode* node);

	virtual vector<ModelNode*>* GetAllNode() { return m_allNode; }
	virtual void SetAllNode(vector<ModelNode*>* allnode) { m_allNode = allnode; }
};

