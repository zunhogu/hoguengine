#include "pch.h"
#include "TextureItem.h"

TextureItem::TextureItem()
{
	m_resourceType = RESOURCE_TYPE::TEXTURE;
}

TextureItem::TextureItem(char* id, char* path)
{
	m_id = id;
	m_path = path;
	m_resourceType = RESOURCE_TYPE::TEXTURE;
	m_isClicked = false;
}

TextureItem::~TextureItem()
{
	if (m_textureView)
	{
		delete m_textureView;
		m_textureView = 0;
	}
}

