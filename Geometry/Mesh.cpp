#include "pch.h"
#include "Mesh.h"
#include "Core.h"
#include "ResMgrClass.h"

MeshPart::MeshPart()
{
}

MeshPart::MeshPart(const MeshPart&)
{
}

MeshPart::~MeshPart()
{
}

void MeshPart::InitializeBuffers(ID3D11Device*)
{
	VertexType* vertices;
	UINT* indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = m_vertexArray.size();
	m_indexCount = m_indexArray.size();

	vertices = new VertexType[m_vertexCount];
	indices = new UINT[m_indexCount];

	for (int i = 0; i < m_vertexCount; i++)
		vertices[i] = *m_vertexArray[i];
	for (int i = 0; i < m_indexCount; i++)
		indices[i] = m_indexArray[i];

	// 정점 배열과 인덱스 배열이 채워졌으므로 이것으로 정점 버퍼와 인덱스 버퍼를 만든다.
	// 버퍼를 초기화 하기위해서는 버퍼의 구조체 서술이 필요하다.
	// 그 다음은 버퍼의 초기화에 사용할 구조체 서술이 필요하다.

	// 정점 버퍼 구조체 description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;   // 정점버퍼가 쓰이는 용도 (현재는 GPU가 버퍼의 자원을 읽어야 하므로 이 용도로 지정)
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;   // 정점버퍼의 크기 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // 정점버퍼의 경우 다음과 같은 플래그를 지정한다. 
	vertexBufferDesc.CPUAccessFlags = 0;    // CPU가 플래그에 접근하는 방식을 지정한다.(버퍼 생성 이후 CPU가 버퍼에 접근하지 않으면 0으로 지정)
	vertexBufferDesc.MiscFlags = 0;    // 정점버퍼에 대해서는 이 기타 플래그를 사용할 필요가 없다.
	vertexBufferDesc.StructureByteStride = 0;   // 구조적 버퍼에 저장된 원소 구조적 버퍼 이외의 버퍼는 0으로 설정하면 된다. 

	// 정점 데이터를 가리키는 보조 리소스 구조체를 서술한다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성한다.
	result = Core::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// 인덱스 버퍼도 정점버퍼와 똑같은 과정으로 진행한다.

	// 인덱스 버퍼 구조체를 서술한다.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 서술한다.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성한다.
	result = Core::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

// RenderBuffers 함수는 Render 함수에서 호출된다. 
// 이 함수의 목적은 정점 버퍼와 인덱스 버퍼를 GPU의 어셈블리의 버퍼로 활성화 시키는 것이다.
// 일단 GPU가 활성화된 정점버퍼를 가지게 되면 셰이더를 이용해서 버퍼의 내용을 그릴 수 있게 된다.
// 또한 이 함수에서는 이 정점을 삼각형이나 선분, 부채꼴 등 어떤 모양으로 그리게 될지 정의한다.
// 어셈블리의 입력에 정점버퍼와 인덱스 버퍼를 넣고 DirectX의  IASetPrimitiveTopology 함수를 이용해 GPU에게 이 정점들을 삼각형을 그리도록 주문한다. 
void MeshPart::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

Mesh::Mesh() {
}

Mesh::Mesh(const Mesh& other) { }

Mesh::~Mesh() { }

// Initialize 함수는 정점 버퍼와 인덱스 버퍼의 초기화 함수를 호출한다.
bool Mesh::Initialize(ID3D11Device* device, const wstring& _strFilePath) {
	bool result;

	// 모델을 가져온다.
	result = LoadMesh(_strFilePath);
	if (!result) {
		return false;
	}

	// 정점 버퍼와 인덱스 버퍼를 초기화한다.
	InitializeBuffers(device);

	return true;
}

void Mesh::Shutdown()
{
	for (int i = 0; i < m_meshParts.size(); i++)
	{
		m_meshParts[i]->GetIndexBuffer()->Release();
		m_meshParts[i]->GetVertexBuffer()->Release();
	}
	return;
}

void Mesh::InitializeBuffers(ID3D11Device* device) {

	for (int i = 0; i <m_meshParts.size(); i++)
		m_meshParts[i]->InitializeBuffers(device);
}

bool Mesh::LoadMesh(const wstring& _strFilePath)
{
	ifstream fin;

	// 파일을 연다.
	fin.open(_strFilePath.c_str());
	if (fin.fail()) 
		return false;

	string buffer;
	fin >> buffer;
	fin >> buffer;

	int meshPartCnt;
	fin >> meshPartCnt;

	for (int i = 0; i < meshPartCnt; i++)
	{
		fin >> buffer;  // Part Num
		fin >> buffer;  // VertexCount string
		fin >> buffer;  // :

		// Mesh Part Recursive
		MeshPart* part = new MeshPart();

		// Vertex Count
		int vertexCount = 0;
		fin >> vertexCount;
		part->SetVertexCount(vertexCount);

		fin >> buffer;  // IndexCount
		fin >> buffer;  // :

		// Index Count
		int indexCount = 0;
		fin >> indexCount;
		part->SetIndexCount(indexCount);


		fin >> buffer;  // material path string
		fin >> buffer;  // :

		// material path
		fin >> buffer;  // relativePath 
		wstring relativePath = Core::ConvCharToWchar((char*)buffer.c_str());
		relativePath = L"contents\\assets\\" + relativePath;
		ResMgrClass::GetInst()->LoadMaterial(Core::GetFileName(relativePath), relativePath);
		part->SetMaterialID(Core::GetFileName(relativePath));

		// Vertex Data
		fin >> buffer;  // Vetex_Data string
		fin >> buffer;  // :
		for (int j = 0; j < vertexCount; j++)
		{
			VertexType* vertex = new VertexType;
			fin >> vertex->position.x >> vertex->position.y >> vertex->position.z;
			fin >> vertex->uv.x >> vertex->uv.y;
			fin >> vertex->normal.x >> vertex->normal.y >> vertex->normal.z;
			fin >> vertex->binormal.x >> vertex->binormal.y >> vertex->binormal.z;
			fin >> vertex->tangent.x >> vertex->tangent.y >> vertex->tangent.z;
			fin >> vertex->boneidx.x >> vertex->boneidx.y >> vertex->boneidx.z >> vertex->boneidx.w;
			fin >> vertex->boneWeight.x >> vertex->boneWeight.y >> vertex->boneWeight.z >> vertex->boneWeight.w;
			part->AddVertex(vertex);
		}

		fin >> buffer; // Index Data String
		fin >> buffer; // :
		for (int k = 0; k < indexCount; k++)
		{
			UINT index;
			fin >> index;
			part->AddIndex(index);
		}

		m_meshParts.push_back(part);
	}
	fin.close();
}