#include "pch.h"
#include "TextureClass.h"

TextureClass::TextureClass()
{
}

TextureClass::~TextureClass()
{
}


void TextureClass::Initialize(ID3D11Device* device, const wstring& _strFilePath)
{
	// �ؽ�ó ������
	DirectX::ScratchImage image = LoadTexture(_strFilePath);

	// �ε��� �ؽ�ó�� ������ �ڿ��並 �����. 
	DirectX::CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_Texture);
}

void TextureClass::Shutdown()
{
	if (m_Texture)
	{
		m_Texture->Release();
		delete m_Texture;
		m_Texture = 0;
	}
}

DirectX::ScratchImage TextureClass::LoadTexture(const wstring& _strFilePath)
{
	HRESULT result;

	//�ؽ�ó �ε�
	WCHAR ext[_MAX_EXT];
	DirectX::ScratchImage image;

	_wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0) {
		result = DirectX::LoadFromDDSFile(_strFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else if (_wcsicmp(ext, L".tga") == 0) {
		result = DirectX::LoadFromTGAFile(_strFilePath.c_str(), nullptr, image);
	}
	else {
		result = DirectX::LoadFromWICFile(_strFilePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}

	return image;
}