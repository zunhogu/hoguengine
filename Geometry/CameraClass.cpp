#include "pch.h"
#include "CameraClass.h"

CameraClass::CameraClass() {
}

CameraClass::CameraClass(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT3 cameraLook, XMFLOAT3 cameraRight, XMFLOAT3 cameraUp)
{
	m_accumulateAngleX = 0;
	m_position = position;
	m_rotation = rotation;
	m_scale = scale;

	m_Look = cameraLook;
	m_Right = cameraRight;
	m_Up = cameraUp;
}

CameraClass::CameraClass(const CameraClass& other) {}


CameraClass::~CameraClass() {}

void CameraClass::SetFrustum(float fovY, float aspect, float zn, float zf) {
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;
	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);
}

// ī�޶� z��(�յ�)�̵� �Լ�
void CameraClass::Strafe(float distance) {
	XMVECTOR s = XMVectorReplicate(distance * CAMERA_TRANSLATE_SPEED);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));   // s*l + p
}

// ī�޶� x��(�¿�)�̵� �Լ�
void CameraClass::Walk(float distance) {
	XMVECTOR s = XMVectorReplicate(distance * CAMERA_TRANSLATE_SPEED);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));   // s*r + p
}

// ī�޶� x�຤�� �������� ȸ��
void CameraClass::Pitch(float angle) {
	// angle�� ���� radian ���̱� ������ degree ������ �ٲ��༭ ���Ʒ��� �ִ�/�ּ� ȸ������ �����Ѵ�.
	if (angle < 0.0f) {
		m_accumulateAngleX += (angle * (180.0f / XM_PI)) * CAMERA_ROTATION_SPEED;
		if (m_accumulateAngleX < -MAX_UP_ANGLE) { m_accumulateAngleX = -MAX_UP_ANGLE; }
	}
	if (angle > 0.0f) {
		m_accumulateAngleX += (angle * (180.0f / XM_PI)) * CAMERA_ROTATION_SPEED;
		if (m_accumulateAngleX > MAX_DOWN_ANGLE) { m_accumulateAngleX = MAX_DOWN_ANGLE; }
	}
	if (m_accumulateAngleX < MAX_DOWN_ANGLE && m_accumulateAngleX > -MAX_UP_ANGLE) {
		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), CAMERA_ROTATION_SPEED * angle);
		// x �� �������� ȸ���� ī�޶� ���⺤�Ϳ�, ī�޶� �������Ͱ� �ٲ��.
		XMStoreFloat3(&m_Up, XMVector2TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
	}
}

// ī�޶� y�຤�� �������� ȸ��
void CameraClass::RotateY(float angle) {
	XMMATRIX R = XMMatrixRotationY(0.15f * angle);

	// y�� �������� ȸ���� ī�޶��� ������ ���Ϳ�, ���� ���Ͱ� �ٲ��.
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

// �� �Լ�
void CameraClass::Zoom(float distance) {
	XMVECTOR s = XMVectorReplicate(distance * CAMERA_ZOOM_SPEED);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));
}

// ������� �����.
void CameraClass::UpdateViewMatrix() {
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_position);

	// �þ� ����� ���鶧�� ������ ����, ���� ����, ���� ���͸� ����ȭ�Ѵ�.
	// �� ������ ȸ���� ������ �ݺ��Ǵٺ��� ��ġ ������ �����Ǳ� �����̴�.
	// �̷��� ���� ������ ���̸� ī�޶� ��ǥ��� Ư�� �������� ������ ��ǥ�谡 �Ǿ ������ �߻��Ѵ�.

	// �ϴ� ���� ���͸� ����ȭ�Ѵ�.
	L = XMVector3Normalize(L);

	// ������ �� ���� ���͸� ����Ѵ�. �׷��� ���� �������Ϳ� ������ ������ ������ ����ȭ�Ѵ�.
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// ������ �� ������ ���͸� ����Ѵ�. �̹� ���������̹Ƿ� ������ �ٽ� ����ȭ �� �ʿ�¾���.
	R = XMVector3Cross(U, L);

	// �þ� ����� ���е��� ä���.
	XMFLOAT4X4 viewMatrix;

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	// ���� ���ŵ� ���͵�� ī�޶� ���͸� �ٽ� �����Ѵ�.
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	viewMatrix(0, 0) = m_Right.x;
	viewMatrix(1, 0) = m_Right.y;
	viewMatrix(2, 0) = m_Right.z;
	viewMatrix(3, 0) = x;

	viewMatrix(0, 1) = m_Up.x;
	viewMatrix(1, 1) = m_Up.y;
	viewMatrix(2, 1) = m_Up.z;
	viewMatrix(3, 1) = y;

	viewMatrix(0, 2) = m_Look.x;
	viewMatrix(1, 2) = m_Look.y;
	viewMatrix(2, 2) = m_Look.z;
	viewMatrix(3, 2) = z;

	viewMatrix(0, 3) = 0.0f;
	viewMatrix(1, 3) = 0.0f;
	viewMatrix(2, 3) = 0.0f;
	viewMatrix(3, 3) = 1.0f;

	m_viewMatrix = XMLoadFloat4x4(&viewMatrix);


	return;
}