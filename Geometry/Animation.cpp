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
	// skinTransform은 모든 Bone들의 특정 시간의 최종적인 transform을 의미한다.
	// 이 행렬은 셰이더로 전달되어서 정점들이 이 행렬과 곱해지게 될것이다. 
	if (m_skinTransform == nullptr)
		m_skinTransform = new XMMATRIX[m_skeleton.size()];

	if (m_boneAnimationTransform == nullptr)
		m_boneAnimationTransform = new XMMATRIX[m_skeleton.size()];

	// 모든 Bone에 대해 해당 KeyFrame에서의 Transform을 수정한다.
	int index = 0;
	for (int i = 0; i < m_skeleton.size(); i++)
	{
		XMMATRIX matInvBindPose = m_skeleton[i]->GetBindPosTransformMatrix();  // 루트->본  ( T Pose에서의 본의 Transform)
		// XMMATRIX matBoneReference = bone->GetBoenReferenceTransform();  // ???
		XMMATRIX matAnimation;  // Bone이 자기 자신의 Local 좌표계 기준으로 얼마나 움직였는가? 
		XMMATRIX matParentAnimation;  // 해당Bone의 부모 Bone까지 합쳐서 얼마나 돈지 결정, 부모는 그 위에 부모의 Matrix값도 가지고 있음

		KeyFrames* animationKeyFrames = m_skeleton[i]->GetKeyFrames();

		if (isUseQuaternionKeyFrames == true)
		{
			// 현재 KeyFrame과 다음 KeyFrame의 Quaternion 값 로드
			const QuaternionKeyFrame& qKeyFrame1 = animationKeyFrames->GetQuaternionKeyFrame(keyFrame);
			const QuaternionKeyFrame& qKeyFrame2 = animationKeyFrames->GetQuaternionKeyFrame(nextKeyFrame);
			const XMVECTOR& q1 = qKeyFrame1.GetQuaternion();
			const XMVECTOR& q2 = qKeyFrame2.GetQuaternion();

			// Quaternion 사용하는 이유 
			// 1. 저장공간, 연산량 감소
			// 2. 짐벌락 방지
			// 3. 구면 보간 가능
			// (보간 : 점과 점 사이의 어떤 특정 값을 계산한다)
			// 여기서의 보간은 현재 KeyFrame과 다음 KeyFrame 사이의 특정 KeyFrame에서 Transform을 계산하는 것이다.
			// 보간에는 선형보간과 구면 보간이 있는데 선형 보간보다 구면 보간이 더 정확하다. 하지만 복잡, 하지만 쿼터니언쓰면 구면 보간 쉬움
			XMVECTOR q;  // 보간될 Quaternion
			q = XMQuaternionSlerp(q1, q2, keyFrameFactor);  // Quaternion(회전축, 회전각) 구면 보간

			matAnimation = XMMatrixRotationQuaternion(q);  // Quaternion을 Transfrom Matrix로 변환 

			// 쿼터니언은 축과 회전정보만 가지고 있으니 위치정보도 가지고 있어야한다.

			XMVECTOR t1 = XMLoadFloat3(&qKeyFrame1.GetTranslation());
			XMVECTOR t2 = XMLoadFloat3(&qKeyFrame2.GetTranslation());

			XMVECTOR vTranslation;  // 보간된 본의 위치
			vTranslation = XMVectorLerp(t1, t2, keyFrameFactor);   // 위치의 선형 보간

			// 회전축, 회전각이 구면보간된 Matrix에 선형 보간된 위치를 추가
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
			// 보간을 사용하지 않을 경우 그냥 KeyFrame Transform을 사용하면 됨
			// 대신 dx에서는 1000fps지만 fbx에서 10fps면 dx도 10fps로 렌더링하게 됨
			matAnimation = animationKeyFrames->GetKeyFrameTransform(keyFrame);
		}

		// skin transform에서 필요한 bone들에 대한 정보를 어떻게 할것인지 고민해보자
		int parentBoneIndex = m_skeleton[i]->GetParentBoneIndex();
		if (parentBoneIndex < 0)
		{
			//루트의 경우 부모 본 TransformAnimation은 Identity Transform을 가져옴
			matParentAnimation = XMMatrixIdentity();
		}
		else
		{
			matParentAnimation = m_boneAnimationTransform[parentBoneIndex];
		}
		// boneAnimationTransforms = (자신 본 -> 부모 본) * (부모 본 -> 부모의 부모 본) * ... (부모의 부모의 ... 부모본 ->루트 본) = (자신본 -> 루트본)
		m_boneAnimationTransform[index] = matAnimation * matParentAnimation;

		// Mesh상의 Vertex는 Root공간에 존재하므로 matInvBindPose(Bone의 초기값->루트)를 적용후 
		// boneAnimationTransforms(루트 본-> 자신 본)을 곱해 최종 Animation된 Bone의 위치로 적용
		m_skinTransform[index] = matInvBindPose * m_boneAnimationTransform[index];

		index++;

		// 이 코드가 끝나면 모든 Bone들의 skinTransform이 완성된다. 
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
