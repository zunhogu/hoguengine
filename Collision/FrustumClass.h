#ifndef _FRUSTUMCLASS_H_ 
#define _FRUSTUMCLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

// FrustumClass는 아무런 초기화나 마무리 함수를 요구하지 않는다.
// 카메라가 한 번그리고 난뒤로 매 프레임마다 ConstructFrustum 함수를 호출한다.
// ConstructFrustum 함수는 갱신된 카메라 위치를 기초로 m_planes 변수를 이용하여 시야 프리스텀의 6개의 평면을 계산하고 저장한다.
// 여기서 점이나 정육면체, 육면체, 구체 등이 시야 프리스텀 안에 있는지 그렇지 않은지 체크하는 4개의 함수를 만든다. 
class FrustumClass {
private:
	// 모델을 둘러쌀 6개의 면 (절두체)
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