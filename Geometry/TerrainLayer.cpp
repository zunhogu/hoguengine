#include "pch.h"
#include "TerrainLayer.h"
#include "Core.h"

TerrainLayer::TerrainLayer()
	: m_maskID(L""), m_material1(nullptr), m_material2(nullptr), m_material3(nullptr)
{
}

TerrainLayer::~TerrainLayer()
{
}

TerrainLayer::TerrainLayer(const TerrainLayer&)
{
}

void TerrainLayer::Shutdown()
{
}

bool TerrainLayer::SetTextureBuffer(TextureClass* texture)
{
	bool result = true;
	m_textureBuffer = texture->GetTextureBuffer();

	m_alphaMapBoard = new RenderTextureClass;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetTexture()->GetDesc(&desc);
	float screenWidth = desc.Width;
	float screenHeight = desc.Height;

	if (desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
	{
		MessageBox(nullptr, "Texture Mask는 DXGI_FORMAT_B8G8R8A8_UNORM Format 이미지만 지원합니다.", "TerrainLayer_Error", 0);
		return false;
	}

	result = m_alphaMapBoard->Initialize(Core::GetDevice(), Core::GetDeviceContext(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, &desc);
	if (!result)
	{
		MessageBox(nullptr, "AlphaMap Can't Initilize", "TerrainLayer_Error", 0);
		delete m_alphaMapBoard;
		m_alphaMapBoard = 0;
		return false;
	}

	m_bitmap = new BitMapClass;
	result = m_bitmap->Initialize(Core::GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(nullptr, "BitMap Can't Initilize", "TerrainLayer_Error", 0);
		delete m_alphaMapBoard;
		m_alphaMapBoard = 0;
		delete m_bitmap;
		m_bitmap = 0;
		return false;
	}

	return true;
}
