#ifndef PCH_H
#define PCH_H

#include "framework.h"

#define _CRT_SECURE_NO_WARNINGS

#ifdef CREATEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

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
#include <stack>
using std::stack;

#include <algorithm>
#include <cstdarg>
#include <fstream>
using std::getline;
#include <commdlg.h>
#include <windowsx.h>

#include <filesystem>
#include <stdlib.h>
#include <time.h>

#endif //PCH_H
