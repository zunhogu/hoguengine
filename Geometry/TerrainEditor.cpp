#include "pch.h"
#include "TerrainEditor.h"
#include "TerrainComp.h"
#include "Core.h"
#include "KeyMgrClass.h"
#include "ResMgrClass.h"
#include "TimeMgrClass.h"

TerrainEditor::TerrainEditor(TerrainVertexType* vertexArray, int triangleSize, int terrainWidth, int terrainHeight)
{
	// Terrain Setting
	InputDesc* m_input = new InputDesc[triangleSize];
	m_output = new OutputDesc[triangleSize];
	for (int i = 0; i < triangleSize; i++)
	{
		UINT index0 = i* 3 + 0;
		UINT index1 = i * 3 + 1;
		UINT index2 = i * 3 + 2;

		m_input[i].v0 = vertexArray[index0].position;
		m_input[i].v1 = vertexArray[index1].position;
		m_input[i].v2 = vertexArray[index2].position;

		m_input[i].index = i;
	}
	m_structuredBuffer = new StructuredBuffer(m_input, sizeof(InputDesc), triangleSize, sizeof(OutputDesc), triangleSize);

	m_triangleSize = triangleSize;
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// Shader Setting
	m_computeShader = 0;
	CreateComputeShader();
}

TerrainEditor::~TerrainEditor()
{
	m_prevResource->Release();

	if (m_computePaintingShader)
	{
		delete m_computePaintingShader;
		m_computePaintingShader = 0;
	}

	if (m_structuredBuffer)
	{
		delete m_structuredBuffer;
		m_structuredBuffer = 0;
	}

	if (m_computeShader)
	{
		delete m_computeShader;
		m_computeShader = 0;
	}

}

void TerrainEditor::StartTerrainEditMode(TerrainLayer* layer)
{
	if (layer != nullptr)
	{
		m_layer = layer;

		TextureClass* alphaMap = ResMgrClass::GetInst()->FindTexture(m_layer->GetMaskID());

		ID3D11Texture2D* buffer = alphaMap->GetTexture();
		D3D11_TEXTURE2D_DESC textureDesc;
		buffer->GetDesc(&textureDesc);

		Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, &m_prevResource);
	}
	//Brush Setting
	m_brush.brushType = 1;
	m_brush.brushPosition = XMFLOAT3();
	m_brush.brushRange = 20.0f;
	m_brush.brushColor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_brush.chanel = XMFLOAT3();
}

void TerrainEditor::EndTerrainEditMode()
{
	if (m_layer)
	{
		while (!m_weightStack.empty())
		{ m_weightStack.pop();}
		m_layer = 0;
	}
	m_brush.brushType = 0;
}

void TerrainEditor::CreateComputeShader()
{
	wstring filePath = PathMgr::GetInst()->GetContentPath();
	wstring path;

	path = filePath + L"contents\\shader\\ComputeShader.hlsl";
	m_computeShader = new ComputePickingShader(path);

	path = filePath + L"contents\\shader\\ComputePaintingShader.hlsl";
	m_computePaintingShader = new ComputePaintingShader(path);
}

bool TerrainEditor::GetBrushPosition(XMMATRIX worldMatrix, XMFLOAT3 cameraPos, XMMATRIX viewMatrix, XMFLOAT3& position)
{
	bool result = false;
	int tmX, tmY;
	XMVECTOR A, B, C;

	ImGuIRenderClass::GetInst()->GetMousePosInViewPort(tmX, tmY);

	// 반직선 생성
	CollisionClass::GetInst()->SetRay(tmX, tmY, viewMatrix, Core::GetProjectionMatrix(), cameraPos);
	XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

	XMVECTOR rayOrigin, rayDir;

	// Picking Test
	rayOrigin = CollisionClass::GetInst()->GetRayOrigin();  // World Sapce
	rayDir = CollisionClass::GetInst()->GetRayDir();  // View Space
	rayDir = XMVector3TransformNormal(rayDir, invViewMatrix);  // View Space to World Sapce
	rayDir = XMVector3Normalize(rayDir);

	rayOrigin = XMVector3TransformCoord(rayOrigin, invWorldMatrix);  // World Space to Local Space
	rayDir = XMVector3TransformNormal(rayDir, invWorldMatrix);  // World Space to Local Space
	rayDir = XMVector3Normalize(rayDir);

	XMFLOAT3 pos, dir;
	XMStoreFloat3(&pos, rayOrigin);
	XMStoreFloat3(&dir, rayDir);

	// 셰이더와 상수버퍼 설정
	m_computeShader->SetShader();
	m_computeShader->SetConstantBuffer(pos, m_triangleSize, dir);

	// CS에서 사용하는 구조체 버퍼를 설정한다.
	Core::GetDeviceContext()->CSSetShaderResources(0, 1, &m_structuredBuffer->GetSRV());  // 입력자원의 경우 셰이더 자원 뷰를 생성한다.
	Core::GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_structuredBuffer->GetUAV(), nullptr);  // 출력 자원의 경우 순서 없는 접근 뷰를 생성한다.

	UINT x = ceil((float)m_triangleSize / 1024.0f);

	// Dispatch 함수를 통해 스레드 그룹들을 3차원 격자 형태로 구성한다. 
	Core::GetDeviceContext()->Dispatch(x, 1, 1);

	// 셰이더가 실행되고 나면 출력값을 복사해온다.
	m_structuredBuffer->Copy(m_output, sizeof(OutputDesc) * m_triangleSize);

	float minDistance = FLT_MAX;
	int minIndex = -1;

	for (UINT i = 0; i < m_triangleSize; i++)
	{
		OutputDesc temp = m_output[i];
		if (temp.picked)
		{
			if (minDistance > temp.distance)
			{
				minDistance = temp.distance;
				minIndex = i;
			}
		}
	}

	if (minIndex >= 0)
	{
		XMStoreFloat3(&position, XMVectorAdd(rayOrigin, rayDir * minDistance));
		result = true;
	}

	return result;
}

void TerrainEditor::PaintWeightMap(XMMATRIX baseViewMatrix)
{
	static bool isPushToStack = false;

	if(MOUSE_HOLD(0) && m_layer->GetMaskID() != L"")
	{
		TextureClass* alphaMap = ResMgrClass::GetInst()->FindTexture(m_layer->GetMaskID());

		if(!isPushToStack)
		{
			Core::GetDeviceContext()->CopyResource(m_prevResource, alphaMap->GetTexture());
			isPushToStack = true;
		}

		XMFLOAT2 uv = CalculateUV(m_brush.brushPosition, m_terrainWidth, m_terrainHeight);
		float range = m_brush.brushRange / m_terrainWidth;

		// rtt
		m_layer->GetAlphaMapBoard()->RenderToTextureStart(Core::GetDeviceContext());

		GraphicsClass::GetInst()->TurnZBufferOff();

		m_layer->GetBitmap()->Render(Core::GetDeviceContext());

		GraphicsClass::GetInst()->RenderTerrainPaintShader(Core::GetDeviceContext(), m_layer->GetBitmap()->GetIndexCount(), XMMatrixIdentity(), baseViewMatrix, m_layer->GetAlphaMapBoard()->GetOrthoMatirx(), m_brush.brushType, uv, range, m_brush.chanel, alphaMap->GetShaderResourceView());

		GraphicsClass::GetInst()->TurnZBufferOn();

		m_layer->GetAlphaMapBoard()->RenderToTextureEnd();

		ImGuIRenderClass::GetInst()->SetRenderToTexture(Core::GetDeviceContext());
		
		ID3D11Resource* resource = nullptr;
		m_layer->GetAlphaMapBoard()->GetShaderResourceView()->GetResource(&resource);
		alphaMap->Copy(resource);

		//// cs
		//TextureBuffer* textureBuffer = m_layer->GetTexturebuffer();

		//m_computePaintingShader->SetShader();
		//m_computePaintingShader->SetConstantBuffer(m_brush.brushType, uv, range, m_brush.chanel);

		//Core::GetDeviceContext()->CSSetShaderResources(0, 1, &alphaMap->GetShaderResourceView());
		//Core::GetDeviceContext()->CSSetShaderResources(1, 1, &m_layer->GetAlphaMapBoard()->GetShaderResourceView());
		//Core::GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &textureBuffer->GetUAV(), nullptr);

		//alphaMap->Copy(textureBuffer->GetOutput());

		//D3D11_TEXTURE2D_DESC desc;
		//alphaMap->GetTexture()->GetDesc(&desc);

		//// Dispatch 함수를 통해 스레드 그룹들을 3차원 격자 형태로 구성한다. 
		//Core::GetDeviceContext()->Dispatch(desc.Width, desc.Height, 1);

	}
	
	if (MOUSE_AWAY(0) && m_layer->GetMaskID() != L"")
	{
		ID3D11Texture2D* input = nullptr;

		ID3D11Texture2D* buffer = (ID3D11Texture2D*)m_prevResource;
		D3D11_TEXTURE2D_DESC textureDesc;

		buffer->GetDesc(&textureDesc);

		// RTT를 생성한다.
		Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, &input);

		Core::GetDeviceContext()->CopyResource(input, m_prevResource);
		m_weightStack.push(input);

		isPushToStack = false;
	}

	if ((KEY_HOLD(DIK_LCONTROL) && KEY_TAP(DIK_Z)))
	{
		if (!m_weightStack.empty())
		{
			ID3D11Texture2D* resource = m_weightStack.top();
			m_weightStack.pop();
			TextureClass* alphaMap = ResMgrClass::GetInst()->FindTexture(m_layer->GetMaskID());
			alphaMap->Copy(resource);
			resource->Release();
		}
	}
}

void TerrainEditor::PaintHeightMap(TerrainVertexType* vertices, int vertexSize, bool isRaise, int paintValue)
{
	float range = m_brush.brushRange / m_terrainWidth;

	if (MOUSE_HOLD(0))
	{
		for (int i = 0; i < vertexSize; i++)
		{
			XMFLOAT3 vertexPos = vertices[i].position;
			XMFLOAT3 brushPos = m_brush.brushPosition;

			float distX = abs(brushPos.x - vertexPos.x);
			float distZ = abs(brushPos.z - vertexPos.z);

			if (distX <= range && distZ <= range)
			{
				if (isRaise)
					vertices[i].position.y += paintValue * TimeMgrClass::GetInst()->GetDT();
				else
					vertices[i].position.y -= paintValue * TimeMgrClass::GetInst()->GetDT();

				// saturate
				vertices[i].position.y = max(0.0f, min(1.0f, vertices[i].position.y));
			}
		}
	}
}

XMFLOAT2 TerrainEditor::CalculateUV(XMFLOAT3 position, float width, float heigth)
{
	float percentX, percentY;

	if (position.x < 0)
		position.x = 0;

	if (position.z < 0)
		position.y = 0;

	if (position.x > width)
		position.x = width;

	if (position.z > heigth)
		position.y = heigth;

	// Terrain 사이즈를 기준으로 현재위치의 비율값 즉 uv를 구한다.
	percentX = position.x / width;
	percentY = 1.0f - (position.z / heigth);

	return XMFLOAT2(percentX, percentY);
}
