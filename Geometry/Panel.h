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

	virtual void Render() = 0;

	void SetRender(bool tf) { m_isRender = tf; }
};

