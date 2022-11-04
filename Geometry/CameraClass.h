#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

const float CAMERA_TRANSLATE_SPEED = 0.5f;
const float CAMERA_ROTATION_SPEED = 0.3f;
const float CAMERA_ZOOM_SPEED = 0.05f;
const float MAX_UP_ANGLE = 90.0f;
const float MAX_DOWN_ANGLE = 90.0f;

class CameraClass{
private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;

	// 세계공간 기준의 카메라 좌표계
	XMFLOAT3 m_Right;   // 시야공간 x 축
	XMFLOAT3 m_Up;   // 시야공간 y축
	XMFLOAT3 m_Look;   // 시야공간 z 축

	// 절두체 속성들
	float m_NearZ;   // 절두체 가까운 평면까지의 거리
	float m_FarZ;   // 절두체 먼 평면까지의 거리
	float m_Aspect;   // 종횡비
	float m_FovY;   // 수직 시야각
	float m_NearWindowHeight;   // 절두체 가까운 평면의 크기
	float m_FarWindowHeight;   // 절두체 먼 평면의 크기

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	float m_accumulateAngleX;

	XMFLOAT2 m_screenSize;
public:
	CameraClass();
	CameraClass(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT3 cameraLook, XMFLOAT3 cameraRight, XMFLOAT3 cameraUp);
	CameraClass(const CameraClass&);
	~CameraClass();

	XMFLOAT3 GetPosition() { return m_position; }
	void SetPosition(XMFLOAT3 position) { m_position = position; }
	XMFLOAT3 GetRotation() { return m_rotation; }
	void SetRotation(XMFLOAT3 rotation) { m_rotation = rotation; }
	XMFLOAT3 GetScale() { return m_scale; }
	void SetScale(XMFLOAT3 scale) { m_scale = scale; }

	void SetScreenSize(XMFLOAT2 size) { m_screenSize = size; }

	void SetFrustum(float, float, float, float);

	void Strafe(float);  // 카메라 횡이동 함수
	void Walk(float);   // 카메라 축이동 함수

	void Pitch(float);  // 카메라 x축 회전함수
	void RotateY(float);   // 카메라 y축 회전함수

	void Zoom(float);   // 줌 함수

	void UpdateViewMatrix();
	XMMATRIX GetViewMatrix() { return m_viewMatrix; }

};

#endif