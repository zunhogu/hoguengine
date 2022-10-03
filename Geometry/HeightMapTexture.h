#pragma once
#include "TextureClass.h"
class HeightMapTexture : public TextureClass
{
private:
	ID3D11Texture2D* m_texture2D;
public:
	HeightMapTexture();
	HeightMapTexture(const HeightMapTexture&);
	~HeightMapTexture();

public:
	virtual void Initialize(const wstring& _strFilePath);
};

