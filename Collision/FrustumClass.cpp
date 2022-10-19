#include "pch.h"
#include "FrustumClass.h"

FrustumClass::FrustumClass() {}

FrustumClass::FrustumClass(const FrustumClass& other) {}

FrustumClass::~FrustumClass() {}

// GraphicsClass�� ���� ConstructFrustum �Լ��� �� �����Ӹ��� ȣ��ȴ�.
// GraphicsClass������ ȭ���� depth�� ���� ���, �׸��� �� ����� �����Ѵ�.
// �� �Էµ��� �̿��Ͽ� ���� �������� �þ� �������� ����� �����. 
// �� �þ� �������� ��ķ� �þ� ���������� �����ϴ� ���� ���� ����� ����� ����.
void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix) {
	float zMinimum, r;
	XMMATRIX matrix;
	XMFLOAT4X4  matrix4X4;
	XMFLOAT4X4 projectionMatrix4X4;

	// ����ü�� �ּ� ���̸� ����Ͽ� ��������� �������ش�.
	// �׷��� ���ؼ��� ��������� ���Ҹ� �����������ϴµ�
	// XMMATRIX�� ��� ���ҿ� �����ϴ� �� �� ����. ���� XMFLOAT4X4�� ����� �ִ� ���� ���� �Ͱ���.
	// �̶� ����ϴ� �Լ��� XMStoreFloat�Լ��� �� �ݴ�� XMStoreFloat �Լ��� �ִ�.
	XMStoreFloat4x4(&projectionMatrix4X4, projectionMatrix);

	zMinimum = -projectionMatrix4X4._43 / projectionMatrix4X4._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix4X4._33 = r;
	projectionMatrix4X4._43 = -r * zMinimum;

	projectionMatrix = XMLoadFloat4x4(&projectionMatrix4X4);

	// �Է¹��� �������, ������� ���� �þ� �������� ����� �����Ѵ�. 
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMStoreFloat4x4(&matrix4X4, matrix);


	// �������� ����� ���� ����ü 6���� ������ش�.
	XMVECTOR resultPlane;
	// �� ���
	m_planes[0].x = matrix4X4._14 + matrix4X4._13;
	m_planes[0].y = matrix4X4._24 + matrix4X4._23;
	m_planes[0].z = matrix4X4._34 + matrix4X4._33;
	m_planes[0].w = matrix4X4._44 + matrix4X4._43;
	resultPlane = XMLoadFloat4(&m_planes[0]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// �����
	m_planes[1].x = matrix4X4._14 - matrix4X4._13;
	m_planes[1].y = matrix4X4._24 - matrix4X4._23;
	m_planes[1].z = matrix4X4._34 - matrix4X4._33;
	m_planes[1].w = matrix4X4._44 - matrix4X4._43;
	resultPlane = XMLoadFloat4(&m_planes[1]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// �����
	m_planes[2].x = matrix4X4._14 + matrix4X4._11;
	m_planes[2].y = matrix4X4._24 + matrix4X4._21;
	m_planes[2].z = matrix4X4._34 + matrix4X4._31;
	m_planes[2].w = matrix4X4._44 + matrix4X4._41;
	resultPlane = XMLoadFloat4(&m_planes[2]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// �����
	m_planes[3].x = matrix4X4._14 - matrix4X4._11;
	m_planes[3].y = matrix4X4._24 - matrix4X4._21;
	m_planes[3].z = matrix4X4._34 - matrix4X4._31;
	m_planes[3].w = matrix4X4._44 - matrix4X4._41;
	resultPlane = XMLoadFloat4(&m_planes[3]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// �����
	m_planes[4].x = matrix4X4._14 - matrix4X4._12;
	m_planes[4].y = matrix4X4._24 - matrix4X4._22;
	m_planes[4].z = matrix4X4._34 - matrix4X4._32;
	m_planes[4].w = matrix4X4._44 - matrix4X4._42;
	resultPlane = XMLoadFloat4(&m_planes[4]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	// �����
	m_planes[5].x = matrix4X4._14 + matrix4X4._12;
	m_planes[5].y = matrix4X4._24 + matrix4X4._22;
	m_planes[5].z = matrix4X4._34 + matrix4X4._32;
	m_planes[5].w = matrix4X4._44 + matrix4X4._42;
	resultPlane = XMLoadFloat4(&m_planes[5]);
	XMStoreFloat4(&m_planes[0], XMPlaneNormalize(resultPlane));

	return;
}

// CheckPoint �Լ��� �ϳ��� ���� �þ� �������� ���ο� �ִ��� Ȯ���ϴ� �Լ���.
// ���ο� �ִ� �� Ȯ���� ���� 3D������ ������ ���Ѵ�. ����� DirectMath ���� ������ �̿��ؼ� ���Ѵ�.
// �ùٸ� ȯ�濡�� �� �Լ��� ����ϸ� �ٸ� �װ��� üũ�Լ��� ���� ���� ȿ�����̴�.
// �� �Լ��� ���� ��ǥ�� �Է����� �ް� ��� ���� ���� ��� �ȿ� ���� �ִ��� Ȯ���Ѵ�. ���ϰ��� ���Կ���
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

// CheckCube �Լ��� ������ü�� 8���� ������ �� ������̶� �þ� �������� ���� �ִ��� Ȯ���Ѵ�.
// �Է����δ� ����ü�� ������ �ݰ游 ������ �Ǹ� �̸� �̿��Ͽ� �������� ������ ��ġ�� ����� �� �ִ�.
// ���� �������� �ϳ��� �������� ���ο� �ִٸ� true�� �����ϰ� �׷��� �ʴٸ� false�� �����Ѵ�. 
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

// CheckSphere �Լ��� ���� �߽��� �þ� ���������� ���� ��鿡 �ݰ� �̳���ŭ �����ϴ����� Ȯ���Ѵ�.
// ��� ���̶� �ٱ��� ������ ������ �ʴ� ���̰� �Լ��� false�� �����Ѵ�. ��� ����� ���ʿ� �ֵ��� ��ü�� ���̴� ������ true�� �����Ѵ�.
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


// CheckRectangle �Լ��� �ึ�� ũ�Ⱑ �޶� x�ݰ�, y�ݰ� z�ݰ��� ���� �޴´ٴ� �͸� �����ϸ� CheckCube �Լ��� �����ϴ�.
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