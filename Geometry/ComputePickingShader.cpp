#include "pch.h"
#include "ComputePickingShader.h"
#include "Core.h"

ComputePickingShader::ComputePickingShader(wstring file)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* computeShader;
	D3D11_BUFFER_DESC rayBufferDesc;

	errorMessage = 0;
	computeShader = 0;

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	result = D3DCompileFromFile(file.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ComputePickingCheckShader", "cs_5_0", flags, 0, &computeShader, &errorMessage);
	if (FAILED(result))
		MessageBox(nullptr, "Compute shader", "Compie Error", MB_OK);

	result = Core::GetDevice()->CreateComputeShader(computeShader->GetBufferPointer(), computeShader->GetBufferSize(), NULL, &m_shaderBuffer);
	if (FAILED(result))
		cout << "cant create compute shader" << endl;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	rayBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	rayBufferDesc.ByteWidth = sizeof(RayBufferType);
	rayBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	rayBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	rayBufferDesc.MiscFlags = 0;
	rayBufferDesc.StructureByteStride = 0;

	Core::GetDevice()->CreateBuffer(&rayBufferDesc, NULL, &m_rayBuffer);
}

ComputePickingShader::~ComputePickingShader()
{
}

void ComputePickingShader::SetShader()
{
	Core::GetDeviceContext()->CSSetShader(m_shaderBuffer, NULL, 0);
}

void ComputePickingShader::SetConstantBuffer(XMFLOAT3 position, FLOAT outputSize, XMFLOAT3 direction)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	RayBufferType* dataPtr;
	unsigned int bufferNumber;

	Core::GetDeviceContext()->Map(m_rayBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	dataPtr = (RayBufferType*)mappedResource.pData;

	dataPtr->position = position;
	dataPtr->outputSize = outputSize;
	dataPtr->direction = direction;

	Core::GetDeviceContext()->Unmap(m_rayBuffer, 0);

	bufferNumber = 0;

	Core::GetDeviceContext()->CSSetConstantBuffers(bufferNumber, 1, &m_rayBuffer);
}
