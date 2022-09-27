#include "pch.h"
#include "PickingClass.h"

PickingClass::PickingClass() {}

PickingClass::PickingClass(const PickingClass&) {}

PickingClass::~PickingClass() {}

bool PickingClass::Initialize(int screenWidth, int screenHeight) {

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	pickIndex = -1;
	pickDepth = INFINITY;

	return true;
}

void PickingClass::SetRay(int mouseX, int mouseY, XMMATRIX viewMatrix, XMMATRIX projection, XMFLOAT3 rayOrigin) {
	float ndcX, ndcY;
	float viewX, viewY;

	m_mouseX = mouseX;
	m_mouseY = mouseY;

	m_rayOrigin = XMVectorSet(rayOrigin.x, rayOrigin.y, rayOrigin.z, 1.0f);

	// 화면공간안의 마우스 좌표를 이용해서 ndc 공간의 좌표를 계산한다.
	ndcX = (((2.0f * m_mouseX) / m_screenWidth) - 1.0f);
	ndcY = (((-2.0f * m_mouseY) / m_screenHeight) + 1.0f);

	XMFLOAT4X4 projection4X4;
	XMStoreFloat4x4(&projection4X4, projection);

	// 투영행렬의 종횡비나 초점거리를 이용해서 ndc공간 좌표를 뷰공간의 좌표로 바꿔준다. 
	viewX = (ndcX / projection4X4(0, 0));
	viewY = (ndcY / projection4X4(1, 1));

	m_rayDir = XMVectorSet(viewX, viewY, 1.0f, 0.0f);

	// 레이는 현재 뷰공간의 좌표이므로 충돌검사를 위해 월드공간으로 옮겨줘야한다.
	// 이때 뷰행렬의 역행렬을 곱해 공간변환시켜주면 된다.
	//XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	//XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

	//XMMATRIX toLocal = XMMatrixMultiply(invViewMatrix, invWorldMatrix);

	// 뷰공간의 좌표를 뷰공간의 역행렬을 이용해서 월드 좌표로 바꿔준다.
	//m_rayDir = XMVector3TransformNormal(XMVectorSet(viewX, viewY, 1.0f, 0.0f), invViewMatrix);
	//XMStoreFloat3(&m_rayOrigin, XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), toLocal));

	// 직선을 표준화해준다.
	//m_rayDir = XMVector3Normalize(m_rayDir);

	return;
}


void PickingClass::PickModel(int index, float depth, int pickingCount) {
	// pickingCount가 1이면 곂치는 오브젝트가 없음
	if (pickingCount == 1) {
		pickIndex = index;
		pickDepth = depth;
	}
	// pickingCount가 2이상이면 곂치는 오븍제트가 있음
	else if (pickingCount >= 2) {
		if (pickDepth > depth) {
			pickIndex = index;
			pickDepth = depth;
		}
	}
	return;
}
void PickingClass::PickModel(int index, float depth) {
	pickIndex = index;
	pickDepth = depth;
}

void PickingClass::SetLastPickModel(int index, float depth) {
	m_LastPickingIndex = index;
	m_LastPickingDepth = depth;
	return;
}

void PickingClass::UnPickModel() {
	pickIndex = -1;
	pickDepth = INFINITY;
	return;
}

int PickingClass::GetPickIndex() {
	return pickIndex;
}

int PickingClass::GetLastPickIndex() {
	return m_LastPickingIndex;
}

float PickingClass::GetPickDepth() {
	return pickDepth;
}

float PickingClass::GetLastPickDepth() {
	return m_LastPickingDepth;
}