#pragma once
#include "ResourceClass.h"

class QuaternionKeyFrame
{
private:
	XMVECTOR m_quaternion;
	XMFLOAT3 m_translation;
public:
	QuaternionKeyFrame(const XMMATRIX& transform)
	{
		m_quaternion = XMQuaternionRotationMatrix(transform);

		XMFLOAT4X4 transform4x4;
		XMStoreFloat4x4(&transform4x4, transform);
		m_translation = XMFLOAT3(transform4x4._41, transform4x4._42, transform4x4._43);
	}
	const XMVECTOR& GetQuaternion() const { return m_quaternion; }
	const XMFLOAT3& GetTranslation() const { return m_translation; }
};

class KeyFrames
{
private:
	typedef pair<XMMATRIX, QuaternionKeyFrame> Pair;
	vector<Pair> m_keyFrames;
public:
	KeyFrames();
	~KeyFrames();

	void AddKeyFrame(const XMMATRIX& transforom);

	const XMMATRIX& GetKeyFrameTransform(int keyFrame) const;
	const QuaternionKeyFrame& GetQuaternionKeyFrame(int keyFrame) const;
};

class Bone
{
private:
	string m_boneName;
	int m_parentBoneIndex;

	XMMATRIX m_bindPoseTransformMatrix;  // 루트 -> 본
	KeyFrames* m_keyFrames;
public:
	Bone();
	Bone(string boneName, int parentIndex);
	~Bone();

	string GetBoneName() { return m_boneName; }
	void SetBoneName(string name) { m_boneName = name; }

	int GetParentBoneIndex() { return m_parentBoneIndex; }
	void SetParentBoneIndex(int idx) { m_parentBoneIndex = idx; }

	XMMATRIX GetBindPosTransformMatrix() { return m_bindPoseTransformMatrix; }
	void SetBindPosTransformMatrix(XMMATRIX mat) { m_bindPoseTransformMatrix = mat; }

	KeyFrames* GetKeyFrames() { return m_keyFrames; }
	void SetKeyFrames(KeyFrames* keyframes) { m_keyFrames = keyframes; }
};

class Animation : public ResourceClass
{
private:
	wstring m_animationName;
	int m_keyFrames;
	float m_fps;
	float m_startTime;
	float m_endTime;

	vector<Bone*> m_skeleton;

	// bone들 마다의 skinTransform 
	XMMATRIX* m_skinTransform;
	XMMATRIX* m_boneAnimationTransform;

public:
	Animation();
	Animation(const Animation&);
	~Animation();

	void Initialize(const wstring& _strFilePath);

	wstring GetAnimationName() { return m_animationName; }
	void SetAnimationName(wstring name) { m_animationName = name; }
	int GetKeyFrames() { return m_keyFrames; }
	void SetKeyFrames(int keyframes) { m_keyFrames = keyframes; }
	float GetFPS() { return m_fps; }
	void SetFPS(float fps) { m_fps = fps; }
	float GetStartTime() { return m_startTime; }
	void SetStartTime(float time) { m_startTime = time; }
	float GetEndTime() { return m_endTime; }
	void SetEndTime(float time) { m_endTime = time; }

	bool LoadAnimation(const wstring& _strFilePath);

	XMMATRIX* GetSkinTransform() { return m_skinTransform; }
	int GetBoneCount() { return m_skeleton.size(); }
		 
	void UpdateBone(int keyFrame, int nextKeyFrame, int keyFrameFactor, bool isUseQuaternionKeyFrames);
};

