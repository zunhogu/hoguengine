#pragma once

#include <DirectXMath.h>
using namespace DirectX;

// Singleton 매크로
#define SINGLE(type) public:\
							static type* GetInst()\
							{\
								 static type mgr;\
								 return &mgr;\
							 }\


enum class SCENE_TYPE
{
	TOOL,
	SCNENE1,
	SCNENE2,

	END,
};

enum class COMPONENT_TYPE
{
	MODELINFO,
	TRANSFORM,
	MESH,
	MATERIAL,
	BONE,
	ANIMATION,
	TERRAIN,

	END,
};


enum class IMGUI_WINDOW_TYPE
{
	VIEWPORT,
	HIERACHY,
	RESOURCE,
	INSPECTOR,
	SCENE,
	CONTENTBROWSER,

	END = 5,
};

enum class RESOURCE_TYPE
{
	MODEL,
	TEXTURE,
	SOUND,
	MESH,

	END,
};

enum class TEXTURE_TYPE
{
	DEFAULT,
	HEIGHTMAP,

	END,
};

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
	XMFLOAT4 boneidx;
	XMFLOAT4 boneWeight;
};

struct TerrainVertexType
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

#define KEY_CHECK(key, state) KeyMgrClass::GetInst()->GetKeyboardState(key) == state
#define KEY_HOLD(key) KEY_CHECK(key, KEY_STATE::HOLD)
#define KEY_TAP(key) KEY_CHECK(key, KEY_STATE::TAP)
#define KEY_AWAY(key) KEY_CHECK(key, KEY_STATE::AWAY)

#define MOUSE_CHECK(key, state) KeyMgrClass::GetInst()->GetMouseState(key) == state
#define MOUSE_HOLD(key) MOUSE_CHECK(key, KEY_STATE::HOLD)
#define MOUSE_TAP(key) MOUSE_CHECK(key, KEY_STATE::TAP)
#define MOUSE_AWAY(key) MOUSE_CHECK(key, KEY_STATE::AWAY)


#define FWRITE_WCHAR(wstr, size, pFile) size = wcslen(wstr.c_str()) * sizeof(wchar_t);\
										fwrite(&size, sizeof(size_t), 1, pFile);\
										fwrite(wstr.c_str(), sizeof(wchar_t), size, pFile);

#define FREAD_WCHAR(wstr, size, pFile) fread(&size, sizeof(size_t), 1, pFile);\
									   wstr = new wchar_t[size];\
								       fread(wstr, sizeof(wchar_t), size, pFile);

// 리소스 확장자
static const wchar_t* g_modelExtension[] = { L"pref" };
static const wchar_t* g_textureExtension[] = { L"dds", L"tga", L"png", L"jpg" };
#define IMGUI_WINDOW_FOCUS_CHECK ImGui::GetMousePos().x >= ImGui::GetCursorScreenPos().x && ImGui::GetMousePos().x <= ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x &&\
								 ImGui::GetMousePos().y >= ImGui::GetCursorScreenPos().y && ImGui::GetMousePos().y <= ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y


