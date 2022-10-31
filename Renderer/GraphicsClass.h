// ��ó���� CREATEDLL_EXPORTS�� CREATEDLL_EXPORTS��� ��ó���Ⱑ ���ǵǾ����� Ȯ���ϴ� ���ǹ�
// ���� ���ǰ� �Ǿ��ٸ� MYDLL_DECLSPEC�� __declspec(dllexport) �ǹ�, �׷��� ������ __declspec(dllimport) �̰� �ǹ�
// ��ó���Ⱑ ���� �Ǿ����� Ȯ���� ���ؼ� ������Ʈ ���� > C/C++ > ��ó���� > ��ó���� ���ǿ��� Ȯ�� ����

// __declspec(dllexport) : Ű���尡 ���� �Լ��� DLL �ܺη� �����Ŵ 
// __declspec(dllimport) : �ٸ� DLL�� ���ǵ� �Լ��� ȣ���ϱ� ���� ���

// ���� �ͱ۸� : C++�� ��� C�� �ٸ��������Ϸ��� �Լ��� �̸��� ������ ��Ģ�� ������ �����ϴ� ���� �ǹ��Ѵ�.
// DLL�� ����Ͽ� ��ŷ�� �Ϸ��� �ϸ� ȣ���� �Լ��� �̸��� �˾ƾ��ϴµ� �ͱ۸��� �Ǹ� �˱Ⱑ �����. extern "C"�� ���� �ͱ۸��� �������� �ʰڴٴ� �ǹ� 

//���� windows�� �׷����� ������� �������̹Ƿ� d3dclass.h�� include ���ش�.
#include "define.h"
#include "D3DClass.h"
#include "ModelShader.h"
#include "ShaderManagerClass.h"

//Graphics�� SystemClass�� �����ϴ� �� �ٸ� ��ü�� �� ���ø����̼��� ��� �׷��� ����� �� ��ü �ȿ� ĸ��ȭ �Ǿ��ִ�.
//����� Ǯ��ũ�� ���� ���� �׷��Ȱ� ���õ� �������� ������ �Ͽ���.

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class MYDLL_DECLSPEC GraphicsClass {
private:
	SINGLE(GraphicsClass);

	//D3DClass�� �����ϴ� �����͸� �����Ѵ�.
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