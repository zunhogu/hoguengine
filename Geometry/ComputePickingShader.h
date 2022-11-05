#pragma once

class ComputePickingShader
{
private:
	struct RayBufferType
	{
		XMFLOAT3 position;
		FLOAT outputSize;
		XMFLOAT3 direction;
		FLOAT padding;
	};
private:
	ID3D11ComputeShader* m_shaderBuffer;
	ID3D11Buffer* m_rayBuffer;
public:
	ComputePickingShader(wstring file);
	~ComputePickingShader();

public:
	void SetShader();
	void SetConstantBuffer(XMFLOAT3 position, FLOAT outputSize, XMFLOAT3 direction);
};

