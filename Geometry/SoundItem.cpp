#include "pch.h"
#include "SoundItem.h"

SoundItem::SoundItem()
{
	m_resourceType = RESOURCE_TYPE::SOUND;
}

SoundItem::SoundItem(char* id, char* path)
{
	m_id = id;
	m_path = path;
	m_resourceType = RESOURCE_TYPE::SOUND;
	m_isClicked = false;
}

SoundItem::~SoundItem()
{
}
