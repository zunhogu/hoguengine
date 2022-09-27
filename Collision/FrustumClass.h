#ifndef _FRUSTUMCLASS_H_ 
#define _FRUSTUMCLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

// FrustumClass�� �ƹ��� �ʱ�ȭ�� ������ �Լ��� �䱸���� �ʴ´�.
// ī�޶� �� ���׸��� ���ڷ� �� �����Ӹ��� ConstructFrustum �Լ��� ȣ���Ѵ�.
// ConstructFrustum �Լ��� ���ŵ� ī�޶� ��ġ�� ���ʷ� m_planes ������ �̿��Ͽ� �þ� ���������� 6���� ����� ����ϰ� �����Ѵ�.
// ���⼭ ���̳� ������ü, ����ü, ��ü ���� �þ� �������� �ȿ� �ִ��� �׷��� ������ üũ�ϴ� 4���� �Լ��� �����. 
class FrustumClass {
private:
	// ���� �ѷ��� 6���� �� (����ü)
	XMFLOAT4 m_planes[6];
public:
	FrustumClass();
	FrustumClass(const FrustumClass&);
	~FrustumClass();

	void ConstructFrustum(float, XMMATRIX, XMMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);
};


#endif