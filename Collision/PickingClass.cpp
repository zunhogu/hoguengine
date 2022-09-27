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

	// ȭ��������� ���콺 ��ǥ�� �̿��ؼ� ndc ������ ��ǥ�� ����Ѵ�.
	ndcX = (((2.0f * m_mouseX) / m_screenWidth) - 1.0f);
	ndcY = (((-2.0f * m_mouseY) / m_screenHeight) + 1.0f);

	XMFLOAT4X4 projection4X4;
	XMStoreFloat4x4(&projection4X4, projection);

	// ��������� ��Ⱦ�� �����Ÿ��� �̿��ؼ� ndc���� ��ǥ�� ������� ��ǥ�� �ٲ��ش�. 
	viewX = (ndcX / projection4X4(0, 0));
	viewY = (ndcY / projection4X4(1, 1));

	m_rayDir = XMVectorSet(viewX, viewY, 1.0f, 0.0f);

	// ���̴� ���� ������� ��ǥ�̹Ƿ� �浹�˻縦 ���� ����������� �Ű�����Ѵ�.
	// �̶� ������� ������� ���� ������ȯ�����ָ� �ȴ�.
	//XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	//XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

	//XMMATRIX toLocal = XMMatrixMultiply(invViewMatrix, invWorldMatrix);

	// ������� ��ǥ�� ������� ������� �̿��ؼ� ���� ��ǥ�� �ٲ��ش�.
	//m_rayDir = XMVector3TransformNormal(XMVectorSet(viewX, viewY, 1.0f, 0.0f), invViewMatrix);
	//XMStoreFloat3(&m_rayOrigin, XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), toLocal));

	// ������ ǥ��ȭ���ش�.
	//m_rayDir = XMVector3Normalize(m_rayDir);

	return;
}


void PickingClass::PickModel(int index, float depth, int pickingCount) {
	// pickingCount�� 1�̸� ��ġ�� ������Ʈ�� ����
	if (pickingCount == 1) {
		pickIndex = index;
		pickDepth = depth;
	}
	// pickingCount�� 2�̻��̸� ��ġ�� ������Ʈ�� ����
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