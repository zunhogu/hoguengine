#pragma once
#include "pch.h"
#include "define.h"

//extern UINT g_iCurFocusWindowID;
//extern UINT g_iCurMousePosWindowID;

class Panel
{
private:
protected:
	IMGUI_WINDOW_TYPE m_type;
	bool m_isRender;
public:
	Panel();
	virtual ~Panel();

	virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	void SetRender(bool tf) { m_isRender = tf; }

	IMGUI_WINDOW_TYPE GetPanelType() { return m_type; }
};

