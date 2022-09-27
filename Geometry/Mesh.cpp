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

	// ���� �迭�� �ε��� �迭�� ä�������Ƿ� �̰����� ���� ���ۿ� �ε��� ���۸� �����.
	// ���۸� �ʱ�ȭ �ϱ����ؼ��� ������ ����ü ������ �ʿ��ϴ�.
	// �� ������ ������ �ʱ�ȭ�� ����� ����ü ������ �ʿ��ϴ�.

	// ���� ���� ����ü description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;   // �������۰� ���̴� �뵵 (����� GPU�� ������ �ڿ��� �о�� �ϹǷ� �� �뵵�� ����)
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;   // ���������� ũ�� 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // ���������� ��� ������ ���� �÷��׸� �����Ѵ�. 
	vertexBufferDesc.CPUAccessFlags = 0;    // CPU�� �÷��׿� �����ϴ� ����� �����Ѵ�.(���� ���� ���� CPU�� ���ۿ� �������� ������ 0���� ����)
	vertexBufferDesc.MiscFlags = 0;    // �������ۿ� ���ؼ��� �� ��Ÿ �÷��׸� ����� �ʿ䰡 ����.
	vertexBufferDesc.StructureByteStride = 0;   // ������ ���ۿ� ����� ���� ������ ���� �̿��� ���۴� 0���� �����ϸ� �ȴ�. 

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �����Ѵ�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����Ѵ�.
	result = Core::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// �ε��� ���۵� �������ۿ� �Ȱ��� �������� �����Ѵ�.

	// �ε��� ���� ����ü�� �����Ѵ�.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �����Ѵ�.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����Ѵ�.
	result = Core::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

// RenderBuffers �Լ��� Render �Լ����� ȣ��ȴ�. 
// �� �Լ��� ������ ���� ���ۿ� �ε��� ���۸� GPU�� ������� ���۷� Ȱ��ȭ ��Ű�� ���̴�.
// �ϴ� GPU�� Ȱ��ȭ�� �������۸� ������ �Ǹ� ���̴��� �̿��ؼ� ������ ������ �׸� �� �ְ� �ȴ�.
// ���� �� �Լ������� �� ������ �ﰢ���̳� ����, ��ä�� �� � ������� �׸��� ���� �����Ѵ�.
// ������� �Է¿� �������ۿ� �ε��� ���۸� �ְ� DirectX��  IASetPrimitiveTopology �Լ��� �̿��� GPU���� �� �������� �ﰢ���� �׸����� �ֹ��Ѵ�. 
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

// Initialize �Լ��� ���� ���ۿ� �ε��� ������ �ʱ�ȭ �Լ��� ȣ���Ѵ�.
bool Mesh::Initialize(ID3D11Device* device, const wstring& _strFilePath) {
	bool result;

	// ���� �����´�.
	result = LoadMesh(_strFilePath);
	if (!result) {
		return false;
	}

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�Ѵ�.
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

	// ������ ����.
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