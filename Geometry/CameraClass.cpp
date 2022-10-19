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

// 카메라 z축(앞뒤)이동 함수
void CameraClass::Strafe(float distance) {
	XMVECTOR s = XMVectorReplicate(distance * CAMERA_TRANSLATE_SPEED);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));   // s*l + p
}

// 카메라 x축(좌우)이동 함수
void CameraClass::Walk(float distance) {
	XMVECTOR s = XMVectorReplicate(distance * CAMERA_TRANSLATE_SPEED);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));   // s*r + p
}

// 카메라 x축벡터 기준으로 회전
void CameraClass::Pitch(float angle) {
	// angle의 값은 radian 값이기 때문에 degree 값으로 바꿔줘서 위아래의 최대/최소 회전각을 설정한다.
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
		// x 축 기준으로 회전시 카메라 상향벡터와, 카메라 시점벡터가 바뀐다.
		XMStoreFloat3(&m_Up, XMVector2TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
	}
}

// 카메라 y축벡터 기준으로 회전
void CameraClass::RotateY(float angle) {
	XMMATRIX R = XMMatrixRotationY(0.15f * angle);

	// y축 기준으로 회전시 카메라의 오른쪽 벡터와, 시점 벡터가 바뀐다.
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

// 줌 함수
void CameraClass::Zoom(float distance) {
	XMVECTOR s = XMVectorReplicate(distance * CAMERA_ZOOM_SPEED);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));
}

// 뷰행렬을 만든다.
void CameraClass::UpdateViewMatrix() {
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_position);

	// 시야 행렬을 만들때는 오른쪽 벡터, 상향 벡터, 시점 벡터를 정규화한다.
	// 그 이유는 회전이 여러번 반복되다보면 수치 오차가 누적되기 때문이다.
	// 이렇게 누적 오차가 쌓이면 카메라 좌표계는 특정 방향으로 기울어진 좌표계가 되어서 문제가 발생한다.

	// 일단 시점 벡터만 정규화한다.
	L = XMVector3Normalize(L);

	// 보정된 새 상향 벡터를 계산한다. 그러기 위해 시점벡터와 오른쪽 벡터의 외적을 정규화한다.
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// 보정된 색 오른쪽 벡터를 계산한다. 이미 정규직교이므로 외적을 다시 정규화 할 필요는없다.
	R = XMVector3Cross(U, L);

	// 시야 행렬의 성분들을 채운다.
	XMFLOAT4X4 viewMatrix;

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	// 새로 갱신된 벡터들로 카메라 벡터를 다시 갱신한다.
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