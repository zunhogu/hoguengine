#pragma once
#include "Scene.h"

class Scene_Final : public Scene
{
private:

	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjectionMatrix;
public:
	Scene_Final();
	Scene_Final(const Scene_Final&);
	~Scene_Final();
public:
	virtual bool Initialize(int, int, HWND);
	virtual void Shutdown();

	virtual void Frame();
	virtual void Render();

	virtual void Enter();
	virtual void Exit();


};

