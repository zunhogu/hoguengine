#pragma once
#include "ResourceItem.h"

class MYDLL_DECLSPEC TextureItem : public ResourceItem
{
private:
	ID3D11ShaderResourceView* m_textureView;
public:
	TextureItem();
	TextureItem(char* id, char* path);
	~TextureItem();

	ID3D11ShaderResourceView* GetTextureView() { return m_textureView; }
	void SetTextureView(ID3D11ShaderResourceView* view) { m_textureView = view; }
};

