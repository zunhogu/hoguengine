#pragma once
#include "ModelComp.h"
#include "Material.h"

class MaterialComp : public ModelComp
{
private:
	wstring m_materialName;

	Material* m_material;

	//XMFLOAT4 m_ambientColor;
	//XMFLOAT4 m_emissiveColor;
	//XMFLOAT4 m_diffuseColor;
	//XMFLOAT4 m_specularColor;
	//float m_shinness;

	//string m_ambientTextureID;
	//string m_emissiveTextureID;
	//string m_diffuseTextureID;
	//string m_specularTextureID;
	//string m_normalTextureID;

	//wstring m_ambientTexturePath;
	//wstring m_emissiveTexturePath;
	//wstring m_diffuseTexturePath;
	//wstring m_specularTexturePath;
	//wstring m_normalTexturePath;

public:
	MaterialComp();
	MaterialComp(const MaterialComp& material);
	~MaterialComp();

	wstring GetMaterialName() { return m_materialName; }
	void SetMateriaName(wstring name) { m_materialName = name; }

	Material* GetMaterial() { return m_material; }
	void SetMaterial(Material* material) { m_material = material; }

	//XMFLOAT4 GetAmbientColor() { return m_ambientColor; }
	//void SetAmibientColor(XMFLOAT4 color) { m_ambientColor = color; }
	//XMFLOAT4 GetEmissiveColor() { return m_emissiveColor; }
	//void SetEmissiveColor(XMFLOAT4 color) { m_emissiveColor = color; }
	//XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; }
	//void SetDiffuseColor(XMFLOAT4 color) { m_diffuseColor = color; }
	//XMFLOAT4 GetSpecularColor() { return m_specularColor; }
	//void SetSpecularColor(XMFLOAT4 color) { m_specularColor = color; }
	//float GetShinness() { return m_shinness; }
	//void SetShinness(float shinness) { m_shinness = shinness; }

	//string GetAmbientTextureID() { return m_ambientTextureID; }
	//void SetAmibentTextureID(string id) { m_ambientTextureID = id; }
	//string GetEmissiveTextureID() { return m_emissiveTextureID; }
	//void SetEmissiveTextureID(string id) { m_emissiveTextureID = id; }
	//string GetDiffuseTextureID() { return m_diffuseTextureID; }
	//void SetDiffuseTextureID(string id) { m_diffuseTextureID = id; }
	//string GetSpecularTextureID() { return m_specularTextureID; }
	//void SetSpecularTextureID(string id) { m_specularTextureID = id; }
	//string GetNormalTextureID() { return m_normalTextureID; }
	//void SetNormalTextureID(string id) { m_normalTextureID = id; }

	//wstring GetAmbientTexturePath() { return m_ambientTexturePath; }
	//void SetAmibentTexturePath(wstring path) { m_ambientTexturePath = path; }
	//wstring GetEmissiveTexturePath() { return m_emissiveTexturePath; }
	//void SetEmissiveTexturePath(wstring path) { m_emissiveTexturePath = path; }
	//wstring GetDiffuseTexturePath() { return m_diffuseTexturePath; }
	//void SetDiffuseTexturePath(wstring path) { m_diffuseTexturePath = path; }
	//wstring GetSpecularTexturePath() { return m_specularTexturePath; }
	//void SetSpecularTexturePath(wstring path) { m_specularTexturePath = path; }
	//wstring GetNormalTexturePath() { return m_normalTexturePath; }
	//void SetNormalTexturePath(wstring path) { m_normalTexturePath = path; }


	virtual void Render(ModelNode* node);
};

