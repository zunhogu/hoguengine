// 전처리기 CREATEDLL_EXPORTS는 CREATEDLL_EXPORTS라는 전처리기가 정의되었는지 확인하는 조건문
// 만약 정의가 되었다면 MYDLL_DECLSPEC은 __declspec(dllexport) 의미, 그렇지 않으면 __declspec(dllimport) 이걸 의미
// 전처리기가 정의 되었는지 확인을 위해서 프로젝트 설정 > C/C++ > 전처리기 > 전처리기 정의에서 확인 가능

// __declspec(dllexport) : 키워드가 붙은 함수를 DLL 외부로 노출시킴 
// __declspec(dllimport) : 다른 DLL에 정의된 함수를 호출하기 위해 사용

// 네임 맹글링 : C++의 경우 C와 다르게컴파일러가 함수의 이름을 일정한 규칙을 가지고 변경하는 것을 의미한다.
// DLL을 사용하여 링킹을 하려고 하면 호출할 함수의 이름을 알아야하는데 맹글링이 되면 알기가 힘들다. extern "C"는 네임 맹글링을 수행하지 않겠다는 의미 

//기존 windows의 그래픽은 사용하지 않을것이므로 d3dclass.h를 include 해준다.
#include "define.h"
#include "D3DClass.h"
#include "ModelShader.h"
#include "ShaderManagerClass.h"

//Graphics는 SystemClass가 생성하는 또 다른 객체로 이 어플리케이션의 모든 그래픽 기능이 이 객체 안에 캡슐화 되어있다.
//헤더에 풀스크린 모드와 같은 그래픽과 관련된 전역변수 선언을 하였다.

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class MYDLL_DECLSPEC GraphicsClass {
private:
	SINGLE(GraphicsClass);

	//D3DClass를 참조하는 포인터를 선언한다.
	D3DClass* m_D3D;

	ShaderManagerClass* m_shaderMgr;

public:

	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();

	D3DClass* GetDirectDevice();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11RenderTargetView* const* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void ResetViewPort();

	void TurnZBufferOn();
	void TurnZBufferOff();
	
	void TurnOnCulling();
	void TurnOffCulling();

	void TurnOnWireFrame();
	void TurnOffWireFrame();

	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetViewMatrix();

	void BeginScene(float, float, float, float);
	void EndScene();

	void RenderModel(int indexcount,
		XMMATRIX worldMatrix,
		XMMATRIX veiwMatrix, XMFLOAT3 cameraPos,
		XMFLOAT4 lightColor, XMFLOAT3 lightPos, XMFLOAT3 lightDir,
		XMFLOAT4 ambientColor, XMFLOAT4 emmisvieColor, XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float shinnes,
		ID3D11ShaderResourceView* ambientTexture, ID3D11ShaderResourceView* emmisiveTexture, ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* normalTexture,
		XMMATRIX boneScale, XMMATRIX* boneMatrixArray, UINT skinning
	);

	void RenderGridShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix);
	void RenderGirdShader(ID3D11DeviceContext* deviceContext, int indexCount);
	void RenderSkyDome(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor);
	void RenderTerrainShaderSetParam(ID3D11DeviceContext* deviceContext, bool isWireFrame, bool isLOD, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* texture);
	void RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount);
	void RenderTerrainWireFrameShaderSetParam(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, float tessellationAmount);
	void RenderTerrainWireFrameShader(ID3D11DeviceContext* deviceContext, int indexCount);
	void RenderMaterialShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, ID3D11ShaderResourceView** textures);


};