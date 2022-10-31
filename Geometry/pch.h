// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

#define _CRT_SECURE_NO_WARNINGS

#ifdef CREATEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include "define.h"

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <vector>
using std::vector;
#include <map>
using std::map;
using std::pair;
#include <string>
using std::string;
#include <wtypes.h>
using std::wstring;

#include <cstdarg>
#include <fstream>
using std::getline;
#include <commdlg.h>
#include <windowsx.h>

#include <filesystem>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

#include "library/imgui.h"
#include "library/imgui_internal.h"
#include "library/imgui_impl_dx11.h"
#include "library/imgui_impl_win32.h"
#include "library/ImGuizmo.h"
using namespace ImGui;

#include <filesystem>

#include "framework.h"
#include "define.h"
#include <DirectXMath.h>
#include <dxgi.h> 
#include <d3dcommon.h> 
#include <d3d11.h> 

//#include <d3dx11async.h> 
#include  <d3dcompiler.h>// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#include <DirectXMath.h>
#include <DirectXTex.h>
#include <DirectXCollision.h>
using namespace DirectX;
#endif //PCH_H