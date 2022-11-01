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

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
};

