#include "pch.h"
#include "Material.h"
#include "Core.h"
#include "ResMgrClass.h"

Material::Material()
{
}

Material::Material(const Material&)
{
}

Material::~Material()
{
}

bool Material::Initialize(const wstring& _strFilePath)
{
	bool result = true;
	result = LoadMaterial(_strFilePath);
	if (!result)
		return false;

	return result;
}

bool Material::LoadMaterial(const wstring& _strFilePath)
{
	ifstream fin;

	m_materialName = Core::GetFileName(_strFilePath);

	// 파일을 연다.
	fin.open(_strFilePath.c_str());
	if (fin.fail())
		return false;

	string buffer;

	// Ambient Color
	fin >> buffer;  // Ambient Color string
	fin >> buffer;  // :
	fin >> m_ambientColor.x >> m_ambientColor.y >> m_ambientColor.z >> m_ambientColor.w;

	// Emissive Color
	fin >> buffer;  // Emissive Color string
	fin >> buffer;  // :
	fin >> m_emissiveColor.x >> m_emissiveColor.y >> m_emissiveColor.z >> m_emissiveColor.w;

	// Diffuse Color
	fin >> buffer;  // Diffuse Color string
	fin >> buffer;  // :
	fin >> m_diffuseColor.x >> m_diffuseColor.y >> m_diffuseColor.z >> m_diffuseColor.w;

	// Specular Color
	fin >> buffer;  // Specular Color string
	fin >> buffer;  // :
	fin >> m_specularColor.x >> m_specularColor.y >> m_specularColor.z >> m_specularColor.w;

	// Shinness
	fin >> buffer;  // Specular Color string
	fin >> buffer;  // :
	fin >> m_shinness;

	// Ambient Texture
	fin >> buffer;  // Emissive Color string
	fin >> buffer;  // :
	fin >> buffer;  // relativePath
	wstring relativePath = Core::ConvCharToWchar((char*)buffer.c_str());
	if (relativePath != L"NONE")
	{
		relativePath = L"contents\\assets\\" + relativePath;
		ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(relativePath), relativePath);
		m_ambientTextureID = Core::ConvWcharTochar(Core::GetFileName(relativePath));
	}
	else
		m_ambientTextureID = "NONE";

	// Emissive Texture
	fin >> buffer;  // Emissive Color string
	fin >> buffer;  // :
	fin >> buffer;  // relativePath
	relativePath = Core::ConvCharToWchar((char*)buffer.c_str());
	if (relativePath != L"NONE")
	{
		relativePath = L"contents\\assets\\" + relativePath;
		ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(relativePath), relativePath);
		m_emissiveTextureID = Core::ConvWcharTochar(Core::GetFileName(relativePath));
	}
	else
		m_emissiveTextureID = "NONE";
	
	// Diffuse Texture
	fin >> buffer;  // Emissive Color string
	fin >> buffer;  // :
	fin >> buffer;  // relativePath
	relativePath = Core::ConvCharToWchar((char*)buffer.c_str());
	if (relativePath != L"NONE")
	{
		relativePath = L"contents\\assets\\" + relativePath;
		ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(relativePath), relativePath);
		m_diffuseTextureID = Core::ConvWcharTochar(Core::GetFileName(relativePath));
	}
	else
		m_diffuseTextureID = "NONE";

	// Specular Texture
	fin >> buffer;  // Emissive Color string
	fin >> buffer;  // :
	fin >> buffer;  // relativePath
	relativePath = Core::ConvCharToWchar((char*)buffer.c_str());
	if (relativePath != L"NONE")
	{
		relativePath = L"contents\\assets\\" + relativePath;
		ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(relativePath), relativePath);
		m_specularTextureID = Core::ConvWcharTochar(Core::GetFileName(relativePath));
	}
	else
		m_specularTextureID = "NONE";

	// NormalMap Texture
	fin >> buffer;  // NormalMap Texture string
	fin >> buffer;  // :
	fin >> buffer;  // relativePath
	relativePath = Core::ConvCharToWchar((char*)buffer.c_str());
	if (relativePath != L"NONE")
	{
		relativePath = L"contents\\assets\\" + relativePath;
		ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(relativePath), relativePath);
		m_normalTextureID = Core::ConvWcharTochar(Core::GetFileName(relativePath));
	}
	else
		m_normalTextureID = "NONE";

	fin.close();
}
