#include "pch.h"
#include "TextureBuffer.h"
#include "Core.h"
#include "TextureClass.h"

TextureBuffer::TextureBuffer()
{
}

TextureBuffer::TextureBuffer(TextureClass* texture)
{
	ID3D11Texture2D* texture2D = texture->GetTexture();
	
	D3D11_TEXTURE2D_DESC inputTextureDesc;
	texture2D->GetDesc(&inputTextureDesc);

	// ouput buffer
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = inputTextureDesc.Width;
	textureDesc.Height = inputTextureDesc.Height;
	textureDesc.MipLevels = inputTextureDesc.MipLevels;
	textureDesc.ArraySize = inputTextureDesc.ArraySize;
	textureDesc.Format = inputTextureDesc.Format;
	textureDesc.SampleDesc.Count = inputTextureDesc.SampleDesc.Count;
	textureDesc.SampleDesc.Quality = inputTextureDesc.SampleDesc.Quality;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = inputTextureDesc.CPUAccessFlags;
	textureDesc.MiscFlags = inputTextureDesc.MiscFlags;

	Core::GetDevice()->CreateTexture2D(&textureDesc, 0, &m_output);

	Core::GetDeviceContext()->CopyResource(m_output, texture2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	Core::GetDevice()->CreateShaderResourceView(m_output, &srvDesc, &m_output_srv);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	Core::GetDevice()->CreateUnorderedAccessView(m_output, &uavDesc, &m_output_uav);
}

TextureBuffer::~TextureBuffer()
{
	m_output->Release();
	m_output_srv->Release();
	m_output_uav->Release();
}