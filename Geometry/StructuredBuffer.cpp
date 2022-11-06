#include "pch.h"
#include "StructuredBuffer.h"
#include "Core.h"

StructuredBuffer::StructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount)
	: m_inputData(inputData), m_inputStride(inputStride), m_inputCount(inputCount), m_outputStride(outputStride), m_outputCount(outputCount)
{
	if (outputStride == 0 || outputCount == 0)
	{
		this->m_outputStride = inputStride;
		this->m_outputCount = inputCount;
	}

	CreateInput();
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

StructuredBuffer::~StructuredBuffer()
{
	m_input->Release();
	m_srv->Release();
	m_output->Release();
	m_uav->Release();
	m_result->Release();
}

void StructuredBuffer::Copy(void* data, UINT size)
{
	Core::GetDeviceContext()->CopyResource(m_result, m_output);

	D3D11_MAPPED_SUBRESOURCE subResource;

	Core::GetDeviceContext()->Map(m_result, 0, D3D11_MAP_READ, 0, &subResource);
	memcpy(data, subResource.pData, size);
	Core::GetDeviceContext()->Unmap(m_result, 0);
}

void StructuredBuffer::CreateInput()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = m_inputStride * m_inputCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = m_inputStride;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_inputData;

	Core::GetDevice()->CreateBuffer(&desc, &initData, &buffer);

	m_input = (ID3D11Resource*)buffer;
}

void StructuredBuffer::CreateSRV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)m_input;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = m_inputCount;

	Core::GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &m_srv);
}

void StructuredBuffer::CreateOutput()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = m_outputStride * m_outputCount;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = m_outputStride;

	Core::GetDevice()->CreateBuffer(&desc, nullptr, &buffer);

	m_output = (ID3D11Resource*)buffer;
}

void StructuredBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)m_output;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_outputCount;

	Core::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &m_uav);
}

void StructuredBuffer::CreateResult()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	((ID3D11Buffer*)m_output)->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	Core::GetDevice()->CreateBuffer(&desc, nullptr, &buffer);

	m_result = (ID3D11Resource*)buffer;
}
