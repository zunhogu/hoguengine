#include "pch.h"
#include "TerrainShader.h"

TerrainShader::TerrainShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_hullShader = 0;
	m_domainShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
}

TerrainShader::TerrainShader(const TerrainShader&)
{
}

TerrainShader::~TerrainShader()
{
}

bool TerrainShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"./contents/shader/terrain.vs", L"./contents/shader/terrain.hs", L"./contents/shader/terrain.ds", L"./contents/shader/terrain.ps");

	if (!result)
		return false;

	return true;
}

void TerrainShader::Shutdown()
{
	ShutdownShader();
}

bool TerrainShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* hullShaderBuffer;
	ID3D10Blob* domainShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC wireFrameBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TerrainVertexShader", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, "Vertex Shader", "Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(hsFilename, NULL, NULL, "TerrainHullShader", "hs_5_0", 0, 0, &hullShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		else
			MessageBox(hwnd, "Hull Shader", "Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(dsFilename, NULL, NULL, "TerrainDomainShader", "ds_5_0", 0, 0, &domainShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		else
			MessageBox(hwnd, "Domain Shader", "Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "TerrainPixelShader", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, "Pixel Shader", "Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result))
		return false;

	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "COLOR";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;


	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
		return false;

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
		return false;

	wireFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	wireFrameBufferDesc.ByteWidth = sizeof(WireFrameBufferType);
	wireFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	wireFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	wireFrameBufferDesc.MiscFlags = 0;
	wireFrameBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&wireFrameBufferDesc, NULL, &m_wireFrameBuffer);
	if (FAILED(result))
		return false;

	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void TerrainShader::ShutdownShader()
{
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

}

bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, bool isWireFrame, bool isLOD, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* matrixBuffer;
	LightBufferType* lightBuffer;
	CameraBufferType* cameraBuffer;
	WireFrameBufferType* wireFrameBuffer;
	TessellationBufferType* tessellationBuffer;

	///////////////////////////////////////////////////////////////////////////////
	// Vertex Shader //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	cameraBuffer = (CameraBufferType*)mappedResource.pData;

	cameraBuffer->cameraPos = cameraPos;
	cameraBuffer->padding = 0.0f;

	deviceContext->Unmap(m_cameraBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	tessellationBuffer = (TessellationBufferType*)mappedResource.pData;

	tessellationBuffer->tessellationAmounts = XMFLOAT4(4.0f, 2.0f, 2.0f, 1.0f);
	tessellationBuffer->distanceStandard = isLOD ? XMFLOAT4(10.f, 30.f, 50.f, 1.0f) : XMFLOAT4(10.f, 30.f, 50.f, 0.0f);

	deviceContext->Unmap(m_tessellationBuffer, 0);

	bufferNumber++;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	///////////////////////////////////////////////////////////////////////////////
	// Hull Shader ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	// Domain Shader //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	matrixBuffer = (MatrixBufferType*)mappedResource.pData;

	matrixBuffer->world = worldMatrix;
	matrixBuffer->view = viewMatrix;
	matrixBuffer->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	///////////////////////////////////////////////////////////////////////////////
	// Pixel Shader ///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	lightBuffer = (LightBufferType*)mappedResource.pData;

	lightBuffer->ambientColor = ambientColor;
	lightBuffer->diffuseColor = diffuseColor;
	lightBuffer->lightDir = lightDirection;
	lightBuffer->padding = 0.0f;

	deviceContext->Unmap(m_lightBuffer, 0);

	result = deviceContext->Map(m_wireFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	wireFrameBuffer = (WireFrameBufferType*)mappedResource.pData;

	wireFrameBuffer->wireColor = isWireFrame ? XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) : XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_wireFrameBuffer, 0);

	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	bufferNumber++;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_wireFrameBuffer);
	bufferNumber++;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 해당 함수의 StartIndexLocation, BaseVertexLocation을 통해서 버퍼를 끊어서 셰이더에 전달가능
	// IA 단계 설정에도 Stride와 Offset을 이용하면 이와 같은 동작을 수행할 수 있다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
