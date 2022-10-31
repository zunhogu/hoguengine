#pragma once

class BitMapClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
public:
	BitMapClass();
	BitMapClass(const BitMapClass&);
	~BitMapClass();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY);

	int GetIndexCount();
	XMFLOAT2 GetBitMapSize() { return XMFLOAT2(m_bitmapWidth, m_bitmapHeight); }
private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

