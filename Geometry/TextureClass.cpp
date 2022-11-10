#include "pch.h"
#include "TextureClass.h"
#include "Core.h"

TextureClass::TextureClass()
{
	m_device = Core::GetDevice();
	m_texture = 0;
	m_textureBuffer = 0;
	m_shaderResourceView = 0;
}

TextureClass::~TextureClass()
{
}


void TextureClass::Initialize(const wstring& _strFilePath)
{
	// ScratchImge -> ResourceView
	
	DirectX::ScratchImage image = LoadTexture(_strFilePath);

	// 텍스처 로드 1
	DirectX::CreateTexture(m_device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), (ID3D11Resource**)&m_texture);

	if (!m_texture) return;

	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture->GetDesc(&textureDesc);
	textureDesc.Usage = D3D11_USAGE_STAGING;
	
	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	m_device->CreateShaderResourceView(m_texture, &srvDesc, &m_shaderResourceView);

	m_textureBuffer = new TextureBuffer(this);

	//// 텍스처 로드 2  -> SRV에 대한 세부사항 입력에 제한이 있다.
	//DirectX::CreateShaderResourceView(m_device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_textureView);
}

void TextureClass::Shutdown()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		delete m_shaderResourceView;
		m_shaderResourceView = 0;
	}
}

DirectX::ScratchImage TextureClass::LoadTexture(const wstring& _strFilePath)
{
	HRESULT result;

	//텍스처 로드
	WCHAR ext[_MAX_EXT];
	DirectX::ScratchImage image;

	m_filePath = _strFilePath;

	_wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0) {
		m_fileExtension = L".dds";
		result = DirectX::LoadFromDDSFile(_strFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else if (_wcsicmp(ext, L".tga") == 0) {
		m_fileExtension = L".tga";
		result = DirectX::LoadFromTGAFile(_strFilePath.c_str(), nullptr, image);
	}
	else {
		m_fileExtension = Utility::GetInst()->GetFileExtension(_strFilePath);
		result = DirectX::LoadFromWICFile(_strFilePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}

	return image;
}

bool TextureClass::SaveTexture()
{
	HRESULT hr;
	bool result = true;
	DirectX::ScratchImage image;

	ID3D11Resource* resource;
	m_shaderResourceView->GetResource(&resource);
	hr = DirectX::CaptureTexture(m_device, Core::GetDeviceContext(), resource, image);
	
	if(SUCCEEDED(hr))
	{
		if (m_fileExtension ==  L".dds")
			DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, m_filePath.c_str());
		else if (m_fileExtension == L".tga")
			DirectX::SaveToTGAFile(*image.GetImages(), m_filePath.c_str());
		else 
			DirectX::SaveToWICFile(image.GetImages(), image.GetImageCount(), WIC_FLAGS_NONE, GUID_ContainerFormatPng, m_filePath.c_str());
	}
	return result;
}

void TextureClass::Copy(ID3D11Resource* resource)
{
	Core::GetDeviceContext()->CopyResource(m_texture, resource);
}
