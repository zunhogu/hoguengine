#include "pch.h"
#include "TerrainWireFrameShader.h"

TerrainWireFrameShader::TerrainWireFrameShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_hullShader = 0;
	m_domainShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_tessellationBuffer = 0;
}

TerrainWireFrameShader::TerrainWireFrameShader(const TerrainWireFrameShader&)
{
}

TerrainWireFrameShader::~TerrainWireFrameShader()
{
}

bool TerrainWireFrameShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, L"./contents/shader/terrain_wireframe.vs", 
											L"./contents/shader/terrain_wireframe.ps", 
											L"./contents/shader/terrain_wireframe.hs", 
											L"./contents/shader/terrain_wireframe.ds");
	if (!result)
		return false;
	return true;
}

void TerrainWireFrameShader::Shutdown()
{
	ShutdownShader();

	return;
}

bool TerrainWireFrameShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, WCHAR* hsFilename, WCHAR* dsFilename)
{

	// ���̴� �ʱ�ȭ ������ ���̴� ������ -> ���̴� ���� -> ���̴����� ����� ���� ���� ����ü�� description�� ���� �Ѵ� -> ����ü�� �Է� ���̾ƿ��� ����־�´�.
	// �Է� ���̾ƿ�(�Է� ��ġ)�� �ְԵǸ� Direct3D���� ���� ���� ������ �˷��ش�.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* hullShaderBuffer;
	ID3D10Blob* domainShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;

	// �� �Լ����� ����ϴ� �����͵��� null�� �����Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	hullShaderBuffer = 0;
	domainShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vs_main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����Ѵ�.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���Ѵ�.
		else {
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// �� ���̴� �������Ѵ�.
	result = D3DCompileFromFile(hsFilename, NULL, NULL, "hs_main", "hs_5_0", 0, 0, &hullShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		else 
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);

		return false;
	}

	// ������ ���̴� �������Ѵ�.
	result = D3DCompileFromFile(dsFilename, NULL, NULL, "ds_main", "ds_5_0", 0, 0, &domainShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		else 
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);

		return false;
	}

	// �ȼ� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ps_main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else 
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);

		return false;
	}

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	// ���۷κ��� �� ���̴��� �����Ѵ�.
	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result)) {
		return false;
	}

	// ���۷κ��� �����μ��̴��� �����Ѵ�.
	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result)) {
		return false;
	}

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// ���� �Է� ���̾ƿ� description�� �ۼ��Ѵ�.
	// �� ������ ModelClass�� ���̴��� �ִ� VertexType�� ��ġ�ؾ� �Ѵ�.
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


	// ���̾ƿ��� ��� ������ �����´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	// �� �̻� ������ �ʴ� ���̴� ���۴� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	hullShaderBuffer->Release();
	hullShaderBuffer = 0;

	domainShaderBuffer->Release();
	domainShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ������ ���̴����� ����ϴ� ��� ��� ������ description�� �ۼ��Ѵ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// �� ���̴����� ����ϴ� ������� description�� �ۼ��Ѵ�.
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

void TerrainWireFrameShader::ShutdownShader()
{
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_tessellationBuffer) {
		m_tessellationBuffer->Release();
		m_tessellationBuffer = 0;
	}

	// ���̾ƿ��� �����Ѵ�.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴��� �����Ѵ�.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ������ ���̴��� �����Ѵ�.
	if (m_domainShader) {
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// �� ���̴��� �����Ѵ�.
	if (m_hullShader) {
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// ���� ���̴��� �����Ѵ�.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

bool TerrainWireFrameShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TessellationBufferType* dataPtr2;
	unsigned int bufferNumber;


	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// ��� ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// �׼����̼� ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// �׼����̼� ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr2 = (TessellationBufferType*)mappedResource.pData;

	// �׼����̼� ��� ���ۿ� ����� �����Ѵ�.
	dataPtr2->tessellationAmount = tessellationAmount;

	// �׼����̼� ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_tessellationBuffer, 0);

	// �׼����̼� ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// �׼����̼� ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}

void TerrainWireFrameShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
