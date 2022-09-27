#include "pch.h"
#include "ModelItem.h"

ModelItem::ModelItem()
{
	m_resourceType = RESOURCE_TYPE::MODEL;
}

ModelItem::ModelItem(char* id, char* path)
{
	m_id = id;
	m_path = path;
	m_resourceType = RESOURCE_TYPE::MODEL;
	m_isClicked = false;
}

ModelItem::~ModelItem()
{
}
