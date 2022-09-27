#pragma once
#include "define.h"

class MYDLL_DECLSPEC ResourceItem
{
private:
protected:
	char* m_id;
	char* m_path;

	RESOURCE_TYPE m_resourceType;
	bool m_isClicked;
public:
	ResourceItem();
	ResourceItem(char* id, char* path);
	~ResourceItem();

	char* GetID() { return m_id; }
	void SetID(char* id) { m_id = id; }
	char* GetPath() { return m_path; }
	void SetPath(char* path) { m_path = path; }
	bool GetIsClicked() { return m_isClicked; }
	void SetIsClicked(bool tf) { m_isClicked = tf; }


	RESOURCE_TYPE GetResourceType() { return m_resourceType; }
	void SetResourceType(RESOURCE_TYPE type) { m_resourceType = type; }
};

