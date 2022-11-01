#include "ModelShader.h"
#include <iostream>
using namespace std;

ModelShader::ModelShader()
{
}

ModelShader::ModelShader(const ModelShader&)
{
}

ModelShader::~ModelShader()
{
}

void ModelShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	//정점 셰이더와 픽셀 셰이더를 초기화한다.
	InitializeShader(device, hwnd, L"./contents/shader/modelshader.vs", L"./contents/shader/modelshader.ps");
}

void ModelShader::Shutdown()
{
	ShutdownShader();
}

void ModelShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[7];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC materialBufferDesc;
	D3D11_BUFFER_DESC BoneBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// 이 함수에서 사용하는 포인터들을 null로 설정한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 컴파일한다.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vs_main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);
		}
	}

	// 픽셀 셰이더를 컴파일한다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ps_main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else {
			MessageBox(hwnd, nullptr, "Missing Shader File", MB_OK);
		}
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Shader File", MB_OK);
	}

	// 버퍼로부터 픽셀 셰이더를 생성한다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Shader File", MB_OK);
	}

	// 정점 입력 레이아웃 description을 작성한다.
	// 이 설정은 ModelClass와 셰이더에 있는 VertexType와 일치해야 한다.
	// 입력 레이아웃의 두번째 정점 성분도 바꾼다. 
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

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "BLENDINDICES";
	polygonLayout[5].SemanticIndex = 0;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[5].InputSlot = 0;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[5].InstanceDataStepRate = 0;

	polygonLayout[6].SemanticName = "BLENDWEIGHT";
	polygonLayout[6].SemanticIndex = 0;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[6].InputSlot = 0;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[6].InstanceDataStepRate = 0;


	// 레이아웃의 요소 갯수를 가져온다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Input Layout", MB_OK);
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Matrix Buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Buffer", MB_OK);
	}

	// LightBuffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Buffer", MB_OK);
	}

	// Camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Buffer", MB_OK);
	}

	// Material Buffer
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBufferType);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&materialBufferDesc, NULL, &m_MaterialBuffer);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Buffer", MB_OK);
	}

	// Bone Buffer
	BoneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BoneBufferDesc.ByteWidth = sizeof(BoneBufferType);
	BoneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BoneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BoneBufferDesc.MiscFlags = 0;
	BoneBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&BoneBufferDesc, NULL, &m_BoneBuffer);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Buffer", MB_OK);
	}

	// samplerDesc 구조체의 description을 작성한다.
	// samplerDesc 구조체에서 가장 중요한 것은 Filter다 Filter는 최종 도현 표면에서 텍스처의 어느 픽셀이 사용되거나 혼합될 것인지 결정하는 방법이다.
	// 예를 들어 처리량 면에서는 무겁지만 가장 좋은 결과를 보여주는  D3D11_FILTER_MIN_MAG_MIP_LINEAR 사용한다.

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

	// 텍스처샘플러를 생성한다. 
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) {
		MessageBox(hwnd, nullptr, "Can't Create Texture Sampler", MB_OK);
	}
}

void ModelShader::ShutdownShader()
{
	if (m_BoneBuffer)
	{
		delete m_BoneBuffer;
		m_BoneBuffer = 0;
	}
	if (m_MaterialBuffer)
	{
		delete m_MaterialBuffer;
		m_MaterialBuffer = 0;
	}
	if (m_LightBuffer)
	{
		delete m_LightBuffer;
		m_LightBuffer = 0;
	}
	if (m_CameraBuffer)
	{
		delete m_CameraBuffer;
		m_CameraBuffer = 0;
	}
}

void ModelShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT3 lightDir, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisvieTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture, XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning)
{
	SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, cameraPos, lightColor, lightPos, lightDir, ambientColor, emmisiveColor, diffuseColor, specularColor, shinness, ambientTexture, emmisvieTexture, diffuseTexture, specularTexture, normalTexture, boneScale, boneMatrixArray, skinning);

	// 셰이더의 인자를 다 받아왔다면 셰이더 이용하여 활성화된 버퍼를 그린다.
	RenderShader(deviceContext, indexCount);
}

void ModelShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT3 lightDir, XMFLOAT4 ambientColor, XMFLOAT4 emmisiveColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, FLOAT shinness, ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisvieTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture, XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixBuffer;
	LightBufferType* lightBuffer;
	CameraBufferType* cameraBuffer;
	MaterialBufferType* materialBuffer;
	BoneBufferType* boneBuffer;
	UINT vertexShaderBufferCount = -1;
	UINT pixelShaderBufferCount = -1;
	

	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Matrix Buffer
	// 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	matrixBuffer = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사한다.
	matrixBuffer->world = worldMatrix;
	matrixBuffer->view = viewMatrix;
	matrixBuffer->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	vertexShaderBufferCount++;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->VSSetConstantBuffers(vertexShaderBufferCount, 1, &m_matrixBuffer);

	// LightBuffer
	deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	lightBuffer = (LightBufferType*)mappedResource.pData;

	lightBuffer->lightColor = lightColor;
	lightBuffer->lightPos = lightPos;
	lightBuffer->lightDir = lightDir;
	lightBuffer->padding = XMFLOAT2(0.0f, 0.0f);

	deviceContext->Unmap(m_LightBuffer, 0);

	vertexShaderBufferCount++;

	deviceContext->VSSetConstantBuffers(vertexShaderBufferCount, 1, &m_LightBuffer);

	// Camera Buffer
	deviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	cameraBuffer = (CameraBufferType*)mappedResource.pData;

	cameraBuffer->cameraPosition = cameraPos;
	cameraBuffer->padding = 0.0f;

	deviceContext->Unmap(m_CameraBuffer, 0);

	vertexShaderBufferCount++;

	deviceContext->VSSetConstantBuffers(vertexShaderBufferCount, 1, &m_CameraBuffer);

	// Material Buffer
	deviceContext->Map(m_MaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	materialBuffer = (MaterialBufferType*)mappedResource.pData;

	materialBuffer->ambientColor = ambientColor;
	materialBuffer->emmisiveColor = emmisiveColor;
	materialBuffer->diffuseColor = diffuseColor;
	materialBuffer->specularColor = specularColor;
	materialBuffer->shinness = shinness;
	materialBuffer->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_MaterialBuffer, 0);

	pixelShaderBufferCount++;

	deviceContext->PSSetConstantBuffers(pixelShaderBufferCount, 1, &m_MaterialBuffer);

	// Bone Buffer
	deviceContext->Map(m_BoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	boneBuffer = (BoneBufferType*)mappedResource.pData;

	boneBuffer->boneScale = boneScale;
	for (int i = 0; i < 100; i++)
	{
		boneBuffer->boneMatrixArray[i] = boneMatrixArray[i];
	}
	boneBuffer->skinning = skinning;
	boneBuffer->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_BoneBuffer, 0);

	vertexShaderBufferCount++;

	deviceContext->VSSetConstantBuffers(vertexShaderBufferCount, 1, &m_BoneBuffer);


	// Texture 전달 
	deviceContext->PSSetShaderResources(0, 1, &ambientTexture);
	deviceContext->PSSetShaderResources(1, 1, &emmisvieTexture);
	deviceContext->PSSetShaderResources(2, 1, &diffuseTexture);
	deviceContext->PSSetShaderResources(3, 1, &specularTexture);
	deviceContext->PSSetShaderResources(4, 1, &normalTexture);

	//cout << "Ambient Color : " << ambientColor.x << ", " << ambientColor.y << ", " << ambientColor.z << ", " << ambientColor.w << endl;
	//cout << "Diffuse Color : " << diffuseColor.x << ", " << diffuseColor.y << ", " << diffuseColor.z << ", " << diffuseColor.w << endl;
	//cout << "Specular Color : " << specularColor.x << ", " << specularColor.y << ", " << specularColor.z << ", " << specularColor.w << endl;
}

void ModelShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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
}
