#include "pch.h"
#include "TerrainMesh.h"
#include "Core.h"

#define GRID_SIZE 256

////////////////////////////////////////////////////////////////////////////////////////////
// TERRAIN MESH ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
TerrainMesh::TerrainMesh()
	: m_terrainHeight(GRID_SIZE), m_terrainWidth(GRID_SIZE), m_heightMapWidth(0), m_heightMapHeight(0)
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

			index = (m_heightMapHeight * j) + i;

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
		return false;

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
			if ((i < (m_heightMapWidth - 1)) && ((j - 1) >= 0))
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
			if ((i < (m_heightMapWidth - 1)) && (j < (m_heightMapHeight - 1)))
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
	int index;
	int index1, index2, index3, index4;
	float tu, tv;
	int gridSize = m_terrainWidth * m_terrainHeight;
	int heightMapSize = m_heightMapWidth * m_heightMapHeight;

	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	m_vertices = new TerrainVertexType[m_vertexCount];
	if (!m_vertices) return false;

	index = 0;

	if (gridSize <= heightMapSize)
	{
		// gridSize와 heightMapSize가 같다면 단순히 값을 복사해주면 된다.
		// girdSize가 더 작다는 것은 HeightMap에서 몇가지 값을 버려줘야한다는 것이다
		// 버려주는 동시에 디테일은 감소되지만 Low polygon을 가질 수 있다.

		int offset = m_heightMapWidth / m_terrainWidth;

		for (int j = 0; j < (m_heightMapHeight - offset); j += offset)
		{
			for (int i = 0; i < (m_heightMapWidth - offset); i += offset)
			{
				index1 = (m_heightMapHeight * j) + i;          // Bottom left.
				index2 = (m_heightMapHeight * j) + (i + offset);      // Bottom right.
				index3 = (m_heightMapHeight * (j + offset)) + i;      // Upper left.
				index4 = (m_heightMapHeight * (j + offset)) + (i + offset);  // Upper right.

				// 1. Upper Left 
				tv = m_heightMap[index3].tv;
				if (tv == 1.0f) { tv = 0.0f; }
				m_vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
				m_vertices[index].texture = XMFLOAT2(m_heightMap[index3].tu, tv);
				m_vertices[index].normal = XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
				index++;

				// 2. Upper Right
				tu = m_heightMap[index4].tu;
				tv = m_heightMap[index4].tv;
				if (tu == 0.0f) { tu = 1.0f; }
				if (tv == 1.0f) { tv = 0.0f; }
				m_vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				m_vertices[index].texture = XMFLOAT2(tu, tv);
				m_vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				index++;

				// 3. Bottom Left
				m_vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				m_vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
				m_vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				index++;

				// 4. Bottom Left
				m_vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				m_vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
				m_vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				index++;

				// 5. Upper Right
				tu = m_heightMap[index4].tu;
				tv = m_heightMap[index4].tv;
				if (tu == 0.0f) { tu = 1.0f; }
				if (tv == 1.0f) { tv = 0.0f; }
				m_vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				m_vertices[index].texture = XMFLOAT2(tu, tv);
				m_vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				index++;

				// 6. Bottom Right
				tu = m_heightMap[index2].tu;
				if (tu == 0.0f) { tu = 1.0f; }
				m_vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
				m_vertices[index].texture = XMFLOAT2(tu, m_heightMap[index2].tv);
				m_vertices[index].normal = XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
				index++;
			}
		}
	}
	else if (gridSize > heightMapSize)
	{
		// gridSize가 heightMapSize 보다 크다면 heightMap에는 없는 새로운 값을 보간하여 넣어줘야한다.

		int count = m_terrainWidth / m_heightMapWidth + 1;  // 보간으로 얻을 값들의 개수

		for (int j = 0; j < (m_heightMapHeight - 1); j++)
		{
			for (int i = 0; i < (m_heightMapWidth - 1); i++)
			{
				index1 = (m_heightMapHeight * j) + i;          // Bottom left.
				index2 = (m_heightMapHeight * j) + (i + 1);      // Bottom right.
				index3 = (m_heightMapHeight * (j + 1)) + i;      // Upper left.
				index4 = (m_heightMapHeight * (j + 1)) + (i + 1);  // Upper right.

				HeightMapType upper_left = m_heightMap[index3];
				HeightMapType upper_right = m_heightMap[index4];
				HeightMapType bottom_left = m_heightMap[index1];
				HeightMapType bottom_right = m_heightMap[index2];

				vector<HeightMapType> crd1, crd2, crd;

				// upper_left <-> bottom_left
				for (int i = 0; i < count; i++)
				{
					HeightMapType temp;
					temp.x = LinearInterpoation(upper_left.x, bottom_left.x, (float)i / (count-1));
					temp.y = LinearInterpoation(upper_left.y, bottom_left.y, (float)i / (count - 1));
					temp.z = LinearInterpoation(upper_left.z, bottom_left.z, (float)i / (count - 1));

					temp.tu = LinearInterpoation(upper_left.tu, bottom_left.tu, (float)i / (count - 1));
					temp.tv = LinearInterpoation(upper_left.tv, bottom_left.tv, (float)i / (count - 1));

					temp.nx = LinearInterpoation(upper_left.nx, bottom_left.nx, (float)i / (count - 1));
					temp.ny = LinearInterpoation(upper_left.ny, bottom_left.ny, (float)i / (count - 1));
					temp.nz = LinearInterpoation(upper_left.nz, bottom_left.nz, (float)i / (count - 1));

					crd1.push_back(temp);
				}

				// upper_right <-> bottom right;
				for (int i = 0; i < count; i++)
				{
					HeightMapType temp;
					temp.x = LinearInterpoation(upper_right.x, bottom_right.x, (float)i / (count - 1));
					temp.y = LinearInterpoation(upper_right.y, bottom_right.y, (float)i / (count - 1));
					temp.z = LinearInterpoation(upper_right.z, bottom_right.z, (float)i / (count - 1));

					temp.tu = LinearInterpoation(upper_right.tu, bottom_right.tu, (float)i / (count - 1));
					temp.tv = LinearInterpoation(upper_right.tv, bottom_right.tv, (float)i / (count - 1));

					temp.nx = LinearInterpoation(upper_right.nx, bottom_right.nx, (float)i / (count - 1));
					temp.ny = LinearInterpoation(upper_right.ny, bottom_right.ny, (float)i / (count - 1));
					temp.nz = LinearInterpoation(upper_right.nz, bottom_right.nz, (float)i / (count - 1));

					crd2.push_back(temp);
				}

				// crd1 <-> crd2 interpolation
				for (int i = 0; i < crd1.size(); i++)
				{
					for (int j = 0; j < count; j++)
					{
						HeightMapType temp;
						temp.x = LinearInterpoation(crd1[i].x, crd2[i].x, (float)j / (count - 1));
						temp.y = LinearInterpoation(crd1[i].y, crd2[i].y, (float)j / (count - 1));
						temp.z = LinearInterpoation(crd1[i].z, crd2[i].z, (float)j / (count - 1));

						temp.tu = LinearInterpoation(crd1[i].tu, crd2[i].tu, (float)j / (count - 1));
						temp.tv = LinearInterpoation(crd1[i].tv, crd2[i].tv, (float)j / (count - 1));

						temp.nx = LinearInterpoation(crd1[i].nx, crd2[i].nx, (float)j / (count - 1));
						temp.ny = LinearInterpoation(crd1[i].ny, crd2[i].ny, (float)j / (count - 1));
						temp.nz = LinearInterpoation(crd1[i].nz, crd2[i].nz, (float)j / (count - 1));

						crd.push_back(temp);
					}
				}

				// 보간을 한 값으로 정점배열을 생성
				for (int j = 0; j < count-1; j++)
				{
					for (int i = 0; i < count-1; i++)
					{
						index1 = (count * j) + i;          // Bottom left.
						index2 = (count * j) + (i + 1);      // Bottom right.
						index3 = (count * (j + 1)) + i;      // Upper left.
						index4 = (count * (j + 1)) + (i + 1);  // Upper right.

						// 1. Upper Left 
						tv = m_heightMap[index3].tv;
						if (tv == 1.0f) { tv = 0.0f; }
						m_vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
						m_vertices[index].texture = XMFLOAT2(m_heightMap[index3].tu, tv);
						m_vertices[index].normal = XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
						index++;

						// 2. Upper Right
						tu = m_heightMap[index4].tu;
						tv = m_heightMap[index4].tv;
						if (tu == 0.0f) { tu = 1.0f; }
						if (tv == 1.0f) { tv = 0.0f; }
						m_vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
						m_vertices[index].texture = XMFLOAT2(m_heightMap[index4].tu, tv);
						m_vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
						index++;

						// 3. Bottom Left
						m_vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
						m_vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
						m_vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
						index++;

						// 4. Bottom Left
						m_vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
						m_vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
						m_vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
						index++;

						// 5. Upper Right
						tu = m_heightMap[index4].tu;
						tv = m_heightMap[index4].tv;
						if (tu == 0.0f) { tu = 1.0f; }
						if (tv == 1.0f) { tv = 0.0f; }
						m_vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
						m_vertices[index].texture = XMFLOAT2(m_heightMap[index4].tu, tv);
						m_vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
						index++;

						// 6. Bottom Right
						tu = m_heightMap[index2].tu;
						if (tu == 0.0f) { tu = 1.0f; }
						m_vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
						m_vertices[index].texture = XMFLOAT2(tu, m_heightMap[index2].tv);
						m_vertices[index].normal = XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
						index++;
					}
				}
			}
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

float TerrainMesh::LinearInterpoation(float A, float B, float Alpha)
{
	return A * (1 - Alpha) + B * Alpha;
}

float TerrainMesh::BilinearInterpolation(float A, float B, float C, float D, float Alpha, float Beta)
{
	return (1 - Beta) * (1 - Alpha) * A + Beta * Alpha * B + Beta * (1 - Alpha) * D + (1 - Beta) * Alpha * C;
}
