#include "pch.h"
#include "TextureClass.h"
#include "Core.h"

TextureClass::TextureClass()
{
	m_device = Core::GetDevice();
}

TextureClass::~TextureClass()
{
}


void TextureClass::Initialize(const wstring& _strFilePath)
{
	// ScratchImge -> ResourceView
	
	DirectX::ScratchImage image = LoadTexture(_strFilePath);

	// ResourceView
	DirectX::CreateShaderResourceView(m_device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_resourceView);
}

void TextureClass::Shutdown()
{
	if (m_resourceView)
	{
		m_resourceView->Release();
		delete m_resourceView;
		m_resourceView = 0;
	}
}

DirectX::ScratchImage TextureClass::LoadTexture(const wstring& _strFilePath)
{
	HRESULT result;

	//텍스처 로드
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