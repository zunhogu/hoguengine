#ifndef _PickingClass_ 
#define _PickingClass_

class PickingClass {
private:
	XMVECTOR m_rayDir;
	XMVECTOR m_rayOrigin;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	int pickIndex;
	float pickDepth;

	int m_LastPickingIndex;
	float m_LastPickingDepth;

public:
	PickingClass();
	PickingClass(const PickingClass&);
	~PickingClass();

	bool Initialize(int, int);

	void SetRay(int, int, XMMATRIX, XMMATRIX, XMFLOAT3);
	XMVECTOR GetRayDir() { return m_rayDir; }
	XMVECTOR GetRayOrigin() { return m_rayOrigin; }

	void PickModel(int, float, int);
	void PickModel(int, float);
	void SetLastPickModel(int, float);
	void UnPickModel();

	int GetPickIndex();
	int GetLastPickIndex();
	float GetPickDepth();
	float GetLastPickDepth();

};

#endif