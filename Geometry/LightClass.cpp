#include "pch.h"
#include "lightclass.h"

LightClass::LightClass() {}

LightClass::LightClass(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor)
{
	m_position = position;
	m_rotation = rotation;
	m_scale = scale;

	m_diffuseColor = diffuseColor;
	m_ambientColor = ambientColor;
}

LightClass::LightClass(const LightClass& other) {}

LightClass::~LightClass() {}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha) {
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha) {
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

XMFLOAT4 LightClass::GetDiffuseColor() {
	return m_diffuseColor;
}


XMFLOAT4 LightClass::GetAmbientColor() {
	return m_ambientColor;
}

