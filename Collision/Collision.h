#pragma once


#ifdef CREATEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

#include "PickingClass.h" 
#include "FrustumClass.h"

class MYDLL_DECLSPEC CollisionClass
{
private:
	PickingClass* m_Picking;
	FrustumClass* m_Frustum;
private:
	SINGLE(CollisionClass);
public:
	CollisionClass();
	CollisionClass(const CollisionClass&);
	~CollisionClass();
public:
	bool Initialize(int screenWidth, int screenHeight);
	bool InitializePickingClass(int screenWidth, int screenHeight);
	void Shutdown();

	void SetRay(int, int, XMMATRIX, XMMATRIX, XMFLOAT3);
	XMVECTOR GetRayDir() { return m_Picking->GetRayDir(); }
	XMVECTOR GetRayOrigin() { return m_Picking->GetRayOrigin(); }

	void ConstructFrustum(float, XMMATRIX, XMMATRIX);
	bool CheckCube(float, float, float, float);
};

