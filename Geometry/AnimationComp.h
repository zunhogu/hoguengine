#pragma once
#include "ModelComp.h"
#include "Animation.h"

class ModelBuffer
{
private:
	struct Data
	{
		XMMATRIX boneScale;
		XMMATRIX boneMatrixArray[100];
		UINT isSkinning;
		XMFLOAT3 padding;
	};

	Data data;
public:
	ModelBuffer()
	{
		data.boneScale = XMMatrixIdentity();

		for (int i = 0; i < 100; i++)
			data.boneMatrixArray[i] = XMMatrixIdentity();
		data.isSkinning = false;
	}
	~ModelBuffer() {}

	void SetSkinning(bool isSkinning)
	{
		data.isSkinning = (UINT)isSkinning;
	}

	void SetBoneArray(XMMATRIX* matrix, UINT count)
	{
		memcpy(data.boneMatrixArray, matrix, count * sizeof(XMMATRIX));

		for (UINT i = 0; i < count; i++)
		{
			data.boneMatrixArray[i] = XMMatrixTranspose(matrix[i]);
		}
	}
public:

	XMMATRIX GetBoneScale() { return data.boneScale; }
	XMMATRIX* GetBoneMatrixArray() { return data.boneMatrixArray; }
	UINT GetBoneSkinning() { return data.isSkinning; }
};

class ModelBuffer;

class AnimationComp : public ModelComp
{
private:
	vector<Animation*> m_animations;

	bool m_isAnimate;
	int m_currAnimIndex;
	char* m_currAnimName;
	float m_progress;

	int m_currKeyFrame;
	int m_nextKeyFrame;
	float m_accumulateDirectXDeltaTime;
	float m_keyFrameFactor;

	bool m_isSkinning;

	ModelBuffer* m_modelBuffer;

public:
	AnimationComp();
	AnimationComp(const AnimationComp& animationComp);
	~AnimationComp();

	void Shutdown();

	void AddAnimation(Animation* animation) { m_animations.push_back(animation); }

	bool GetAnimationMode() { return m_isAnimate; }
	char* GetCurrAnimationName() { return m_currAnimName; }
	float GetAnimationProgress() { return m_progress; }

	void Update();
	void UpdateAnimation(ModelBuffer* modelBuffer);

	virtual void Render(ModelNode* node);
	virtual void Clear();

	bool IsSkinning() { return m_isSkinning; }
	void SetSkinning(bool tf) { m_isSkinning = tf; }

	void SetModelBuffer(ModelBuffer* buffer) { m_modelBuffer = buffer; }
	ModelBuffer* GetModelBuffer() { return m_modelBuffer; }
};

