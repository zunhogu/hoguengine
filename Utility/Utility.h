#pragma once
#include <string>

class MYDLL_DECLSPEC Utility
{
private:
	SINGLE(Utility);

public:
	Utility();
	~Utility();

	char* ConvWcharTochar(wstring _wstr);
	wchar_t* ConvCharToWchar(char* _str);
	wstring GetRandomKey();

	wstring GetFileName(wstring filePath);
	wstring GetFileExtension(wstring filePath);

	void ConvertData(char* buff, float& dest);
};

