#include "MaterialShader.h"

MaterialShader::MaterialShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}

MaterialShader::MaterialShader(const MaterialShader&)
{
}

MaterialShader::~MaterialShader()
{
}

bool MaterialShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"./contents/shader/material.vs", L"./contents/shader/material.ps");
	if (!result)
		return false;

	return true;
}

void MaterialShader::Shutdown()
{
	ShutdownShader();

	return;
}

bool MaterialShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView** textures)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, orthoMatrix, ambientColor, emmisiveColor, diffuseColor, specularColor, shinness, textures);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool MaterialShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC materialBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFileName, NULL, NULL, "MaterialVertexShader", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		else
			MessageBox(hwnd, "MaterialVertexShader", "Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFileName, NULL, NULL, "MaterialPixelShader", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		else
			MessageBox(hwnd, "MaterialPixelShader", "Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
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

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// Material Buffer
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBufferType);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&materialBufferDesc, NULL, &m_materialBuffer);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Buffer", MB_OK);
	}

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

	return true;
}

void MaterialShader::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
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

	return;
}

bool MaterialShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView** textures)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	MaterialBufferType* materialBuffer;
	unsigned int bufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	orthoMatrix = XMMatrixTranspose(orthoMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = orthoMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Material Buffer
	deviceContext->Map(m_materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	materialBuffer = (MaterialBufferType*)mappedResource.pData;

	materialBuffer->ambientColor = ambientColor;
	materialBuffer->emmisiveColor = emmisiveColor;
	materialBuffer->diffuseColor = diffuseColor;
	materialBuffer->specularColor = specularColor;
	materialBuffer->shinness = shinness;
	materialBuffer->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_materialBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_materialBuffer);


	deviceContext->PSSetShaderResources(0, 5, textures);
	return true;
}

void MaterialShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(nullptr, NULL, 0);
	deviceContext->DSSetShader(nullptr, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 여기서 픽셀 셰이더의 샘플러 상태를 설정하는 부분을 추가한다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
