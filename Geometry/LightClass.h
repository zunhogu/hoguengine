#ifndef _LIGHTCLASS_H_ 
#define _LIGHTCLASS_H_


class LightClass{
private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;

	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_ambientColor;
public:
	LightClass();
	LightClass(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor);
	LightClass(const LightClass&);
	~LightClass();

	XMFLOAT3 GetPosition() { return m_position; }
	void SetPosition(XMFLOAT3 position) { m_position = position; }
	XMFLOAT3 GetRotation() { return m_rotation; }
	void SetRotation(XMFLOAT3 rotation) { m_rotation = rotation; }
	XMFLOAT3 GetScale() { return m_scale; }
	void SetScale(XMFLOAT3 scale) { m_scale = scale; }

	void SetDiffuseColor(float, float, float, float);
	void SetAmbientColor(float, float, float, float);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetAmbientColor();
};

#endif