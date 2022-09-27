#pragma once

#ifdef CREATEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

class MYDLL_DECLSPEC PathMgr
{
	SINGLE(PathMgr);
private:
	wchar_t m_szContentPath[255];
public:
	PathMgr();
	PathMgr(const PathMgr&);
	~PathMgr();
public:
	void Initialize();
	const wchar_t* GetContentPath() { return this->m_szContentPath; }

};
