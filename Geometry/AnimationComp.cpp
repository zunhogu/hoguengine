#include "pch.h"
#include "AnimationComp.h"
#include "Core.h"
#include "TimeMgrClass.h"
#include "MeshComp.h"

const UINT MaxBonesPerVertex = 4;
const UINT MaxBonesPerMax = 100;

AnimationComp::AnimationComp()
{
	m_componentType = COMPONENT_TYPE::ANIMATION;
	m_isAnimate = false;
	m_currAnimIndex = 0;
	m_currAnimName = nullptr;
	m_progress = 0;

	m_currKeyFrame = 0;
	m_nextKeyFrame = 0;
	m_accumulateDirectXDeltaTime = 0;
	m_keyFrameFactor = 0;

	m_isSkinning = false;

	m_modelBuffer = new ModelBuffer;
}

AnimationComp::AnimationComp(const AnimationComp& animationComp)
{
	m_componentType = COMPONENT_TYPE::ANIMATION;
	m_isAnimate = false;
	m_currAnimIndex = 0;
	m_currAnimName = nullptr;
	m_progress = 0;

	m_currKeyFrame = 0;
	m_nextKeyFrame = 0;
	m_accumulateDirectXDeltaTime = 0;
	m_keyFrameFactor = 0;

	m_isSkinning = false;

	m_modelBuffer = new ModelBuffer;

	for (int i = 0; i < animationComp.m_animations.size(); i++)
		m_animations.push_back(animationComp.m_animations[i]);
}

AnimationComp::~AnimationComp()
{
}

void AnimationComp::Shutdown()
{
	if (m_modelBuffer)
	{
		delete m_modelBuffer;
		m_modelBuffer = 0;
	}
}

void AnimationComp::Update()
{
	if (!m_isAnimate)
		return;

    // DirectX Delta Time
    m_accumulateDirectXDeltaTime += TimeMgrClass::GetInst()->GetDT();
    
    // FBX Delta Time 
    float fbxDeltaTime = 1.0f / m_animations[m_currAnimIndex]->GetFPS();

    while (m_accumulateDirectXDeltaTime > fbxDeltaTime)
    {
        // 만약 directX에서의 누적시간이 fbx에서의 누적시간을 초과하게 되면 현재 keyFrames을 다음 keyFrame으로 바꿔준다.
		m_currKeyFrame = (m_currKeyFrame + 1) % m_animations[m_currAnimIndex]->GetKeyFrames();
        m_nextKeyFrame = (m_currKeyFrame + 1) % m_animations[m_currAnimIndex]->GetKeyFrames();

        m_accumulateDirectXDeltaTime -= fbxDeltaTime;
    }

    // KeyFrame의 보정값을 계산 [0 ~ 1]
    // 현재 Frame과 다음 Frame 사이에서 어느정도 시간이 경과되었는지를 나타냄
    // 현재 프레임과 다음 프레임 사이의 위치에서 어느정도 진행됬는가
    m_keyFrameFactor = m_accumulateDirectXDeltaTime / fbxDeltaTime;

}

void AnimationComp::UpdateAnimation(ModelBuffer* modelBuffer)
{
	if (m_currAnimName != nullptr)
	{
		int currFrame = 0;
		float accumulateTime = 0.0f;
		float deltaTime = 1 / m_animations[m_currAnimIndex]->GetFPS();
		while (accumulateTime <= m_progress)
		{
			accumulateTime += deltaTime;
			currFrame++;
		}
		m_currKeyFrame = currFrame % m_animations[m_currAnimIndex]->GetKeyFrames();

		Update();

		m_animations[m_currAnimIndex]->UpdateBone(m_currKeyFrame, m_nextKeyFrame, m_keyFrameFactor, true);
		
		XMMATRIX* boneMatrixArray = m_animations[m_currAnimIndex]->GetSkinTransform();
		if (boneMatrixArray != nullptr)
		{
			UINT boneCount = m_animations[m_currAnimIndex]->GetBoneCount();
			boneCount = boneCount < MaxBonesPerMax ? boneCount : MaxBonesPerMax;

			modelBuffer->SetBoneArray(boneMatrixArray, boneCount);
		}
	}
}


void AnimationComp::Render(ModelNode* node)
{
	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static bool animate = false;
		static bool stop = false;

		static double startTime = 0.0f;
		static double endTime = 0.0f;
		static float deltaTime = 0.0f;

		ImGui::PushItemWidth(220.0f);
		if (ImGui::BeginCombo("##animCombo", m_currAnimName))
		{
			for (int n = 0; n < m_animations.size(); n++)
			{
				char* animName = Core::ConvWcharTochar(m_animations[n]->GetAnimationName());

				static bool is_selected = (m_currAnimName == animName);
				
				if (ImGui::Selectable(animName, is_selected))
				{
					m_currAnimName = animName;
					startTime = m_animations[n]->GetStartTime();
					endTime = m_animations[n]->GetEndTime();
					deltaTime = 1 / m_animations[n]->GetFPS();
					m_currAnimIndex = n;
					m_progress = 0.0f;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("Animate", &animate))
		{
			if (!animate)
			{
				m_progress = 0;
				m_isSkinning = false;
			}
		}

		if (ImGui::SliderFloat("##animationSlider", &m_progress, 0, endTime))
		{
			stop = true;
		}
		else
		{
			if (m_currAnimName && animate && !stop)
			{
				m_isSkinning = true;
				m_isAnimate = true;
				m_progress += deltaTime;
				if (m_progress >= endTime) { m_progress = 0.0f; }
			}
			else
			{
				m_isAnimate = false;
			}
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Stop", &stop))
		{
		}

	}
}

void AnimationComp::Clear()
{
	m_isAnimate = false;
	m_currAnimIndex = 0;
	m_currAnimName = nullptr;
	m_progress = 0;

	m_currKeyFrame = 0;
	m_nextKeyFrame = 0;
	m_accumulateDirectXDeltaTime = 0;
	m_keyFrameFactor = 0;

	m_isSkinning = false;
}
