#pragma once

class ResourceClass
{
private:
	wstring m_strKey;  // ���ҽ��� Ű��
	wstring m_strRelativePath;  // ���ҽ� ����� 
public:
	static int m_referCount;
	static int GetReferCount() { return m_referCount; }
	static void SetReferCount(int cnt) { m_referCount = cnt; }

	ResourceClass();
	virtual ~ResourceClass();
public:

	void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
	const wstring& GetKey() { return m_strKey; }
	void SetRelativePath(const wstring& _strPath) { m_strRelativePath = _strPath; }
	const wstring& GetRelativePath() { return m_strRelativePath; }
};