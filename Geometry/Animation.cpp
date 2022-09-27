#include "pch.h"
#include "Animation.h"
#include "Core.h"

Bone::Bone()
{
}

Bone::Bone(string boneName, int parentIndex)
	:m_boneName(boneName), m_parentBoneIndex(parentIndex)
{
}

Bone::~Bone()
{
}

Animation::Animation()
{
}

Animation::Animation(const Animation&)
{
}

Animation::~Animation()
{
}

void Animation::Initialize(const wstring& _strFilePath)
{
	LoadAnimation(_strFilePath);
}

bool Animation::LoadAnimation(const wstring& _strFilePath)
{
	ifstream fin;

	m_animationName = Core::GetFileName(_strFilePath);

	fin.open(_strFilePath.c_str());
	if (fin.fail())
		return false;

	string buffer;

	fin >> buffer;
	fin >> buffer;
	fin >> m_keyFrames;

	fin >> buffer;
	fin >> buffer;
	fin >> m_fps;

	fin >> buffer;
	fin >> buffer;
	fin >> m_startTime;

	fin >> buffer;
	fin >> buffer;
	fin >> m_endTime;

	while (!fin.eof())
	{
		string boneName;
		fin >> buffer;
		fin >> buffer;
		fin >> boneName;  // bone name

		int parentBoneIndex;
		fin >> buffer;
		fin >> buffer;
		fin >> parentBoneIndex;

		Bone* bone = new Bone(boneName, parentBoneIndex);

		fin >> buffer;
		fin >> buffer;
		XMFLOAT4X4 transform4x4;
		fin >> transform4x4._11 >> transform4x4._12 >> transform4x4._13 >> transform4x4._14 >> transform4x4._21 >> transform4x4._22 >> transform4x4._23 >> transform4x4._24 >> transform4x4._31 >> transform4x4._32 >> transform4x4._33 >> transform4x4._34 >> transform4x4._41 >> transform4x4._42 >> transform4x4._43 >> transform4x4._44;

		bone->SetBindPosTransformMatrix(XMLoadFloat4x4(&transform4x4));

		KeyFrames* keyframes = new KeyFrames;
		for (int i = 0; i < m_keyFrames; i++)
		{
			XMFLOAT4X4 transform4x4;
			fin >> transform4x4._11 >> transform4x4._12 >> transform4x4._13 >> transform4x4._14 >> transform4x4._21 >> transform4x4._22 >> transform4x4._23 >> transform4x4._24 >> transform4x4._31 >> transform4x4._32 >> transform4x4._33 >> transform4x4._34 >> transform4x4._41 >> transform4x4._42 >> transform4x4._43 >> transform4x4._44;
			keyframes->AddKeyFrame(XMLoadFloat4x4(&transform4x4));
		}

		bone->SetKeyFrames(keyframes);

		m_skeleton.push_back(bone);
	}
}

void Animation::UpdateBone(int keyFrame, int nextKeyFrame, int keyFrameFactor, bool isUseQuaternionKeyFrames)
{
	// skinTransform�� ��� Bone���� Ư�� �ð��� �������� transform�� �ǹ��Ѵ�.
	// �� ����� ���̴��� ���޵Ǿ �������� �� ��İ� �������� �ɰ��̴�. 
	if (m_skinTransform == nullptr)
		m_skinTransform = new XMMATRIX[m_skeleton.size()];

	if (m_boneAnimationTransform == nullptr)
		m_boneAnimationTransform = new XMMATRIX[m_skeleton.size()];

	// ��� Bone�� ���� �ش� KeyFrame������ Transform�� �����Ѵ�.
	int index = 0;
	for (int i = 0; i < m_skeleton.size(); i++)
	{
		XMMATRIX matInvBindPose = m_skeleton[i]->GetBindPosTransformMatrix();  // ��Ʈ->��  ( T Pose������ ���� Transform)
		// XMMATRIX matBoneReference = bone->GetBoenReferenceTransform();  // ???
		XMMATRIX matAnimation;  // Bone�� �ڱ� �ڽ��� Local ��ǥ�� �������� �󸶳� �������°�? 
		XMMATRIX matParentAnimation;  // �ش�Bone�� �θ� Bone���� ���ļ� �󸶳� ���� ����, �θ�� �� ���� �θ��� Matrix���� ������ ����

		KeyFrames* animationKeyFrames = m_skeleton[i]->GetKeyFrames();

		if (isUseQuaternionKeyFrames == true)
		{
			// ���� KeyFrame�� ���� KeyFrame�� Quaternion �� �ε�
			const QuaternionKeyFrame& qKeyFrame1 = animationKeyFrames->GetQuaternionKeyFrame(keyFrame);
			const QuaternionKeyFrame& qKeyFrame2 = animationKeyFrames->GetQuaternionKeyFrame(nextKeyFrame);
			const XMVECTOR& q1 = qKeyFrame1.GetQuaternion();
			const XMVECTOR& q2 = qKeyFrame2.GetQuaternion();

			// Quaternion ����ϴ� ���� 
			// 1. �������, ���귮 ����
			// 2. ������ ����
			// 3. ���� ���� ����
			// (���� : ���� �� ������ � Ư�� ���� ����Ѵ�)
			// ���⼭�� ������ ���� KeyFrame�� ���� KeyFrame ������ Ư�� KeyFrame���� Transform�� ����ϴ� ���̴�.
			// �������� ���������� ���� ������ �ִµ� ���� �������� ���� ������ �� ��Ȯ�ϴ�. ������ ����, ������ ���ʹϾ𾲸� ���� ���� ����
			XMVECTOR q;  // ������ Quaternion
			q = XMQuaternionSlerp(q1, q2, keyFrameFactor);  // Quaternion(ȸ����, ȸ����) ���� ����

			matAnimation = XMMatrixRotationQuaternion(q);  // Quaternion�� Transfrom Matrix�� ��ȯ 

			// ���ʹϾ��� ��� ȸ�������� ������ ������ ��ġ������ ������ �־���Ѵ�.

			XMVECTOR t1 = XMLoadFloat3(&qKeyFrame1.GetTranslation());
			XMVECTOR t2 = XMLoadFloat3(&qKeyFrame2.GetTranslation());

			XMVECTOR vTranslation;  // ������ ���� ��ġ
			vTranslation = XMVectorLerp(t1, t2, keyFrameFactor);   // ��ġ�� ���� ����

			// ȸ����, ȸ������ ���麸���� Matrix�� ���� ������ ��ġ�� �߰�
			XMFLOAT4X4 matAnimation4x4;
			XMStoreFloat4x4(&matAnimation4x4, matAnimation);
			XMFLOAT3 vTranslation3;
			XMStoreFloat3(&vTranslation3, vTranslation);

			matAnimation4x4._41 = vTranslation3.x;
			matAnimation4x4._42 = vTranslation3.y;
			matAnimation4x4._43 = vTranslation3.z;

			matAnimation = XMLoadFloat4x4(&matAnimation4x4);
			vTranslation = XMLoadFloat3(&vTranslation3);
		}
		else
		{
			// ������ ������� ���� ��� �׳� KeyFrame Transform�� ����ϸ� ��
			// ��� dx������ 1000fps���� fbx���� 10fps�� dx�� 10fps�� �������ϰ� ��
			matAnimation = animationKeyFrames->GetKeyFrameTransform(keyFrame);
		}

		// skin transform���� �ʿ��� bone�鿡 ���� ������ ��� �Ұ����� ����غ���
		int parentBoneIndex = m_skeleton[i]->GetParentBoneIndex();
		if (parentBoneIndex < 0)
		{
			//��Ʈ�� ��� �θ� �� TransformAnimation�� Identity Transform�� ������
			matParentAnimation = XMMatrixIdentity();
		}
		else
		{
			matParentAnimation = m_boneAnimationTransform[parentBoneIndex];
		}
		// boneAnimationTransforms = (�ڽ� �� -> �θ� ��) * (�θ� �� -> �θ��� �θ� ��) * ... (�θ��� �θ��� ... �θ� ->��Ʈ ��) = (�ڽź� -> ��Ʈ��)
		m_boneAnimationTransform[index] = matAnimation * matParentAnimation;

		// Mesh���� Vertex�� Root������ �����ϹǷ� matInvBindPose(Bone�� �ʱⰪ->��Ʈ)�� ������ 
		// boneAnimationTransforms(��Ʈ ��-> �ڽ� ��)�� ���� ���� Animation�� Bone�� ��ġ�� ����
		m_skinTransform[index] = matInvBindPose * m_boneAnimationTransform[index];

		index++;

		// �� �ڵ尡 ������ ��� Bone���� skinTransform�� �ϼ��ȴ�. 
	}
}

KeyFrames::KeyFrames()
{
}

KeyFrames::~KeyFrames()
{
}

void KeyFrames::AddKeyFrame(const XMMATRIX& transforom)
{
	QuaternionKeyFrame* temp = new QuaternionKeyFrame(transforom);
	m_keyFrames.push_back(Pair(transforom, *temp));
}

const XMMATRIX& KeyFrames::GetKeyFrameTransform(int keyFrame) const
{
	return m_keyFrames[keyFrame].first;
}

const QuaternionKeyFrame& KeyFrames::GetQuaternionKeyFrame(int keyFrame) const
{
	return m_keyFrames[keyFrame].second;
}
