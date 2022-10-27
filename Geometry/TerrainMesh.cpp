#include "pch.h"
#include "TerrainMesh.h"
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TERRAIN MESH ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
TerrainMesh::TerrainMesh()
	: m_terrainHeight(100), m_terrainWidth(100), m_heightMapWidth(0), m_heightMapHeight(0)
{
}

TerrainMesh::TerrainMesh(int width, int height)
{
	m_terrainWidth = width;
	m_terrainHeight = height;
	m_heightMapWidth = 0;
	m_heightMapHeight = 0;
}

TerrainMesh::~TerrainMesh()
{
}

TerrainMesh::TerrainMesh(const TerrainMesh&)
{
}

bool TerrainMesh::Initialize(ID3D11Device* device, char* heightFileName)
{
	bool result = true;

	// 현재 Height 맵과 Gird에 대한 정점 배열은 따로 구했다.
	// Gird와 Height맵의 사이즈가 다르기 때문에 보간을 통해서 새로운 정점 배열을 만든뒤 쿼드트리로 던져줘야한다.
	// 또 현재는 기본도형 위상구조가 Line인데, 텍스쳐 맵핑을 위해서는 Triangle로 바꿔야한다. 이에 대해서는 좀 더 고민해보자

	m_worldMatrix = XMMatrixTranslation(-m_terrainWidth / 2, 0.0f, -m_terrainHeight / 2);

	string path = Core::ConvWcharTochar(PathMgr::GetInst()->GetContentPath());
	path += heightFileName;

	// Load in the height map for the terrain.
	result = LoadHeightMap((char*)path.c_str());
	if (!result)
		return false;

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// 법선에 대해서 계산한다.
	result = CalculateNormals();
	if (!result)
		return false;

	CalculateTextureCoordinates();

	InitializeBuffers(device);
	if (!result)
		return false;
}

void TerrainMesh::Shutdown()
{
	ShutdownBuffers();
}

int TerrainMesh::GetVertexCount()
{
	return m_vertexCount;
}

void TerrainMesh::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(TerrainVertexType) * m_vertexCount);
	return;
}

bool TerrainMesh::LoadHeightMap(char* path)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	error = fopen_s(&filePtr, path, "rb");
	if (error != 0)
		return false;

	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
		return false;

	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
		return false;

	m_heightMapWidth = bitmapInfoHeader.biWidth;
	m_heightMapHeight = bitmapInfoHeader.biHeight;

	imageSize = m_heightMapWidth * m_heightMapHeight * 3;

	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
		return false;

	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
		return false;

	error = fclose(filePtr);
	if (error != 0)
		return false;

	// Height Map Array을 생성
	m_heightMap = new HeightMapType[m_heightMapWidth * m_heightMapHeight];
	if (!m_heightMap)
		return false;

	k = 0;

	// heightMap의 bitmap data가 bitmapImage 1차원 배열에 (x, y, z) 단위로 순서대로 들어가 있다.
	// x, z 값에는 해당 텍셀의 인덱스가, y에는 높이 값이 들어가 있음
	for (j = 0; j < m_heightMapHeight; j++)
	{
		for (i = 0; i < m_heightMapWidth; i++)
		{
			height = bitmapImage[k];

			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void TerrainMesh::NormalizeHeightMap()
{
	int i, j;

	for (j = 0; j < m_heightMapHeight; j++)
	{
		for (i = 0; i < m_heightMapWidth; i++)
		{
			m_heightMap[(m_heightMapHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}

bool TerrainMesh::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;

	normals = new VectorType[(m_heightMapWidth - 1) * (m_heightMapHeight - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_heightMapHeight - 1); j++)
	{
		for (i = 0; i < (m_heightMapWidth - 1); i++)
		{
			index1 = (j * m_heightMapHeight) + i;
			index2 = (j * m_heightMapHeight) + (i + 1);
			index3 = ((j + 1) * m_heightMapHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_heightMapHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < m_heightMapHeight; j++)
	{
		for (i = 0; i < m_heightMapWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_heightMapHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_heightMapHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_heightMapHeight - 1)))
			{
				index = (j * (m_heightMapHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_heightMapHeight - 1)))
			{
				index = (j * (m_heightMapHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_heightMapHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void TerrainMesh::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void TerrainMesh::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_heightMapWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_heightMapWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j < m_heightMapHeight; j++)
	{
		for (i = 0; i < m_heightMapWidth; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(m_heightMapHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_heightMapHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool TerrainMesh::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float positionX, positionZ;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 12;

	// Create the vertex array.
	m_vertices = new TerrainVertexType[m_vertexCount];
	if (!m_vertices)
		return false;

	// Initialize the index to the vertex array.
	index = 0;

	// Load the vertex and index arrays with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{

			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper left.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;
			m_vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			m_vertices[index].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			index++;
		}
	}

	return true;
}

void TerrainMesh::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}
}