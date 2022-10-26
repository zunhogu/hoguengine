#include "pch.h"
#include "ResMgrClass.h"

#include "Core.h"
#include "../FileSystem/PathMgr.h"
#include "Prefab.h"
#include "Mesh.h"
#include "Material.h"
#include "TextureClass.h"
#include "HeightMapTexture.h"
#include "SoundClass.h"
#include "Animation.h"

ResMgrClass::ResMgrClass()
{
}

ResMgrClass::~ResMgrClass()
{
}

bool ResMgrClass::Initialize()
{

	// 기본적인 Resource를 로드해준다. 이 리소스는 프로그램 종료시까지 절대 지워지지 않는다.
	m_defaultResource.push_back(make_pair(L"defaultTexture", L"default\\texture\\id_white_tex.dds"));
	ResMgrClass::GetInst()->LoadTexture(GraphicsClass::GetInst()->GetDevice(), m_defaultResource[m_defaultResource.size()-1].first, m_defaultResource[m_defaultResource.size() - 1].second);
	m_defaultResource.push_back(make_pair(L"id_black_tex.dds", L"default\\texture\\id_black_tex.dds"));
	ResMgrClass::GetInst()->LoadTexture(GraphicsClass::GetInst()->GetDevice(), m_defaultResource[m_defaultResource.size() - 1].first, m_defaultResource[m_defaultResource.size() - 1].second);
	
	m_defaultResource.push_back(make_pair(L"NONE", L""));
	ResMgrClass::GetInst()->LoadTexture(GraphicsClass::GetInst()->GetDevice(), m_defaultResource[m_defaultResource.size() - 1].first, m_defaultResource[m_defaultResource.size() - 1].second);
	
	return true;
}

void ResMgrClass::Shutdown()
{
	map<wstring, SoundClass*>::iterator iterSound = m_mapSound.begin();
	for (; iterSound != m_mapSound.end(); iterSound++)
	{
		delete iterSound->second;
	}

	map<wstring, TextureClass*>::iterator iterTex = m_mapTex.begin();
	for (; iterTex != m_mapTex.end(); iterTex++)
	{
		delete iterTex->second;
	}

	map<wstring, Mesh*>::iterator iterModel = m_mapMesh.begin();
	for (; iterModel != m_mapMesh.end(); iterModel++)
	{
		delete iterModel->second;
	}
}

Prefab* ResMgrClass::LoadPrefab(const wstring& _strKey, const wstring& strRelationPath)
{
	// 만약 동일한 리소스가 있다면?
	Prefab* pPrefab = FindPrefab(_strKey);
	if (pPrefab != nullptr)
	{
		return pPrefab;
	}

	wstring strFilePath = PathMgr::GetInst()->GetContentPath();
	strFilePath += strRelationPath;

	pPrefab = new Prefab;
	pPrefab->Initialize(strFilePath);

	pPrefab->SetKey(_strKey);
	pPrefab->SetRelativePath(strFilePath);

	// 맵에 넣음 
	m_mapPrefab.insert(make_pair(_strKey, pPrefab));

	return pPrefab;
}

Prefab* ResMgrClass::FindPrefab(const wstring& _strKey)
{
	map<wstring, Prefab*>::iterator iter = m_mapPrefab.find(_strKey);
	if (iter == m_mapPrefab.end())
	{
		return nullptr;
	}
	return iter->second;
}

void ResMgrClass::AddMesh(const wstring& _strKey, Mesh* mesh)
{
	if (mesh != nullptr)
		m_mapMesh.insert(make_pair(_strKey, mesh));
}

Mesh* ResMgrClass::LoadMesh(ID3D11Device* device, const wstring& _strKey, const wstring& strRelationPath)
{
	// 만약 동일한 리소스가 있다면?
	Mesh* pMesh = FindMesh(_strKey);
	if (pMesh != nullptr)
	{
		return pMesh;
	}

	wstring strFilePath = PathMgr::GetInst()->GetContentPath();
	strFilePath += strRelationPath;

	pMesh = new Mesh;
	pMesh->Initialize(device, strFilePath);

	pMesh->SetKey(_strKey);
	pMesh->SetRelativePath(strFilePath);

	// 맵에 넣음 
	m_mapMesh.insert(make_pair(_strKey, pMesh));

	return pMesh;
}

Mesh* ResMgrClass::FindMesh(const wstring& _strKey)
{
	map<wstring, Mesh*>::iterator iter = m_mapMesh.find(_strKey);
	if (iter == m_mapMesh.end())
	{
		return nullptr;
	}
	return iter->second;
}

Material* ResMgrClass::LoadMaterial(const wstring& _strKey, const wstring& strRelationPath)
{
	// 만약 동일한 리소스가 있다면?
	Material* pMaterial = FindMaterial(_strKey);
	if (pMaterial != nullptr)
	{
		return pMaterial;
	}

	wstring strFilePath = PathMgr::GetInst()->GetContentPath();
	strFilePath += strRelationPath;

	pMaterial = new Material;
	pMaterial->Initialize(strFilePath);

	pMaterial->SetKey(_strKey);
	pMaterial->SetRelativePath(strFilePath);

	// 맵에 넣음 
	m_mapMaterial.insert(make_pair(_strKey, pMaterial));

	return pMaterial;
}

Material* ResMgrClass::FindMaterial(const wstring& _strKey)
{
	map<wstring, Material*>::iterator iter = m_mapMaterial.find(_strKey);
	if (iter == m_mapMaterial.end())
	{
		return nullptr;
	}
	return iter->second;
}

TextureClass* ResMgrClass::LoadTexture(ID3D11Device* device, const wstring& _strKey, const wstring& strRelationPath, TEXTURE_TYPE type)
{
	// 만약 동일한 리소스가 있다면?
	TextureClass* pTex = FindTexture(_strKey);
	if (pTex != nullptr)
		return pTex;
	
	wstring strFilePath = PathMgr::GetInst()->GetContentPath();
	strFilePath += strRelationPath;

	// 텍스처 생성

	switch (type)
	{
	case TEXTURE_TYPE::DEFAULT:
		pTex = new TextureClass;
		break;
	case TEXTURE_TYPE::HEIGHTMAP:
		pTex = new HeightMapTexture;
		break;
	case TEXTURE_TYPE::END:
		break;
	default:
		break;
	}

	pTex->Initialize(strFilePath);
	pTex->SetKey(_strKey);
	pTex->SetRelativePath(strFilePath);

	// 맵에 넣음 
	m_mapTex.insert(make_pair(_strKey, pTex));

	return pTex;
}

TextureClass* ResMgrClass::FindTexture(const wstring& _strKey)
{
	map<wstring, TextureClass*>::iterator iter = m_mapTex.find(_strKey);
	if (iter == m_mapTex.end())
	{
		return nullptr;
	}
	return iter->second;
}

Animation* ResMgrClass::LoadAnimation(const wstring& _strKey, const wstring& strRelationPath)
{
	// 만약 동일한 리소스가 있다면?
	Animation* pAnim = FindAnimation(_strKey);
	if (pAnim != nullptr)
		return pAnim;

	wstring strFilePath = PathMgr::GetInst()->GetContentPath();
	strFilePath += strRelationPath;

	// 기본 텍스처 초기화
	pAnim = new Animation;
	pAnim->Initialize(strFilePath);

	pAnim->SetKey(_strKey);
	pAnim->SetRelativePath(strFilePath);

	// 맵에 넣음 
	m_mapAnimation.insert(make_pair(_strKey, pAnim));

	return pAnim;
}

Animation* ResMgrClass::FindAnimation(const wstring& _strKey)
{
	return nullptr;
}

bool ResMgrClass::DeleteResource(UINT type, wstring _strKey)
{
	for (int i = 0; i < m_defaultResource.size(); i++)
	{
		if (m_defaultResource[i].first == _strKey)
			return false;
	}

	RESOURCE_TYPE rType = (RESOURCE_TYPE)type;

	// 삭제 시키면 안되는 텍스처들
	wstring dontDelete[] = {L"id_default_tex"};

	switch (rType)
	{
		case RESOURCE_TYPE::MODEL:
		{
			map<wstring, Mesh*>::iterator iterFbx = m_mapMesh.find(_strKey);
			if (iterFbx != m_mapMesh.end())
			{
				if (iterFbx->first == _strKey)
				{
					m_mapMesh.erase(iterFbx);
					return true;
				}
			}
		}
		break;
		case RESOURCE_TYPE::TEXTURE:
		{
			map<wstring, TextureClass*>::iterator iterTex = m_mapTex.find(_strKey);
			if (iterTex != m_mapTex.end())
			{
				if (iterTex->first == _strKey)
				{
					m_mapTex.erase(iterTex);
					return true;
				}
			}
		}
		break;	
		case RESOURCE_TYPE::SOUND:
		{
			map<wstring, SoundClass*>::iterator iterSound = m_mapSound.find(_strKey);
			if (iterSound != m_mapSound.end())
			{
				if (iterSound->first == _strKey)
				{
					m_mapSound.erase(iterSound);
					return true;
				}
			}
		}
		break;
	}

	return false;
}

wstring ResMgrClass::CreateRandomID()
{
	// ID는 32bit 문자(8글자)로 구성
	int id_length = 8;
	wstring result = L"";

	static const wchar_t alphanum[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < id_length; ++i) 
	{
		int idx = rand() % 62;
		result += alphanum[idx];
	}

	return result;
}
