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

	// ������� ������ ī�޶� ��ǥ��
	XMFLOAT3 m_Right;   // �þ߰��� x ��
	XMFLOAT3 m_Up;   // �þ߰��� y��
	XMFLOAT3 m_Look;   // �þ߰��� z ��

	// ����ü �Ӽ���
	float m_NearZ;   // ����ü ����� �������� �Ÿ�
	float m_FarZ;   // ����ü �� �������� �Ÿ�
	float m_Aspect;   // ��Ⱦ��
	float m_FovY;   // ���� �þ߰�
	float m_NearWindowHeight;   // ����ü ����� ����� ũ��
	float m_FarWindowHeight;   // ����ü �� ����� ũ��

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

	void Strafe(float);  // ī�޶� Ⱦ�̵� �Լ�
	void Walk(float);   // ī�޶� ���̵� �Լ�

	void Pitch(float);  // ī�޶� x�� ȸ���Լ�
	void RotateY(float);   // ī�޶� y�� ȸ���Լ�

	void Zoom(float);   // �� �Լ�

	void UpdateViewMatrix();
	XMMATRIX GetViewMatrix() { return m_viewMatrix; }

};

#endif