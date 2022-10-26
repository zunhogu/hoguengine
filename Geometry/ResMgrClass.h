#pragma once

class Prefab;
class Mesh;
class TextureClass;
class SoundClass;
class ModelScene;
class Material;
class Animation;

class ResMgrClass
{
private:
	SINGLE(ResMgrClass);
	ResMgrClass();
	~ResMgrClass();

	vector<pair<wstring, wstring>> m_defaultResource;

private:
	map<wstring, Prefab*> m_mapPrefab;
	map<wstring, Mesh*> m_mapMesh;
	map<wstring, Material*> m_mapMaterial;
	map<wstring, TextureClass*> m_mapTex;
	map<wstring, Animation*> m_mapAnimation;
	map<wstring, SoundClass*> m_mapSound;

public:
	bool Initialize();
	void Shutdown();

	Prefab* LoadPrefab(const wstring& _strKey, const wstring& strRelationPath = L"");
	Prefab* FindPrefab(const wstring& _strKey);

	void AddMesh(const wstring& _strKey, Mesh* mesh);
	Mesh* LoadMesh(ID3D11Device* device, const wstring& _strKey, const wstring& strRelationPath = L"");
	Mesh* FindMesh(const wstring& _strKey);

	Material* LoadMaterial(const wstring& _strKey, const wstring& strRelationPath = L"");
	Material* FindMaterial(const wstring& _strKey);

	TextureClass* LoadTexture(ID3D11Device* device, const wstring& _strKey, const wstring& strRelationPath = L"", TEXTURE_TYPE type = TEXTURE_TYPE::DEFAULT);
	TextureClass* FindTexture(const wstring& _strKey);

	Animation* LoadAnimation(const wstring& _strKey, const wstring& strRelationPath = L"");
	Animation* FindAnimation(const wstring& _strKey);

	bool DeleteResource(UINT type, wstring _strKey);

	static wstring CreateRandomID();
};

