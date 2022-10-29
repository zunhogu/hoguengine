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

	//정점 셰이더와 픽셀 셰이더를 초기화한다.
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

	// 셰이더 초기화 순서는 셰이더 컴파일 -> 셰이더 생성 -> 셰이더에서 사용할 정점 성분 구조체를 description을 서술 한다 -> 구조체를 입력 레이아웃에 집어넣어는다.
	// 입력 레이아웃(입력 배치)에 넣게되면 Direct3D에게 정점 성분 정보를 알려준다.

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

	// 이 함수에서 사용하는 포인터들을 null로 설정한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	hullShaderBuffer = 0;
	domainShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 컴파일한다.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vs_main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		// 셰이더가 컴파일에 실패하면 에러 메세지를 기록한다.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메세지가 없다면 셰이더 파일을 찾지 못한다.
		else {
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// 헐 셰이더 컴파일한다.
	result = D3DCompileFromFile(hsFilename, NULL, NULL, "hs_main", "hs_5_0", 0, 0, &hullShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		else 
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);

		return false;
	}

	// 도메인 셰이더 컴파일한다.
	result = D3DCompileFromFile(dsFilename, NULL, NULL, "ds_main", "ds_5_0", 0, 0, &domainShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		else 
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);

		return false;
	}

	// 픽셀 셰이더를 컴파일한다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ps_main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else 
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);

		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	// 버퍼로부터 헐 셰이더를 생성한다.
	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result)) {
		return false;
	}

	// 버퍼로부터 도메인셰이더를 생성한다.
	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result)) {
		return false;
	}

	// 버퍼로부터 픽셀 셰이더를 생성한다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// 정점 입력 레이아웃 description을 작성한다.
	// 이 설정은 ModelClass와 셰이더에 있는 VertexType와 일치해야 한다.
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


	// 레이아웃의 요소 갯수를 가져온다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	// 더 이상 사용되지 않는 셰이더 버퍼는 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	hullShaderBuffer->Release();
	hullShaderBuffer = 0;

	domainShaderBuffer->Release();
	domainShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 도메인 셰이더에서 사용하는 행렬 상수 버퍼의 description을 작성한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// 헐 셰이더에서 사용하는 상수버퍼 description을 작성한다.
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

	// 레이아웃을 해제한다.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더를 해제한다.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 도메인 셰이더를 해제한다.
	if (m_domainShader) {
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// 헐 셰이더를 해제한다.
	if (m_hullShader) {
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// 정점 셰이더를 해제한다.
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


	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 행렬 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// 행렬 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 행렬 상수 버퍼에 행렬을 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 행렬 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 마지막으로 정점 셰이더의 행렬 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 테셀레이션 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// 테셀레이션 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	dataPtr2 = (TessellationBufferType*)mappedResource.pData;

	// 테셀레이션 상수 버퍼에 행렬을 복사한다.
	dataPtr2->tessellationAmount = tessellationAmount;

	// 테셀레이션 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_tessellationBuffer, 0);

	// 테셀레이션 정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 테셀레이션 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}

void TerrainWireFrameShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
