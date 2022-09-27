#pragma once
#include "ResourceClass.h"

class Material : public ResourceClass
{
private:
	wstring m_materialName;

	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_emissiveColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	float m_shinness;

	string m_ambientTextureID;
	string m_emissiveTextureID;
	string m_diffuseTextureID;
	string m_specularTextureID;
	string m_normalTextureID;
public:
	Material();
	Material(const Material&);
	~Material();

	bool Initialize(const wstring& _strFilePath);
	bool LoadMaterial(const wstring& _strFilePath);

	wstring GetMaterialName() { return m_materialName; }
	void SetMateriaName(wstring name) { m_materialName = name; }

	XMFLOAT4 GetAmbientColor() { return m_ambientColor; }
	void SetAmibientColor(XMFLOAT4 color) { m_ambientColor = color; }
	XMFLOAT4 GetEmissiveColor() { return m_emissiveColor; }
	void SetEmissiveColor(XMFLOAT4 color) { m_emissiveColor = color; }
	XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; }
	void SetDiffuseColor(XMFLOAT4 color) { m_diffuseColor = color; }
	XMFLOAT4 GetSpecularColor() { return m_specularColor; }
	void SetSpecularColor(XMFLOAT4 color) { m_specularColor = color; }
	float GetShinness() { return m_shinness; }
	void SetShinness(float shinness) { m_shinness = shinness; }

	string GetAmbientTextureID() { return m_ambientTextureID; }
	void SetAmibentTextureID(string id) { m_ambientTextureID = id; }
	string GetEmissiveTextureID() { return m_emissiveTextureID; }
	void SetEmissiveTextureID(string id) { m_emissiveTextureID = id; }
	string GetDiffuseTextureID() { return m_diffuseTextureID; }
	void SetDiffuseTextureID(string id) { m_diffuseTextureID = id; }
	string GetSpecularTextureID() { return m_specularTextureID; }
	void SetSpecularTextureID(string id) { m_specularTextureID = id; }
	string GetNormalTextureID() { return m_normalTextureID; }
	void SetNormalTextureID(string id) { m_normalTextureID = id; }
};

