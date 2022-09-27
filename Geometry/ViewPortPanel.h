#pragma once
#include "Panel.h"


class ViewPortPanel : public Panel
{
private:
public:
	ViewPortPanel();
	~ViewPortPanel();

	virtual void Render();
	void ProcessDragAndDropPayload(ImGuiPayload* payload);

	void CreateGizmo();
};

