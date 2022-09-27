#pragma once

class Scene;
class Object;
class ModelNode;

class SceneMgrClass
{
private:

	vector <Scene*> m_arrScene;
	//Scene* m_arrScene[(UINT)SCENE_TYPE::END];  // Scene Type 배열 
	Scene* m_pCurScene;  // 현재 Scene

	int m_iScreenWidth, m_iScreenHeigth;
	HWND m_hWnd;

	SINGLE(SceneMgrClass);

	SceneMgrClass();
	SceneMgrClass(const SceneMgrClass&);
	~SceneMgrClass();
public:

	bool Initialize(int, int, HWND);
	void Shutdown();

	const wstring GetCurSceneName();
	Scene* GetCurScene() { return m_pCurScene; }

	void Frame();
	void Render();

	vector <Scene*>* GetSceneList() { return &m_arrScene; }
	bool SetCurScene(UINT _sceneNum, int _screenWidth, int _screenHeight, HWND _hwnd);

	Scene* GetSceneOfIndex(UINT _sIndex) { return m_arrScene[_sIndex]; }
	UINT GetCurSceneIndex(Scene* currScene);

	Scene* GetToolScene();

	void CreateScene(UINT _sceneNum, const wstring& _sceneName);
};
