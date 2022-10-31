#include "pch.h"
#include "ViewPortPanel.h"
#include "ModelComp.h"
#include "TransformComp.h"
#include "ImGuIRenderClass.h"
#include "ContentBrowserPanel.h"
#include "ModelNode.h"
#include "Prefab.h"
#include "ResMgrClass.h"
#include "SceneMgrClass.h"
#include "Scene.h"
#include "RenderTextureClass.h"

// extern global value

// Selected Model
extern vector<ModelNode*> g_selectedNodes;

// gizmo
extern ImGuizmo::OPERATION g_gizmoOper;
extern ImGuizmo::MODE g_gizmoMode;
extern bool g_isGizmoClicked;

// viewport
extern ImVec2 g_viewPortSize;  // 뷰포트 크기
extern ImVec2 g_viewPortPos;  // 뷰포트 위치
extern ImVec2 g_prevViewPortSize;  // 뷰포트 이전 크기
extern ImVec2 g_mousePos;   // 전체 윈도우 안에서 마우스 좌표
extern ImVec2 g_mousePosInViewPort;  // 뷰포트 안에서의 마우스 좌표 
extern bool g_isViewPortResizing;

// window focus
extern IMGUI_WINDOW_TYPE g_focusedWindow;
extern IMGUI_WINDOW_TYPE g_iCurMousePosWindowID;

// drag and drop
extern wstring g_resourceRelativePath;
extern RESOURCE_TYPE g_rosourceType;

ViewPortPanel::ViewPortPanel()
{
	m_type = IMGUI_WINDOW_TYPE::VIEWPORT;
	m_isRender = false;

}

ViewPortPanel::~ViewPortPanel()
{
}

bool ViewPortPanel::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	bool result = true;

	m_renderTexture = new RenderTextureClass;
	if (!m_renderTexture)
		return false;

	result = m_renderTexture->Initialize(device, deviceContext, textureWidth, textureHeight, screenDepth, screenNear);
	if (!result)
		return false;

	return true;
}

void ViewPortPanel::Shutdown()
{
	if (m_renderTexture) {
		m_renderTexture->Shutdown();
		delete m_renderTexture;
		m_renderTexture = 0;
	}
}

void ViewPortPanel::Render()
{
	// Viewport
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정


		if (ImGui::Begin("Viewport"))
		{
			if (IMGUI_WINDOW_FOCUS_CHECK)
			{
				g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::VIEWPORT;
				if (ImGui::IsMouseClicked(0))
					g_focusedWindow = IMGUI_WINDOW_TYPE::VIEWPORT;
			}

			g_viewPortPos = ImGui::GetCursorScreenPos();
			g_viewPortSize = ImGui::GetWindowSize();
			g_mousePos = ImGui::GetMousePos();

			g_mousePosInViewPort.x = g_mousePos.x - g_viewPortPos.x;
			g_mousePosInViewPort.y = g_mousePos.y - g_viewPortPos.y;

			// mouse pos 
			if (g_mousePosInViewPort.x <= 0) { g_mousePosInViewPort.x = 0; }
			if (g_mousePosInViewPort.y <= 0) { g_mousePosInViewPort.y = 0; }
			if (g_mousePosInViewPort.x >= g_viewPortSize.x) { g_mousePosInViewPort.x = g_viewPortSize.x; }
			if (g_mousePosInViewPort.y >= g_viewPortSize.y) { g_mousePosInViewPort.y = g_viewPortSize.y; }

			//IMGUI_VIEWPORT_RESIZING_CHECK;  // 리사이징 체크 

			if (g_prevViewPortSize.x != g_viewPortSize.x || g_prevViewPortSize.y != g_viewPortSize.y)  // 뷰포트 리사이징 확인
			{
				g_isViewPortResizing = true;
			}
			g_prevViewPortSize.x = g_viewPortSize.x;
			g_prevViewPortSize.y = g_viewPortSize.y;

			ID3D11ShaderResourceView* image;   // RTT를 관리하는 자원 뷰를 가져온다. 
			image = m_renderTexture->GetShaderResourceView();
			ImGui::GetWindowDrawList()->AddImage((ImTextureID)image, ImVec2(ImGui::GetCursorScreenPos()), ImVec2(ImGui::GetCursorScreenPos().x + g_viewPortSize.x, ImGui::GetCursorScreenPos().y + g_viewPortSize.y), ImVec2(0, 0), ImVec2(1, 1));

			ProcessDragAndDropPayload(ImGuIRenderClass::DragAndDropToWindow("CONTENT_BROWSER_ITEM"));

			// Gizmo
			CreateGizmo();
		}
		ImGui::End();
		ImGui::PopStyleVar();   // padding 값 설정s
	}
}

void ViewPortPanel::ProcessDragAndDropPayload(ImGuiPayload* payload)
{
	if (payload == nullptr)
		return;

	wstring fileRelativePath = (wchar_t*)payload->Data;
	wstring fileFileName = Core::GetFileName(fileRelativePath);
	wstring fileExtension = Core::GetFileExtension(fileRelativePath);

	for (int i = 0; i < IM_ARRAYSIZE(g_modelExtension); i++)
	{
		if (fileExtension == g_modelExtension[i])
		{

			Scene* currScene = SceneMgrClass::GetInst()->GetCurScene();

			Prefab* prefab = ResMgrClass::GetInst()->LoadPrefab(fileFileName, fileRelativePath);
			
			currScene->AddNodeToScene(new ModelNode(*prefab->GetModelNode()));

			return;
		}
	}
}


void ViewPortPanel::CreateGizmo()
{
	// Gizmos
	if (g_selectedNodes.size() > 0)
	{
		ModelNode* selectedNode = g_selectedNodes[g_selectedNodes.size() - 1];

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(g_viewPortPos.x, g_viewPortPos.y, g_viewPortSize.x, g_viewPortSize.y);

		// Camera
		XMFLOAT4X4 viewMat, projectionMat;
		XMStoreFloat4x4(&viewMat, Core::GetViewMatrix());
		XMStoreFloat4x4(&projectionMat, Core::GetProjectionMatrix());

		XMFLOAT4X4 transformMat4x4;
		XMFLOAT4X4 parentTransformMat4x4;
		XMFLOAT4X4 deltaTransformMatrix4x4;

		// Selected Object Transform
		TransformComp* transform = selectedNode->GetModelTransformComp();

		// 현재 노드의 transform 정보
		XMFLOAT3 position = transform->GetPosition();
		XMFLOAT3 rotation = transform->GetRotation();
		XMFLOAT3 scale = transform->GetScale();

		// 현재 노드를 제외하고 루트까지의 transform 정보
		XMStoreFloat4x4(&parentTransformMat4x4, selectedNode->GetWorldMatrixExceptThis());
		XMFLOAT3 pPosition, pRotation, pScale;
		ImGuizmo::DecomposeMatrixToComponents(*parentTransformMat4x4.m, &pPosition.x, &pRotation.x, &pScale.x);

		// 기즈모를 부모의 transform에 맞추기 위해서 아래 작업을 수행 
		position.x += pPosition.x;
		position.y += pPosition.y;
		position.z += pPosition.z;

		rotation.x += pRotation.x;
		rotation.y += pRotation.y;
		rotation.z += pRotation.z;

		scale.x *= pScale.x;
		scale.y *= pScale.y;
		scale.z *= pScale.z;

		ImGuizmo::RecomposeMatrixFromComponents(&position.x, &rotation.x, &scale.x, *transformMat4x4.m);

		ImGuizmo::Manipulate(*viewMat.m, *projectionMat.m, g_gizmoOper, g_gizmoMode, *transformMat4x4.m); // gizmo create
		
		if (ImGuizmo::IsUsing())   //gizmo click 
		{
			XMFLOAT3 t, r, s;

			ImGuizmo::DecomposeMatrixToComponents(*transformMat4x4.m, &t.x, &r.x, &s.x);

			// 변환이 끝났다면 실제 노드에 들어갈 transform 정보는 부모 transform 정보를 뺀 부분이 들어간다. 
			t.x -= pPosition.x;
			t.y -= pPosition.y;
			t.z -= pPosition.z;

			r.x -= pRotation.x;
			r.y -= pRotation.y;
			r.z -= pRotation.z;

			s.x /= pScale.x;
			s.y /= pScale.y;
			s.z /= pScale.z;

			switch (g_gizmoOper)
			{
				case ImGuizmo::TRANSLATE:
				{
					transform->SetPosition(t);
					break;
				}
				case ImGuizmo::ROTATE:
				{
					transform->SetRotation(r);
					break;
				}
				case ImGuizmo::SCALE:
				{
					transform->SetScale(s);
					break;
				}
			}
			g_isGizmoClicked = true;
		}
		else {
			g_isGizmoClicked = false;
		}
	}
}