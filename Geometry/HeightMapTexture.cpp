#include "pch.h"
#include "HeightMapTexture.h"

HeightMapTexture::HeightMapTexture()
{
	m_texture2D = nullptr;
}

HeightMapTexture::HeightMapTexture(const HeightMapTexture&)
{
}

HeightMapTexture::~HeightMapTexture()
{
}

void HeightMapTexture::Initialize(const wstring& _strFilePath)
{
	bool result; 

	// ScratchImge -> TextureResource -> ResourceView
	DirectX::ScratchImage image = LoadTexture(_strFilePath);

	// Texture Resource
	result = DirectX::CreateTexture(m_device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), (ID3D11Resource**)&m_texture2D);

	D3D11_TEXTURE2D_DESC desc;
	m_texture2D->GetDesc(&desc);  // �ش� �Լ��� ID3D11Texture2D�� DESC�� ������ ������ �� �ִ�.
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
 


	// ResourceView
	m_device->CreateShaderResourceView(m_texture2D, nullptr, &m_resourceView);
}
