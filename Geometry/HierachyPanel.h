#pragma once
#include "Panel.h"

class ModelNode;

class HierachyPanel : public Panel
{
private:
	ID3D11ShaderResourceView* m_close_node_img;
	ID3D11ShaderResourceView* m_open_node_img;
public:
	HierachyPanel();
	~HierachyPanel();

	virtual void Render();
	void NodeUpdate(ModelNode* node);
	void ShowNodeTree(ModelNode* modelNodes, int num);
	void ShowPopUp();

	void ProcessDragAndDropPayload(ImGuiPayload* payload);
};