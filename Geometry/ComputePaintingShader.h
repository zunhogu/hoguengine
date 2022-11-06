#pragma once

class ComputePaintingShader
{
private:
	struct BrushBufferType
	{
		FLOAT type;
		XMFLOAT2 uv;
		FLOAT range;
		XMFLOAT3 chanel;
		FLOAT padding;
	};
private:
	ID3D11ComputeShader* m_shaderBuffer;
	ID3D11Buffer* m_rayBuffer;
public:
	ComputePaintingShader(wstring file);
	~ComputePaintingShader();

public:
	void SetShader();
	void SetConstantBuffer(FLOAT type, XMFLOAT2 uv, FLOAT range, XMFLOAT3 chanel);
};

