#include "pch.h"
#include "Utility.h"

Utility::Utility()
{
	srand((unsigned int)time(NULL));
}

Utility::~Utility()
{
}

char* Utility::ConvWcharTochar(wstring _wstr)
{
	char* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, _wstr.c_str(), -1, NULL, 0, NULL, NULL);
	pStr = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, _wstr.c_str(), -1, pStr, strSize, 0, 0);

	return  pStr;
}

wchar_t* Utility::ConvCharToWchar(char* _cstr)
{
	wchar_t* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, _cstr, -1, NULL, NULL);
	pStr = new wchar_t[strSize];
	MultiByteToWideChar(CP_ACP, 0, _cstr, strlen(_cstr) + 1, pStr, strSize);

	return pStr;
}

wstring Utility::GetRandomKey()
{
	wstring key = L"";

	for (int i = 0; i < 7; i++)
	{
		switch (rand() % 3)
		{
		case 0:
			key += rand() % 10 + 48;
			break;
		case 1:
			key += rand() % 26 + 65;
			break;
		case 2:
			key += rand() % 26 + 97;
			break;
		}
	}
	return key;
}

wstring Utility::GetFileName(wstring filePath)
{
	wstring result = L"";

	for (int i = filePath.length(); i >= 0; i--)
	{
		if (filePath[i] == L'\\')
		{
			result = filePath.substr(i + 1, filePath.length());
			break;
		}
	}

	return result;
}

wstring Utility::GetFileExtension(wstring filePath)
{
	wstring result = L"";

	for (int i = filePath.length(); i >= 0; i--)
	{
		if (filePath[i] == L'.')
		{
			result = filePath.substr(i + 1, filePath.length());
			break;
		}
	}

	return result;
}

void Utility::ConvertData(char* buff, float& dest)
{
	string changeStr = "";
	int i;
	float data;
	if (buff[0] == '-') {
		for (i = 1; i < sizeof(buff); i++) {
			if (isdigit(buff[i]) || buff[i] == '.') {
				changeStr += buff[i];

				data = stof(changeStr);
				if (data == 0) { dest = 0; }
				else { dest = -data; }
			}
			else { break; }
		}
	}
	else if (buff[0] == '+') {
		for (i = 1; i < sizeof(buff); i++) {
			if (isdigit(buff[i]) || buff[i] == '.') {
				changeStr += buff[i];

				data = stof(changeStr);
				dest = data;
			}
			else { break; }
		}
	}
	else {
		for (i = 0; i < sizeof(buff); i++) {
			if (isdigit(buff[i]) || buff[i] == '.') {
				changeStr += buff[i];
				if (buff[i] != '.') {
					data = stof(changeStr);
					dest = data;
				}
			}
			else { break; }
		}
	}
}
