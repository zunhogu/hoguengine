#include "pch.h"
#include "Collision.h"

CollisionClass::CollisionClass()
{
	m_Picking = 0;
}

CollisionClass::CollisionClass(const CollisionClass&)
{
}

CollisionClass::~CollisionClass()
{
}

bool CollisionClass::Initialize(int screenWidth, int screenHeight)
{
	bool result;
	m_Picking = new PickingClass();
	if (!m_Picking)
	{
		return false;
	}

	m_Frustum = new FrustumClass();
	if (!m_Frustum)
	{
		return false;
	}

	return true;
}

bool CollisionClass::InitializePickingClass(int viewPortScreenWidth, int viewPortScreenHeight)
{
	bool result;

	result = m_Picking->Initialize(viewPortScreenWidth, viewPortScreenHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

void CollisionClass::Shutdown()
{
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_Picking)
	{
		delete m_Picking;
		m_Picking = 0;
	}
}

void CollisionClass::SetRay(int mouseX, int mouseY, XMMATRIX viewMatrix, XMMATRIX projection, XMFLOAT3 rayOrigin)
{
	m_Picking->SetRay(mouseX, mouseY, viewMatrix, projection, rayOrigin);
}

void CollisionClass::ConstructFrustum(float SCREEN_DEPTH, XMMATRIX m_ProjectionMatrix, XMMATRIX m_ViewMatrix)
{
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, m_ProjectionMatrix, m_ViewMatrix);
}

bool CollisionClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	return m_Frustum->CheckCube(xCenter, yCenter, zCenter, radius);
}
