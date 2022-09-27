#pragma once
#include "Scene.h"

class Scene_Start : public Scene
{
private:

	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjectionMatrix;
public:
	Scene_Start();
	Scene_Start(const Scene_Start&);
	~Scene_Start();
public:
	virtual bool Initialize(int screenWidth, int screenHeight, HWND hWnd);
	virtual void Shutdown();

	virtual void Frame();
	virtual void Render();

	virtual void Enter();
	virtual void Exit();

};

