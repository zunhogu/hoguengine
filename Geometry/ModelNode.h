#pragma once
class ModelComp;
class ModelInfoComp;
class TransformComp;
class MeshComp;
class MaterialComp;
class AnimationComp;
class TerrainComp;

class ModelNode
{
private:
	vector <ModelComp*> m_modelComps;
	vector <ModelNode*> m_childNodes;

	int m_parentindex;
	vector <ModelNode*> m_pathToRootNode;

	int m_treeDepth;

	bool m_isOnlyMeshPicking;
	bool m_isOpenedInHierachy;
	bool m_isClickedHierachy;

	BoundingOrientedBox m_OBB;

public:
	ModelNode();
	ModelNode(const ModelNode& node, vector<ModelNode*> path = {});
	~ModelNode();

	void Shutdown();

	void SetTranslationChild(XMFLOAT3 translation);
	void CalculateTranslation(XMMATRIX translationMat);
	void SetRotationChild(XMFLOAT3 translation);
	void CalculateRotation(XMMATRIX translationMat);
	void SetScaleChild(XMFLOAT3 translation);
	void CalculateScale(XMMATRIX translationMat);

	void AddModelComp(ModelComp* comp) { m_modelComps.push_back(comp); }
	vector <ModelComp*>* GetModelComps() { return &m_modelComps; }

	ModelInfoComp* GetModelInfoComp();
	TransformComp* GetModelTransformComp();
	MeshComp* GetMeshComp();
	ModelComp* GetModelComp(COMPONENT_TYPE type);
	MaterialComp* GetMaterialComp(wstring id);
	AnimationComp* GetAnimationComp();
	TerrainComp* GetTerrainComp();

	void AddChildeNode(ModelNode* node) { return m_childNodes.push_back(node); }
	vector <ModelNode*>* GetChildNodes() { return &m_childNodes; }  // 바로 밑의 자식만 출력
	ModelNode* FindChildeNode(wstring nodeName);
	vector<ModelNode*> GetAllChildNodes();  // 모든 자식 출력
	void NodeCircuit(vector<ModelNode*>& arr);
	ModelNode* GetRootNode();

	int GetParentNodeIndex() { return m_parentindex; }
	void SetParentNodeIndex(int idx) { m_parentindex = idx; }

	vector <ModelNode*>* GetPathToRootNode() { return &m_pathToRootNode; }
	void AddPathToRootNode(ModelNode* node) { return m_pathToRootNode.push_back(node); }

	int GetTreeDepth() { return m_treeDepth; }
	void SetTreeDepth(int depth) { m_treeDepth = depth; }

	void Render(XMMATRIX viewMatirx, XMFLOAT3 cameraPos, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightPos, XMFLOAT3 lightDirection);
	void RenderMesh(MeshComp* meshComp, XMMATRIX worldMatrix, XMMATRIX viewMatirx, XMFLOAT3 cameraPos, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightPos);

	XMMATRIX GetWorldMatrix();
	XMMATRIX GetWorldMatrixExceptThis();

	XMMATRIX GetTranslationMatrix();
	XMMATRIX GetRotationMatrix();
	XMMATRIX GetScaleMatrix();
	XMVECTOR GetQuaternion();

	// OBB Collision
	void CreateOBB();
	void CalculateMinMax(XMVECTOR& max, XMVECTOR& min);
	BoundingOrientedBox GetOBB() { return m_OBB; }
	void SetOBB(BoundingOrientedBox box) { m_OBB = box; }

	// Check Picking
	bool CheckPicking(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis);
	bool CheckPickingOBB(XMVECTOR rayOrigin, XMVECTOR rayDir, float& _dis);
	ModelNode* CheckPickingMesh(XMVECTOR rayOrigin, XMVECTOR rayDir);
	ModelNode* CheckPickingTriangle(XMVECTOR rayOrigin, XMVECTOR rayDir, float& tmin);
	void Clear();
	bool GetIsOnlyMeshPicking() { return m_isOnlyMeshPicking; }
	void SetIsOnlyMeshPicking(bool tf) { m_isOnlyMeshPicking = tf; }

	// Hierachy
	bool GetIsOpenedHierachy() { return m_isOpenedInHierachy; }
	void SetIsOpenedHierachy(bool tf) { m_isOpenedInHierachy = tf; }
	void SetIsOpenedHierachyAllTrueToRootPath();

	bool GetIsClickedHierachy() { return m_isClickedHierachy; }
	void SetIsClickedHierachy(bool tf) { m_isClickedHierachy = tf; }
	void SetIsClicekdHierachyAllFalse();
};

