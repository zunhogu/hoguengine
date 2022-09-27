#pragma once

class SceneInfo
{
private:
	char m_sid[128];

	UINT m_iSceneNum;
	wstring m_sSceneName;

	bool m_isClicked;
public:
	SceneInfo();
	SceneInfo(const SceneInfo&);
	~SceneInfo();

	void Initialize();
	void Shutdown();

	void SetSceneID(char* _id) { strcpy_s(m_sid, _id); }
	char* GetSceneID() { return this->m_sid; }

	UINT GetSceneNum() { return this->m_iSceneNum; }
	void SetSceneNum(UINT _num) { this->m_iSceneNum = _num; }

	wstring GetSceneName() { return this->m_sSceneName; }
	void SetSceneName(wstring _str) { this->m_sSceneName = _str; }

	void SetClick(bool _tf) { this->m_isClicked = _tf; }
	bool IsClicked() { return this->m_isClicked; }

};

