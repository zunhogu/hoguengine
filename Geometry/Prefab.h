#pragma once
#include "ResourceClass.h"

class Mesh;
class ModelNode;

class Prefab : public ResourceClass
{
private:
	ModelNode* m_modelNode; // root node

	vector<ModelNode*> m_modelNodes;
public:
	Prefab();
	Prefab(const Prefab& prefab);
	~Prefab();

public:
	bool Initialize(const wstring& _strFilePath);
	bool LoadPrefab(const wstring& _strFilePath);

	ModelNode* GetModelNode() { return m_modelNode; }

};

