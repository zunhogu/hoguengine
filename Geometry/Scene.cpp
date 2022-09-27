#include "pch.h"
#include "Scene.h"
#include "LightClass.h"
#include "CameraClass.h"
#include "SceneMgrClass.h"
#include "ModelNode.h"
	
Scene::Scene()
{
}

Scene::Scene(const Scene&)
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(int screenWidht, int screenHeight, HWND hwnd)
{
	return false;
}

void Scene::Shutdown()
{

}

void Scene::Render()
{
}


void Scene::SaveObjects(FILE* _pFile)
{
	//size_t size;

	//// 1. 씬 번호 Save
	//UINT _sType= (UINT)m_iSceneNum;
	//fwrite(&m_iSceneNum, sizeof(UINT), 1, _pFile);

	//// 2. 씬 이름 Save
	//wchar_t _sceneName;
	//FWRITE_WCHAR(m_strName, size, _pFile);


	//// 3. 씬에 존재하는 모델의 수 Save
	//fwrite(&m_iModelCnt, sizeof(UINT), 1, _pFile);

	//// 4. 씬에 존재하는 모델 전체 Save
	//for (UINT i = 0; i < (UINT)GROUP_TYPE::END; ++i)
	//{
	//	for (size_t j = 0; j < m_arrObj[i].size(); ++j)
	//	{
	//		m_arrObj[i][j]->Save(_pFile);
	//	}
	//}
}

void Scene::LoadObjects(FILE* _pFile)
{
	//// 3. 씬에 존재하는 모델의 수 Load
	//UINT _modelCnt;
	//fread(&_modelCnt, sizeof(UINT), 1, _pFile);

	//// 4. 씬에 존재하는 모델 전체 Load
	//for (UINT i = 0; i < _modelCnt; i++)
	//{

	//	Object* obj = nullptr;

	//	// 5 . 모델 그룹 번호 Load
	//	UINT _groupType;
	//	fread(&_groupType, sizeof(UINT), 1, _pFile);

	//	switch (_groupType)
	//	{
	//		case (UINT)GROUP_TYPE::EMPTHY:
	//		{
	//			break;
	//		}
	//		case (UINT)GROUP_TYPE::LIGHT:
	//		{
	//			obj = new LightClass();
	//			break;
	//		}
	//		case (UINT)GROUP_TYPE::CAMERA:
	//		{
	//			obj = new CameraClass();
	//			break;
	//		}

	//	}
	//	if (obj != nullptr)
	//	{
	//		obj->SetObjectGroupType(_groupType);
	//		SceneMgrClass::GetInst()->AddObjectInScene((UINT)m_iSceneNum, _groupType, obj->Load(_pFile));
	//	}

	//}
}

void Scene::AddNodeToScene(ModelNode* node)
{
	m_allNode->push_back(node);
}

void Scene::DeleteNode(ModelNode* node)
{
	for (int i = 0; i < m_allNode->size(); i++)
	{
		if (m_allNode->at(i) == node)
		{
			m_allNode->erase(m_allNode->begin() + i);
			break;
		}
	}
}