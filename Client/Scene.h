#pragma once

// 전반 선언 : CScene은 CObject를 구체적으로 알지 못하고 이런애가 있다더라~~만 알 수 있음
// CObject.h를 include 하면 COjbect 변경되면 CScene도 다시 컴파일 되서 컴파일 속도 감소하기 때문에 전방선언 사용 
// 다시 말하면 헤더가 헤더를 서로 참조하는 걸 방지하고 하는 이유 
class Object;  // 전방선언은 클래스 타입 사용불가, 포인터 타입으로 사용

// 하나의 Scene안에 여러가지 Object들이 있는데, 안에 있는 32개의 Object를 그룹을 나눠서 관리
class Scene
{
private:
	vector<Object*> m_arrObj[(UINT)GROUP_TYPE::END];  // 오브젝트를 저장 및 관리할 벡터를 그룹 개수만큼 선언 
	wstring m_strName; // Scene 이름
public:
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return this->m_strName; }

	void Frame();
	void Render();

	// 아래 씬에 대한 함수는 부모 클래스에서는 정의 하지않고 인터페이스의 역할만 하는 순수가상함수 사용 
	virtual void Enter() = 0;  // 해당 Scene 진입 시 호출   
	virtual void Exit() = 0;  // 해당 Scene 탈출 시 호출


protected:
	void AddObject(Object* _pObj, GROUP_TYPE _eType)
	{
		m_arrObj[(UINT)_eType].push_back(_pObj);
	}

public:
	Scene();
	Scene(const Scene&);
	// 모든 자식 Scene을 부모 CScene의 포인터로 관리한다.
	// 실제 이 포인터를 delete 하면 업캐스팅된 부모의 소멸자만 실행되어서 자식의 소멸자는 실행안됨, 따라서 부모 소멸자를 오버라이딩 해줘야함
	// 이렇게 하면 자식 클래스 소멸자 실행후 부모 클래스 소멸자 실행된다. 
	virtual ~Scene();
};

