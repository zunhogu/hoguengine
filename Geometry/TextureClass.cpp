#include "pch.h"
#include "TextureClass.h"
#include "Core.h"

TextureClass::TextureClass()
{
	m_device = Core::GetDevice();
	m_textureBuffer = 0;
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

	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture->GetDesc(&textureDesc);
	textureDesc.Usage = D3D11_USAGE_STAGING;
	
	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	m_device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);

	// UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	Core::GetDevice()->CreateUnorderedAccessView(m_texture, &uavDesc, &m_uav);

	m_textureBuffer = new TextureBuffer(this);

	//// 텍스처 로드 2  -> SRV에 대한 세부사항 입력에 제한이 있다.
	//DirectX::CreateShaderResourceView(m_device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_textureView);
}

void TextureClass::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		delete m_textureView;
		m_textureView = 0;
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

void TextureClass::Copy(ID3D11Resource* resource)
{
	Core::GetDeviceContext()->CopyResource(m_texture, resource);
}
