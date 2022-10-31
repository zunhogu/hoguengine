#pragma once
#include "Panel.h"

class RenderTextureClass;

class ViewPortPanel : public Panel
{
private:
	RenderTextureClass* m_renderTexture;
public:
	ViewPortPanel();
	~ViewPortPanel();

	virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	virtual void Shutdown();
	virtual void Render();

	RenderTextureClass* GetRenderTexture() { return m_renderTexture; }

	void ProcessDragAndDropPayload(ImGuiPayload* payload);

	void CreateGizmo();
};

