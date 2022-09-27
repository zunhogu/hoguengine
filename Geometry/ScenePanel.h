#pragma once
#include "Panel.h"
class ScenePanel : public Panel
{
private:
public:
	ScenePanel();
	~ScenePanel();
public:
	virtual void Render();

	void ShowGizmo();
};

