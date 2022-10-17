#include "pch.h"
#include "GridQuadTreeClass.h"
#include "Core.h"

GridQuadTreeClass::GridQuadTreeClass()
{
    m_vertexList = 0;
    m_parentNode = 0;
}

GridQuadTreeClass::GridQuadTreeClass(const GridQuadTreeClass&)
{
}

GridQuadTreeClass::~GridQuadTreeClass()
{
}

bool GridQuadTreeClass::Initialize(GridClass* grid, ID3D11Device* device)
{
	int vertexCount;
	float centerX, centerZ, width;

	// QuadTreeClass가 가장 처음해야할 일은 TerrainClass로 부터 정보를 얻는 것이다. 

	// terrain으로부터 vertexCount를 얻는다.
	vertexCount = grid->GetVertexCount();

	// vertexList를 초기화 하기위해서 squareCount를 계산한다.
	m_squareCount = vertexCount / 8;

	// vertexList를 초기화한다.
	m_vertexList = new VertexType[vertexCount];
	if (!m_vertexList)
		return false;

	grid->CopyVertexArray((void*)m_vertexList);

	// 일단 상위 노드의 정점 정보가 채워지고 나면은 부모노드의 차원을 계산하는 것이 가능하고, 하위 쿼드 트리를 재귀함수를 통해 구성할 수 있따. 

	// Calculate the center x,z and the width of the mesh.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// Create the parent node for the quad tree.
	m_parentNode = new NodeType;
	if (!m_parentNode)
		return false;

	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(m_parentNode, centerX, centerZ, width, device);

	// 쿼드 트리가 구축되고 나면 vertexList는 더이상 필요하지 않기 때문에 Release 시켜준다.
	if (m_vertexList)
	{
		delete[] m_vertexList;
		m_vertexList = 0;
	}

	return true;
}

void GridQuadTreeClass::Shutdown()
{
	if (m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}

	return;
}

void GridQuadTreeClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Render함수에서는 RenderNode를 호출한다.
	// RenderNode함수는 FrustumClas를 사용하여 노드를 렌더링한다. 

	// 렌더링이 끝나기 전에 현재 프레임에서 그린 삼각형의 개수를 0으로 초기화 해야하는데 그 이유는 각각의 노드에 대해 계속 증가하기 때문이다. 
	m_drawCount = 0;

	// Render each node that is visible starting at the parent node and moving down the tree.
	RenderNode(m_parentNode, deviceContext);
}

// 이전 렌더 함수 호출에서 그려진 삼각형의 수를 반환한다.
int GridQuadTreeClass::GetDrawCount()
{
	return m_drawCount;
}

// CalculateMeshDimensions()은 물리적인 부모 노드의 쿼드 사이즈를 계산한다.
// 또 vertexList에 있는 모든 정점을 통과하여 지형의 중심과 지형의 절대 최대 너비를 계산한다. 이것은 부모노드의 크기를 만드는 데 사용된다.
void GridQuadTreeClass::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	int i;
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;

	// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;

	// Sum all the vertices in the mesh.
	for (i = 0; i < vertexCount; i++)
	{
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

	// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(m_vertexList[0].position.x - centerX);
	minDepth = fabsf(m_vertexList[0].position.z - centerZ);

	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for (i = 0; i < vertexCount; i++)
	{
		width = fabsf(m_vertexList[i].position.x - centerX);
		depth = fabsf(m_vertexList[i].position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// Calculate the maximum diameter of the mesh.
	meshWidth = max(maxX, maxZ) * 2.0f;
}

void GridQuadTreeClass::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
	int numSquares, i, count, vertexCount, index, vertexIndex;
	float offsetX, offsetZ;
	VertexType* vertices;
	unsigned long* indices;
	bool result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Store the node position and size in world.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// Initialize the triangle count to zero for the node.
	node->squareCount = 0;

	// Initialize the vertex and index buffer to null.
	node->vertexBuffer = 0;
	node->indexBuffer = 0;

	// Initialize the children nodes of this node to null.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// 현재 노드의 triangle Count를 계싼한다. 
	numSquares = CountSquares(positionX, positionZ, width);

	// triangle의 Count에 따라 현재 노드가 어떻게 진행될지 결정되는데

	// Case 1 : 노드 안에 triangle 0개 일때 그냥 노드 완성
	if (numSquares == 0)
		return;

	// Case 2 : 노드 안에 triangle이 MAX_SQUARES을 넘어서서 쿼드가 split 되어야 할때 
	if (numSquares > MAX_SQUARE)
	{
		for (i = 0; i < 4; i++)
		{
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// See if there are any triangles in the new node.
			count = CountSquares((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// If there are triangles inside where this new node would be then create the child node.
				node->nodes[i] = new NodeType;

				// Extend the tree starting from this new child node now.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
			}
		}

		return;
	}

	// Case3 : MAX_SQUARE 이하의 square Count를 가진다면 정점 및 인덱스 버퍼를 만들고 노드로 로드한다.
	// 해당 노드는 리프노드여야 한다.
	node->squareCount = numSquares;

	// Calculate the number of vertices.
	vertexCount = numSquares * 8;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];

	// Create the index array.
	indices = new unsigned long[vertexCount];

	// Initialize the index for this new vertex and index array.
	index = 0;

	// Go through all the triangles in the vertex list.
	for (i = 0; i < m_squareCount; i++)
	{
		// If the triangle is inside this node then add it to the vertex array.
		result = IsSquareContained(i, positionX, positionZ, width);
		if (result == true)
		{
			// Calculate the index into the terrain vertex list.
			vertexIndex = i * 8;

			// Get the three vertices of this triangle from the vertex list.
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
			vertexIndex++;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].color = m_vertexList[vertexIndex].color;
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return;
}

int GridQuadTreeClass::CountSquares(float positionX, float positionZ, float width)
{
	int count, i;
	bool result;

	// Initialize the count to zero.
	count = 0;

	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for (i = 0; i < m_squareCount; i++)
	{
		// If the triangle is inside the node then increment the count by one.
		result = IsSquareContained(i, positionX, positionZ, width);
		if (result == true)
			count += 1;
	}

	return count;
}

bool GridQuadTreeClass::IsSquareContained(int index, float positionX, float positionZ, float width)
{
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3, x4, z4;
	float minimumX, maximumX, minimumZ, maximumZ;


	// Calculate the radius of this node.
	radius = width / 2.0f;

	// Get the index into the vertex list.
	vertexIndex = index * 8;

	// Get the three vertices of this triangle from the vertex list.
	x1 = m_vertexList[vertexIndex].position.x;
	z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex += 2;

	x2 = m_vertexList[vertexIndex].position.x;
	z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex += 2;

	x3 = m_vertexList[vertexIndex].position.x;
	z3 = m_vertexList[vertexIndex].position.z;
	vertexIndex += 2;

	x4 = m_vertexList[vertexIndex].position.x;
	z4 = m_vertexList[vertexIndex].position.z;

	// first triangle
	minimumX = min(x1, min(x2, min(x3, x4)));
	if (minimumX > (positionX + radius))
		return false;

	maximumX = max(x1, max(x2, max(x3, x4)));
	if (maximumX < (positionX - radius))
		return false;

	minimumZ = min(z1, min(z2, min(z3, z4)));
	if (minimumZ > (positionZ + radius))
		return false;

	maximumZ = max(z1, max(z2, max(z3, z4)));
	if (maximumZ < (positionZ - radius))
		return false;

	return true;
}

void GridQuadTreeClass::ReleaseNode(NodeType* node)
{
	int i;


	// Recursively go down the tree and release the bottom nodes first.
	for (i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// Release the vertex buffer for this node.
	if (node->vertexBuffer)
	{
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}

	// Release the index buffer for this node.
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	// Release the four child nodes.
	for (i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}

	return;
}

void GridQuadTreeClass::RenderNode(NodeType* node, ID3D11DeviceContext* deviceContext)
{
	bool result;
	int count, i, indexCount;
	unsigned int stride, offset;

	// Frustum Check
	result = CollisionClass::GetInst()->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));
	cout << result << endl;
	if (!result)
		return;

	// 만약 Frustum Check의 결과 값이 true가 나오면 자식노드로 해당 함수를 재귀적으로 호출한다.
	count = 0;
	for (i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], deviceContext);
		}
	}

	// 자식 노드가 없다면은 렌더링할 삼각형을 더이상 포함하지 않으므로 진행하지 않아도 된다.
	if (count != 0)
		return;

	// 해당 노드의 버퍼를 렌더링 해준다.

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


	// Determine the number of indices in this node.
	indexCount = node->squareCount * 8;

	// Call the terrain shader to render the polygons in this node.
	GraphicsClass::GetInst()->RenderGirdShader(deviceContext, indexCount);

	// Increase the count of the number of polygons that have been rendered during this frame.
	m_drawCount += node->squareCount;

	return;
}
