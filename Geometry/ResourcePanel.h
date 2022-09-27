#pragma once
#include "Panel.h"

class ResourceItem;

class ResourcePanel : public Panel
{
private:
	vector<ResourceItem*>* m_vResources;
public:
	ResourcePanel();
	ResourcePanel(vector<ResourceItem*>* resources);
	~ResourcePanel();

public:
	virtual void Render();

	void ShowResource();
	void ProcessDragAndDropPayload(ImGuiPayload* payload);
};

