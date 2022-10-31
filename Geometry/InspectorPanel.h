#pragma once
#include "Panel.h"

class TransformComp;
class MeshComp;
class MaterialComp;
class AnimationComp;
class BoneComp;
class Bone;
class ResourceItem;

class InspectorPanel : public Panel
{
private:
	vector<ResourceItem*>* m_vResources;
public:
	InspectorPanel();
	InspectorPanel(vector<ResourceItem*>* resourceItems);
	~InspectorPanel();

	virtual void Shutdown();
	virtual void Render();
	void ShowPopUp();
};

