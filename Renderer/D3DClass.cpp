#include "pch.h"
#include "D3DClass.h"


D3DClass::D3DClass() {
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_depthDisabledStencilState = 0;
	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;
}

D3DClass::D3DClass(const D3DClass& other) {}

D3DClass::~D3DClass() {}


bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear) {
	//screenWidth : ������ ���� �ʺ�
	//screenHeight : ������ ���� ����
	//vsync : Direct3D �������� ������� �ֻ����� ���� ������, �ƴϸ� ������ �� ������ �ٽ� �׸� ������ �����Ѵ�.
	//hwnd : ������ ������� �����쿡 ���� �ڵ�
	//fullscreen : ���ø����̼��� ������ ��忡�� �������� ��ü ȭ�� ��忡�� �������� �˷��ش�.
	//screenDepth : ȭ�鿡 �׷��� ���̰� -> ��ĵ��� ���Ҷ� ���ȴ�. 
	//screenNear : ȭ�鿡 �׷��� ���̰� -> ��ĵ��� ���Ҷ� ���ȴ�. 

	m_depthNear = screenNear;
	m_depthFar = screenDepth;

	HRESULT result;

	//�׷���ī��/����� �ֻ��� ���� �� ���Ǵ� ����
	IDXGIFactory* factory;   // �׷���ī�� �������̽� ���丮
	IDXGIAdapter* adapter;   // �׷���ī�� �������̽� �����
	IDXGIOutput* adapterOutput;   // ������� ���
	unsigned int numModes, i, numerator, denominator, stringLength;   // ��� ��, , �ֻ��� ����, �ֻ��� �и�, �׷���ī�� �̸�
	DXGI_MODE_DESC* displayModeList;   // ����Ϳ� �׷���ī�� ���� ����Ʈ
	DXGI_ADAPTER_DESC adapterDesc;   // ����� description (�׷���ī�忡�� ������)
	int error;

	//����ü�� description ����ü���� ���Ǵ� ����
	DXGI_SWAP_CHAIN_DESC swapChainDesc;   // ����ü��
	D3D_FEATURE_LEVEL featureLevel; // ����ü�� ��ó����

	//����ü�ΰ� ����۸� �����ų�� ����ϴ� ���� 
	ID3D11Texture2D* backBufferPtr; // ����� ������
	D3D11_TEXTURE2D_DESC backBufferDesc;  // ����ۿ� ���� description
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc; // ������� ���ڿ� description
	D3D11_TEXTURE2D_DESC depthBufferDesc;   //���� ���� description

	//���� ������ description ����ü���� ���Ǵ� ����
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;   // ����-���ٽ� description
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;  //����-���ٽ� �� description
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// ���� ���¸� �����ϱ����� description ����
	D3D11_BLEND_DESC blendStateDescription;


	// vsync(��������ȭ) ������ �����Ѵ�.
	// vsync ������ true���� ������ �ý��ۿ��� ���� �ֻ���(60Hz)�� �����Ѵ�. false�� ������ 1�ʿ� �ִ��� ������ ȭ�鿡 �׷�������.
	// ����� VSYNC_ENABLED ���� �־� ����� �ֻ����� �����ϰ� �Ѵ�. 
	m_vsync_enabled = vsync;

	// Direct3D�� �ʱ�ȭ�ϱ� ���� �׷���ī��/������� �ֻ���(���ΰ�ħ ����)�� �˾ƾ��Ѵ�.
	// �� �׷���ī���� �̸���, �׷���ī���� ��밡���� �޸� ũ�⵵ �˾ƾ��Ѵ�. 
	// �� ��ǻ�͸��� �� ������ ���ݾ� �ٸ��� ������ ��ǻ�Ϳ��� �ش������� ����͸� ���ؼ���ȸ�ؾ��Ѵ�.
	// �� ������ ���� �и��� ��ȸ�� �� ���� �߿� DirectX���� �� ���� �˷��ָ� ������ �ֻ����� ����Ѵ�.
	// ���� �� �۾��� ���� �ʴ´ٸ� �⺻������ �������� DrirectX ȭ���� ǥ���Ҷ� ���� �ø��� ������� �ʰ� blit�� ����Ͽ� ������ ���ϵǰ�, ����� ��¿� �Ž����� �޼����� �����.

	/////////////////////////////////////////////////////////////////////////////////////
	//�׷���ī��/����� �ֻ���, �׷���ī�� �̸�, �׷���ī�� ��� ������ �޸� ��������//
	/////////////////////////////////////////////////////////////////////////////////////
	// DirectX �׷��� �������̽� ���丮�� �����.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		return false;
	}

	// ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� ����͸� �����.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		return false;
	}

	// ���(���Ƽ)�� ���� ù��° ����͸� �����Ѵ�.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ����� ������ ���Ѵ�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����Ѵ�.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return false;
	}

	// ���÷��� ��忡 ���� ����Ʈ ������ ä���ִ´�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	// ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã�´�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����Ѵ�.
	for (i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// �̷��� �ֻ����� ���ڿ� �и� ������ �Ǹ�, �׷��� ī���� �̸��� ��� ������ �׷��� ī�� �޸��� ũ�⸦ ����͸� �̿��Ͽ� �޾ƿ´�.
	// �����(�׷���ī��)�� description�� �����´�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	// ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ �����Ѵ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �׷��� ī���� �̸��� char�� ���ڿ� �迭�� �ٲ� �� �����Ѵ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	//���� ����� �ֻ��� ������ ����, �и𰪰� �׷���ī���� ������ �˰� �ֱ� ������ ������ ��� ���� ����޶� ����ü��� �������̽����� �����ؾ��Ѵ�.

	// ���÷��� ��� ����Ʈ�� �Ҵ��� �����Ѵ�.
	delete[] displayModeList;
	displayModeList = 0;

	// ��� ����͸� �Ҵ� �����Ѵ�.
	adapterOutput->Release();
	adapterOutput = 0;

	// ����͸� �Ҵ� �����Ѵ�.
	adapter->Release();
	adapter = 0;

	//���丮 ��ü�� �Ҵ� �����Ѵ�.
	factory->Release();
	factory = 0;

	/////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	// ���� �ý��ۿ��� ���� �ֻ����� �˰� �����Ƿ� DirectX�� �ʱ�ȭ �� �� �ִ�. 
	// ���� ���� �ؾ��� ���� ����ü���� description ����ü�� ä���ִ� ���̴�. 
	// ���� ü���� ������ �������� ����Ʈ���ۿ� ����ۿ� ��ϵȴ�.
	// ���� �������� �� �� �ϳ��� �����۸��� ����ϸ�, �� ���� ����� �׸���, ����Ʈ ���ۿ� �ٲ�ġ��(swap)�Ͽ� ������ ȭ�鿡 �����ְ� �ȴ�. �̰��� ����ü���̴�.
	// ���� ü�� description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// �ϳ��� ����۸��� ����ϵ��� �Ѵ�.
	swapChainDesc.BufferCount = 1;

	// ������� �ʺ�� ���̸� �����Ѵ�.
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;

	// ����۷� �Ϲ����� 32bit�� �����̽��� �����Ѵ�.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �ֻ����� ������ ������� �ֻ��� �����Ѵ�.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ������� �뵵�� �����Ѵ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� �̷���� �������� �ڵ��� �����Ѵ�.
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø��� ����.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ������ ��� �Ǵ� Ǯ��ũ�� ��带 �����Ѵ�.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ��ĵ������ ���İ� ��ĵ���̴��� �������� �������� (unspecified) �����Ѵ�.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ������ ������� ������ �������� �Ѵ�.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��׸� ������� �ʴ´�.
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// ����ü�� ����ü�� �����ϰ� ���� ���� �����̶�� ������ �����ؾ��Ѵ�.
	// �� ������ �츮�� ��� ������ DirectX�� ����� ������ �˷��ش�. 
	// ���� ������ DirectX 11�� �����Ѵ�.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/////////////////////////////////////////////////////////////////////////////////////

	// �� ������ ��ġ�� ���� ���� ü��, Direct3D ��ġ, Driect3D ��ġ ���ؽ�Ʈ�� ���� �� �ִ�.
	// Direct3D�� Direct3D ��ġ ���ؽ�Ʈ�� ������ Direct3D ��ġ�� ����� �ΰ��� �ɰ� ���̴�. ���� �� ��ü�� ��� �ʿ��ϴ�.
	// Direct3D ��ġ�� Driect3D ���ؽ�Ʈ�� ��� Direct3D �Լ����� �������̽��� �Ǳ� ������ �ſ� �߿��ϴ�.

	// ���� ü��, Direct3D ����̽�, Direct3D ����̽� ���ؽ�Ʈ�� �����մϴ�.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ DirectX11�� �����ϴ� �׷���ī�带 ������ ���� �ʴٸ�, ��ġ�� ��ġ ���ؽ�Ʈ�� �����ϴ� �Լ��� ������ ���̴�.
	// �׷��� ���� �κп��� D3D_DRIVER_TYPE_HARDWARE�� D3D_DRIVER_TYPE_REFERNECE�� �ٲ��ָ� �׷��� ī�尡 �ƴ� CPU���� �������� ó���ϰ� �� �� �ִ�
	// �ӵ��� 1/1000 ������ �������� DirectX11�� �����ϴ� �׷���ī�尡 ���� ����鿡�Դ� ����ϱ� ���� ����̴�.
	// ������ ù��° �׷���ī�尡 DirectX 10�� �����ϰ� �ι�° �׷���ī�尡 DirectX 11�� �������� �ʴ� ��Ⱑ ���� ���� �ֵ�.
	// ���� �Ϻ� ���̺긮�� �׷���ī���� ��� ù��° �׷��� ī��� �������� ���� �׷���ī���, �ι�° �׷���ī��� Nvidia�� �׷���ī��� ������ �� �ִ�.
	// �̷� ��쿡 ��� �����ϱ� ���ؼ��� ����� �⺻ �׷��� ī�常�� ������� ���ƾ��ϰ�, ����� ��� ���� ī����� �����Ͽ� ������ ���� ���� �߸´� �׷��� ī��� ��ġ�� ������ �� �ְ� �ؾ��Ѵ�.

	/////////////////////////////////////////////////////////////////////////////////////

	//// backBufferDesc �� ���� �ۼ����� �ʾƵ� ����ü���� �������� ����۸� �������� �װ��� ���� Ÿ�ٺ�� �����ϸ� ������
	//// ����� ������� ������ ���ҽ���� �������⶧���� RTT�� ���� ������ش�. RTT�� ����۴�� ����ü�ΰ� ��������ش�.
	//ZeroMemory(&backBufferDesc, sizeof(backBufferDesc));

	//backBufferDesc.Width = screenWidth;
	//backBufferDesc.Height = screenHeight;
	//backBufferDesc.MipLevels = 1;
	//backBufferDesc.ArraySize = 1;
	//backBufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//backBufferDesc.SampleDesc.Count = 1;
	//backBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//backBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//backBufferDesc.CPUAccessFlags = 0; backBufferDesc.MiscFlags = 0;

	//// RTT�� �����Ѵ�.
	//result = m_device->CreateTexture2D(&backBufferDesc, NULL, &m_renderTargetTexture);
	//if (FAILED(result)) {
	//	return false;
	//}

	// ���� ����ü���� �����Ƿ� ������� �����͸� ������ ����ü�ο� �����������Ѵ�.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		return false;
	}

	// ����ü�ΰ� RTT(�����x)�� �����ϱ� ���ؼ��� Direct3D ��ġ�� ����Ÿ�� �並 RTT(�����x)�� �����Ѵ�. 
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result)) {
		return false;
	}

	// ����� �����͸� ���̻� ������� �����Ƿ� �Ҵ� �����Ѵ�.
	backBufferPtr->Release();
	backBufferPtr = 0;

	//// ���ҽ� �信 ���� discription�� �ۼ��Ѵ�.
	//shaderResourceViewDesc.Format = backBufferDesc.Format;
	//shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	//shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//// ���ҽ��並 �����Ѵ�.
	//result = m_device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	//if (FAILED(result)) {
	//	return false;
	//}

	/////////////////////////////////////////////////////////////////////////////////////

	// �̿� ���� ���� ������ description ����ü�� �ۼ��Ѵ�.
	// ���� ���۸� ������ 3D �������� �츮�� ��������� �ùٸ��� �׷�����. ���� ���ÿ� ���ٽ� ���۵� �� ���� ���ۿ� ������ ���̴�.
	// ���ٽ� ���۴� ��� �������, ������Ʈ�� �׸��� ���� ȿ���� �� �� ���ȴ�.

	// ���� ������ description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ������ description�� �ۼ��Ѵ�.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ���� ���ۿ� ���ٽǹ��۸� �����ϱ� ���ؼ��� Direct3D ��ġ�� ���̹����� �ؽ�ó�� �����Ѵ�.
	// ���⼭�� CreateTexture2D �Լ��� ���۸� �����ϴµ� �� ������ ���� ���� 2D �ؽ�ó�̱� �����̴�.
	// ��������� ���ĵǰ� ������ȭ �� ���Ŀ��� ��·�� 2D ��ǥ�� �ȼ����� �Ǳ� �����̴�. 
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////

	// ���� �ʿ��� ���� ����-���ٽ� ���� description�� �ۼ��ϴ� ���̴�.
	// �̰��� �츮�� Direct3D���� �� �ȼ��� � ���� �׽�Ʈ�� �� ������ ���� �� �ְ� ���ش�. 

	// ���ٽ� ������ description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ������ description�� �ۼ��Ѵ�. 
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ������ �ۼ��� description�� ������ ����-���ٽ� ���� ������ �����.

	// ����-���ٽ� ���¸� �����Ѵ�.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result)) {
		return false;
	}

	// ���º����� ����� �̰��� �����ؾ��ϴµ� ��ġ ���ؽ�Ʈ�� �����ϴ� ���� ��������
	// ����-���ٽ� ���¸� �����Ѵ�.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ���⿡ ���� ���ٽ� ���º��� 2�� description�� �߰��Ѵ�. 
	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;  // ���� ���º����� �ٸ��� �� ���� false�� �ۼ��Ǿ���.
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���¸� �����Ѵ�.
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	// ���� ������ description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// ���İ��� ����� ���� ���¸� ����� ���ؼ� BlendEnable�� true�� �ٲٰ� DestBlend�� D3D11_BLEND_INV_SRC_ALPHA�� �����Ѵ�.
	// �ٸ� �������� �⺻������ �ϴµ� �� ������ DirectX �׷��Ƚ� �������� ��밡���ϴ�.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;


	// �׸��� ���� description�� �̿��Ͽ� ���İ��� ���� ���� ���¸� �����Ѵ�.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result)) {
		return false;
	}

	// ���İ��� ������� �ʴ� ���¸� ����� ���ؼ� BlendEnable�� false�� �����Ѵ�. 
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// �ٲ� description�� ����Ͽ� ���İ��� ���� �������¸� �����Ѵ�. ���� ���� ������ ���� �Ѵµ� �ʿ��� �� ���� ���¸� �����Եȴ�.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////

	// �������� ������ �� ���� ����-���ٽ� ������ �信 ���� description�̴�.
	// �� �۾��� �ؾ� Direct3D�� ���� ���۸� ����-���ٽ� �ؽ��ķ� �ν��Ѵ�.
	// �� ����ü�� ä��� �� �Ŀ� CreateDepthStencilView �Լ��� ȣ���Ͽ� ����-���ٽ� ���� �並 �����Ѵ�.

	// ����-���ٽ� ���� description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ����-���ٽ� ���� description�� �ۼ��մϴ�. 
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ����-���ٽ� �並 �����մϴ�. 
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////

	// ������� �Դٸ� OMSetRenderTarget �Լ��� ȣ���� �� �ִ�.
	// �� �Լ��� ����Ÿ�ٺ�� ����-���ٽ� �並 ��� ������ ���������ο� ���ε� ��Ų��. 
	// �̷��� �Ͽ� ������������ �̿��� �������� ���� �ɶ� �츮�� ������� ����ۿ� ����� �׷����� �ȴ�.
	// ����ۿ� �׷��� ���� ����Ʈ ���ۿ� �ٱ�ġ���Ͽ� ������ ����Ϳ� ����ϴ� ���̴�.

	// ����Ÿ�� ��� ����-���ٽ� ���۸� ���� ��� ���������ο� ���ε��Ѵ�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	/////////////////////////////////////////////////////////////////////////////////////

	// ���� Ÿ���� ������ �� �츮�� ������ �������� ���� �߰����� ����� ������ �� �ִ�.


	// ù��°�� ����� �Ǵ� ���� ������ȭ�� ����(rasterizer state)��. �̰��� ������ ��� �ȼ��� �׷����� ���� ���� ��� �� �� �ְ� ���ش�.
	// �츮�� ȭ���� ���̾������� ���� �׸��ų� ������ �� �޸��� ��� �׸����� �� �� �ִ�.
	// �Ʒ��� �Ȱ��� �������� ������� ���ư��� DriectX �⺻ ������ȭ�� ���°� ������, ���� ������ �ʴ� �̻� �̿� ���� ������� ����.

	// � ������ ��� �׸� ������ �����ϴ� ������ȭ�� description�� �ۼ��մϴ�. 
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0; rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// �ۼ��� description���κ��� ������ȭ�� ���¸� �����մϴ�. 
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result)) {
		return false;
	}

	// ������ȭ�� ���¸� �����մϴ�.
	m_deviceContext->RSSetState(m_rasterState);
	/////////////////////////////////////////////////////////////////////////////////////
	float fieldOfView, screenAspect;
	// ���� ����Ʈ�� �־�� ����Ÿ�� �������� Ŭ������ ������ �� �ִ�.
	// �켱 ����Ʈ�� ������ ��ü ũ��� �����ϰ� �����Ѵ�.

	// �������� ���� ����Ʈ�� �����մϴ�. 
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// ����Ʈ�� �����մϴ�.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// ������ ���� ����� ������ ���ʴ�
	// ��������� 3D�� ȭ���� �ռ� ������� 2D ����Ʈ �������� ��ȯ�ϵ��� �����ִ� ���̴�.
	// �� ����� ���纻�� ����� ���̴����� ����� �׸� �� ����� �� �ֵ��� �ؾ��Ѵ�.

	// ��������� �����Ѵ�.
	fieldOfView = 45.0f;   // ���� �þ߰� 45�� (��/4)
	screenAspect = (float)screenWidth / (float)screenHeight;
	// 3D �������� ���� ���� ��������� �����Ѵ�.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	//XMFLOAT4X4 projectionMatrix;
	//projectionMatrix(0, 0) = 1 / (screenAspect * tan(fieldOfView / 2.0f));
	//projectionMatrix(0, 1) = 0;
	//projectionMatrix(0, 2) = 0;
	//projectionMatrix(0, 3) = 0;
	//projectionMatrix(1, 0) = 0;
	//projectionMatrix(1, 1) = 1 / tan(fieldOfView / 2.0f);
	//projectionMatrix(1, 2) = 0;
	//projectionMatrix(1, 3) = 0;
	//projectionMatrix(2, 0) = 0;
	//projectionMatrix(2, 1) = 0;
	//projectionMatrix(2, 2) = m_depthFar / (m_depthFar - m_depthNear);
	//projectionMatrix(2, 3) = 1;
	//projectionMatrix(3, 0) = 0;
	//projectionMatrix(3, 1) = 0;
	//projectionMatrix(3, 2) = -m_depthNear * m_depthFar / (m_depthFar - m_depthNear);
	//projectionMatrix(3, 3) = 0;

	//m_projectionMatrix = XMLoadFloat4x4(&projectionMatrix);

	// ���� ���� ����̶�� �� �ٸ� ����� �������Ѵ�.
	// �� ����� ������Ʈ���� ��ǥ�� 3D ������ ��ǥ�� ��ȯ�ϴ� �� ���ȴ�.
	// ���� 3���� ���������� ȸ��/�̵�/ũ�� ��ȯ���� ���ȴ�.
	// ó������ �� ����� ���� ��ķ� ����� ���纻�� �����. �� ���纻 ���� ���̴��� ���޵Ǿ� ����� �� �ְ� �Ѵ�.

	// ���� ����� ���� ��ķ� �ʱ�ȭ�Ѵ�.
	m_worldMatrix = XMMatrixIdentity();

	// ���Ⱑ ���� �� ����� �����ϴ� ���̴�. �� ����� ���� ��鿡�� �츮�� ��� ��ġ���� ��� ������ �ٶ󺸰� �ִ°��� ����ϴ� �� �̿�ȴ�.
	// 3D���踦 ī������� �Ѵٸ� ī�޶� ���� ����̶�� ������ �� �ִ�.
	// �� ������ ��Ȯ�ϱ� ������ ������ ������� �ǳʶٰ� ���� ������ ������ ī�޶� Ŭ�������� �ٷ絵������

	// �׸��� Initilize �Լ����� ���������� �ؾ� �� ���� ���� ��������� ����� ���̴�.
	// �� ����� 3D��ü�� �ƴ϶� UI ���� 2D ��ҵ��� �׸��� ���� ���ȴ�. ���߿� 2D �׷��Ȱ� ��Ʈ�� �ٷ�� �������� ����� ���̴�.

	// 2D �������� ���� ���� ���� ����� �����Ѵ�.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, m_depthNear, m_depthFar);
	/////////////////////////////////////////////////////////////////////////////////////

	return true;
}


// Shutdown �Լ��� ���������� Initialize �Լ����� ����ߴ� �����͵��� �����ϰ� �����ϴ� ���� �� ���̴�.
// ������ �� �۾� ���� ����ü���� ������ ���� �ٲٴ� �Լ��� ȣ���Ѵ�.
// ���� ���۾��� ���� �ʰ� Ǯ��ũ�� ���¿��� ����ü���� �����ϸ� ��� ���ܰ� �߻��ϰ� �ȴ�.
// ���� �׷� ���ܸ� ���ϱ� ���� Direct3D�� �����ϱ� ������ ������ ���� ü���� ������ ���� �ٲپ��־���Ѵ�.

void D3DClass::Shutdown() {
	// �����ϱ� ���� �̷��� ������ ���� �ٲ��� ������ ����ü���� �Ҵ� ������ �� ���ܰ� �߻��մϴ�. 
	if (m_swapChain) {
		m_swapChain->SetFullscreenState(false, NULL);
	}
	if (m_alphaEnableBlendingState) {
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}
	if (m_alphaDisableBlendingState) {
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}
	if (m_rasterState) {
		m_rasterState->Release();
		m_rasterState = 0;
	}
	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthStencilState) {
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	if (m_depthDisabledStencilState) {
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}
	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_renderTargetView) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	if (m_deviceContext) {
		m_deviceContext->Release();
		m_deviceContext = 0;
	}
	if (m_device) {
		m_device->Release();
		m_device = 0;
	}
	if (m_swapChain) {
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

// D3DClass���� �� ���� ����� �Լ����� �ִ�. ó�� �ΰ��� BeginScene�� EndScene�̴�.
// BeginScene�� �� �������� ���۸��� 3D ȭ���� �׸��� ������ �� ȣ��ȴ�.
// �� �Լ��� ���۸� �� ������ �ʱ�ȭ�ϰ� �������� �̷�������� �غ��Ѵ�.
// EndScene�Լ��� �� �������� �������� ���� ü�ο��� ����ۿ� �׸� 3Dȭ���� ǥ���ϵ��� �ϰ� �Ѵ�.

void D3DClass::BeginScene(float red, float green, float blue, float alpha) {
	float color[4];
	// ���۸� � �������� ���� ������ �����մϴ�. 
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// ������� ������ ����ϴ�.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// ���� ���۸� ����ϴ�. 
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene() {
	// �������� �Ϸ�Ǿ����Ƿ� ������� ������ ȭ�鿡 ǥ���մϴ�. 
	if (m_vsync_enabled) {
		// ���ΰ�ħ ������ �����մϴ�. 
		m_swapChain->Present(1, 0);
	}
	else {
		// ������ �� ������ ǥ���մϴ�. 
		m_swapChain->Present(0, 0);
	}
	return;
}


// ���� �Լ��� �ܼ��� Direct3D ����̽��� Driect3D ���ؽ�Ʈ�� �����͸� �������� �Լ���.
// �� ����� �Լ����� �����ӿ�ũ���� ���� ȣ��ɰ��̴�.

ID3D11Device* D3DClass::GetDevice() {
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext() {
	return m_deviceContext;
}

// ���� �� ����� �Լ��� ����, ����, ���� ���� ����� ��ȯ�Ѵ�.
// ��κ��� ���̴������� �� ��ĵ��� �ʿ��ϱ� ������ �̸� ������ �ս��� ����� �ʿ��ϴ�.

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix) {
	orthoMatrix = m_orthoMatrix;
	return;
}


// ������ ����� �Լ��� �׷���ī���� �̸��� ��밡���� �޸��� ���� ��ȯ�Ѵ�.
// �׷��� ī���� �̸��� �޸��� ũ�⸦ �ƴ� ���� �ٸ� �������� ������ϴ� �� ������ �ȴ�.
void D3DClass::GetVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

// �Ʒ��� �� �Լ��� Z ���۸� �Ѱ� ���� ������ �Ѵ�.

void D3DClass::TurnZBufferOn() {
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff() {
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

// �� �Լ��� TurnOnAlphaBlending�� OMSetBlendState �Լ��� m_alphaEnableBlendingState ������ ����Ͽ� ���ĺ����� �Ѵ� ���� �Ѵ�.
void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}
// �� ���� �Լ��� TurnOffAlphaBlending�� ���� OMSetBlendState �Լ��� m_alphaDisableBlendingState ������ ����Ͽ� ���ĺ����� ���� ���� �Ѵ�.
void D3DClass::TurnOffAlphaBlending() {
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

ID3D11RenderTargetView* const* D3DClass::GetRenderTargetView() {
	return &m_renderTargetView;
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView() {
	return m_depthStencilView;
}

// SetBackBufferRenderTarget �Լ��� �� Ŭ������ ����۸� ������ ���� Ÿ������ �����Ѵ�.
// ���� RTT�� ������ ���� Ÿ���� �ٽ� ����۷� ������ ���ȴ�. 
void D3DClass::SetBackBufferRenderTarget() {
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return;
}

void D3DClass::ResetViewport() {
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}