#include "pch.h"
#include "ComputePaintingShader.h"
#include "Core.h"

ComputePaintingShader::ComputePaintingShader(wstring file)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* computeShader;
	D3D11_BUFFER_DESC rayBufferDesc;

	errorMessage = 0;
	computeShader = 0;

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	result = D3DCompileFromFile(file.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ComputePaintingShader", "cs_5_0", flags, 0, &computeShader, &errorMessage);
	if (FAILED(result))
		MessageBox(nullptr, "Compute  Vertex Shader", "Compie Error", MB_OK);

	result = Core::GetDevice()->CreateComputeShader(computeShader->GetBufferPointer(), computeShader->GetBufferSize(), NULL, &m_shaderBuffer);
	if (FAILED(result))
		MessageBox(nullptr, "Compute Pixel Shader", "Compie Error", MB_OK);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	rayBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	rayBufferDesc.ByteWidth = sizeof(BrushBufferType);
	rayBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	rayBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	rayBufferDesc.MiscFlags = 0;
	rayBufferDesc.StructureByteStride = 0;

	Core::GetDevice()->CreateBuffer(&rayBufferDesc, NULL, &m_rayBuffer);
}

ComputePaintingShader::~ComputePaintingShader()
{
}

void ComputePaintingShader::SetShader()
{
	Core::GetDeviceContext()->CSSetShader(m_shaderBuffer, NULL, 0);
}

void ComputePaintingShader::SetConstantBuffer(FLOAT type, XMFLOAT2 uv, FLOAT range, XMFLOAT3 chanel)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	BrushBufferType* dataPtr;
	unsigned int bufferNumber;

	Core::GetDeviceContext()->Map(m_rayBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	dataPtr = (BrushBufferType*)mappedResource.pData;

	dataPtr->type = type;
	dataPtr->uv = uv;
	dataPtr->range = range;
	dataPtr->chanel = chanel;
	dataPtr->padding = 0.0f;

	Core::GetDeviceContext()->Unmap(m_rayBuffer, 0);

	bufferNumber = 0;

	Core::GetDeviceContext()->CSSetConstantBuffers(bufferNumber, 1, &m_rayBuffer);
}
