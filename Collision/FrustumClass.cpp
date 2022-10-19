#include "pch.h"
#include "FrustumClass.h"

FrustumClass::FrustumClass() {}

FrustumClass::FrustumClass(const FrustumClass& other) {}

FrustumClass::~FrustumClass() {}

// GraphicsClass에 의해 ConstructFrustum 함수가 매 프레임마다 호출된다.
// GraphicsClass에서는 화면의 depth와 투영 행렬, 그리고 뷰 행렬을 전달한다.
// 이 입력들을 이용하여 현재 프레임의 시야 프리스텀 행렬을 만든다. 
// 이 시야 프리스텀 행렬로 시야 프리스텀을 구성하는 여섯 개의 평면을 계산해 낸다.
void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix) {
	float zMinimum, r;
	XMMATRIX matrix;
	XMFLOAT4X4  matrix4X4;
	XMFLOAT4X4 projectionMatrix4X4;

	// 절두체의 최소 깊이를 계산하여 투영행렬을 수정해준다.
	// 그러기 위해서는 투영행렬의 원소를 변경시켜줘야하는데
	// XMMATRIX는 행렬 원소에 접근하는 할 수 없다. 따라서 XMFLOAT4X4로 만들어 주는 것이 좋을 것같다.
	// 이때 사용하는 함수는 XMStoreFloat함수다 그 반대로 XMStoreFloat 함수도 있다.
	XMStoreFloat4x4(&projectionMatrix4X4, projectionMatrix);

	zMinimum = -projectionMatrix4X4._43 / projectionMatrix4X4._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix4X4._33 = r;
	projectionMatrix4X4._43 = -r * zMinimum;

	projectionMatrix = XMLoadFloat4x4(&projectionMatrix4X4);

	// 입력받은 투영행렬, 뷰행렬을 통해 시야 프리스텀 행렬을 생성한다. 
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMStoreFloat4x4(&matrix4X4, matrix);


	// 프러스텀 행렬을 통해 절두체 6면을 계산해준다.
	XMVECTOR resultPlane;
	// 근 평면
	m_planes[0].x = matrix4X4._14 + matrix4X4._13;
	m_planes[0].y = matrix4X4._24 + matrix4X4._23;
	m_planes[0].z = matrix4X4._34 + matrix4X4._33;
	m_planes[0].w = matrix4X4._44 + matrix4X4._43;
	resultPlane = XMLoadFloat4(&m_planes[0]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// 원평면
	m_planes[1].x = matrix4X4._14 - matrix4X4._13;
	m_planes[1].y = matrix4X4._24 - matrix4X4._23;
	m_planes[1].z = matrix4X4._34 - matrix4X4._33;
	m_planes[1].w = matrix4X4._44 - matrix4X4._43;
	resultPlane = XMLoadFloat4(&m_planes[1]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// 좌평면
	m_planes[2].x = matrix4X4._14 + matrix4X4._11;
	m_planes[2].y = matrix4X4._24 + matrix4X4._21;
	m_planes[2].z = matrix4X4._34 + matrix4X4._31;
	m_planes[2].w = matrix4X4._44 + matrix4X4._41;
	resultPlane = XMLoadFloat4(&m_planes[2]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// 우평면
	m_planes[3].x = matrix4X4._14 - matrix4X4._11;
	m_planes[3].y = matrix4X4._24 - matrix4X4._21;
	m_planes[3].z = matrix4X4._34 - matrix4X4._31;
	m_planes[3].w = matrix4X4._44 - matrix4X4._41;
	resultPlane = XMLoadFloat4(&m_planes[3]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// 상평면
	m_planes[4].x = matrix4X4._14 - matrix4X4._12;
	m_planes[4].y = matrix4X4._24 - matrix4X4._22;
	m_planes[4].z = matrix4X4._34 - matrix4X4._32;
	m_planes[4].w = matrix4X4._44 - matrix4X4._42;
	resultPlane = XMLoadFloat4(&m_planes[4]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// 하평면
	m_planes[5].x = matrix4X4._14 + matrix4X4._12;
	m_planes[5].y = matrix4X4._24 + matrix4X4._22;
	m_planes[5].z = matrix4X4._34 + matrix4X4._32;
	m_planes[5].w = matrix4X4._44 + matrix4X4._42;
	resultPlane = XMLoadFloat4(&m_planes[5]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	return;
}

// CheckPoint 함수는 하나의 점이 시야 프러스텀 내부에 있는지 확인하는 함수다.
// 내부에 있는 지 확인은 평면과 3D벡터의 내적을 구한다. 현재는 DirectMath 말고 공식을 이용해서 구한다.
// 올바른 환경에서 이 함수를 사용하면 다른 네개의 체크함수에 비해 가장 효율적이다.
// 이 함수는 점의 좌표를 입력으로 받고 모든 여섯 개의 평면 안에 점이 있는지 확인한다. 리턴값은 포함여부
bool FrustumClass::CheckPoint(float x, float y, float z) {
	int i;

	float result = 0.0f;
	for (i = 0; i < 6; i++) {
		result = m_planes[i].x * x + m_planes[i].y * y + m_planes[i].z * z + m_planes[i].w * 1.0f;
		if (result < 0.0f) {
			return false;
		}
	}
	return true;
}

// CheckCube 함수는 정육면체의 8개의 꼭지점 중 어느것이라도 시야 프레스텀 내에 있는지 확인한다.
// 입력으로는 육면체의 중점과 반경만 있으면 되며 이를 이용하여 여덟개의 꼭지점 위치를 계산할 수 있다.
// 만약 꼭지점이 하나라도 프리스텀 내부에 있다면 true를 리턴하고 그렇지 않다면 false를 리턴한다. 
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	XMFLOAT3 cubeVec;
	float result = 0.0f;

	for (i = 0; i < 6; i++) {
		cubeVec = XMFLOAT3(xCenter - radius, yCenter - radius, zCenter - radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + radius, yCenter - radius, zCenter - radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter - radius, yCenter + radius, zCenter - radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + radius, yCenter + radius, zCenter - radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter - radius, yCenter - radius, zCenter + radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + radius, yCenter - radius, zCenter + radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter - radius, yCenter + radius, zCenter + radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + radius, yCenter + radius, zCenter + radius);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		return false;
	}

	return true;
}

// CheckSphere 함수는 구의 중심이 시야 프러스텀의 여섯 평면에 반경 이내만큼 존재하는지를 확인한다.
// 어느 것이라도 바깥에 있으면 보이지 않는 것이고 함수는 false를 리턴한다. 모든 평면의 안쪽에 있따면 구체는 보이는 것으로 true를 리턴한다.
bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius) {
	int i;

	float result = 0.0f;
	for (i = 0; i < 6; i++) {
		result = m_planes[i].x * xCenter + m_planes[i].y * yCenter + m_planes[i].z * zCenter + m_planes[i].w * 1.0f;
		if (result + radius < 0.0f)
		{
			return false;
		}
	}

	return true;
}


// CheckRectangle 함수는 축마다 크기가 달라서 x반경, y반경 z반경을 각각 받는다는 것만 제외하면 CheckCube 함수와 동일하다.
bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize) {
	int i;

	XMFLOAT3 cubeVec;
	float result = 0.0f;

	for (i = 0; i < 6; i++) {
		cubeVec = XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter - zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter - zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter - zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter + zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter - zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter + zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter + zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		cubeVec = XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter + zSize);
		result = m_planes[i].x * cubeVec.x + m_planes[i].y * cubeVec.y + m_planes[i].z * cubeVec.z + m_planes[i].w * 1.0f;
		if (result >= 0.0f) { continue; }

		return false;
	}
	return true;
}