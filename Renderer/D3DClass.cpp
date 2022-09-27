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
	//screenWidth : 윈도우 가로 너비
	//screenHeight : 윈도우 세로 높이
	//vsync : Direct3D 렌더링을 모니터의 주사율에 맞출 것인지, 아니면 가능한 한 빠르게 다시 그릴 것인지 지정한다.
	//hwnd : 변수로 만들어진 윈도우에 대한 핸들
	//fullscreen : 어플리케이션이 윈도우 모드에서 동작할지 전체 화면 모드에서 동작할지 알려준다.
	//screenDepth : 화면에 그려질 깊이값 -> 행렬들을 구할때 사용된다. 
	//screenNear : 화면에 그려질 깊이값 -> 행렬들을 구할때 사용된다. 

	m_depthNear = screenNear;
	m_depthFar = screenDepth;

	HRESULT result;

	//그래픽카드/모니터 주사율 구할 때 사용되는 변수
	IDXGIFactory* factory;   // 그래픽카드 인터페이스 팩토리
	IDXGIAdapter* adapter;   // 그래픽카드 인터페이스 어댑터
	IDXGIOutput* adapterOutput;   // 어댑터의 출력
	unsigned int numModes, i, numerator, denominator, stringLength;   // 모드 수, , 주사율 분자, 주사율 분모, 그래픽카드 이름
	DXGI_MODE_DESC* displayModeList;   // 모니터와 그래픽카드 조합 리스트
	DXGI_ADAPTER_DESC adapterDesc;   // 어댑터 description (그래픽카드에서 가져옴)
	int error;

	//스왑체인 description 구조체에서 사용되는 변수
	DXGI_SWAP_CHAIN_DESC swapChainDesc;   // 스왑체인
	D3D_FEATURE_LEVEL featureLevel; // 스왑체인 피처레벨

	//스왑체인과 백버퍼를 연결시킬대 사용하는 변수 
	ID3D11Texture2D* backBufferPtr; // 백버퍼 포인터
	D3D11_TEXTURE2D_DESC backBufferDesc;  // 백버퍼에 대한 description
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc; // 백버퍼의 뷰자원 description
	D3D11_TEXTURE2D_DESC depthBufferDesc;   //깊이 버퍼 description

	//깊이 버퍼의 description 구조체에서 사용되는 변수
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;   // 깊이-스텐실 description
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;  //깊이-스텐실 뷰 description
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// 블렌딩 상태를 생성하기위한 description 변수
	D3D11_BLEND_DESC blendStateDescription;


	// vsync(수직동기화) 설정을 저장한다.
	// vsync 값으로 true값이 들어오면 시스템에서 정한 주사율(60Hz)로 동작한다. false가 들어오면 1초에 최대한 빠르게 화면에 그려나간다.
	// 현재는 VSYNC_ENABLED 값을 넣어 모니터 주사율과 동일하게 한다. 
	m_vsync_enabled = vsync;

	// Direct3D를 초기화하기 전에 그래픽카드/모니터의 주사율(새로고침 비율)을 알아야한다.
	// 또 그래픽카드의 이름과, 그래픽카드의 사용가능한 메모르 크기도 알아야한다. 
	// 각 컴퓨터마다 그 값들은 조금씩 다르기 때문에 컴퓨터에게 해당정보를 어댑터를 통해서조회해야한다.
	// 이 비율의 분자 분모값을 조회한 뒤 설정 중에 DirectX에게 그 값을 알려주면 적절한 주사율을 계산한다.
	// 만약 이 작업을 하지 않는다면 기본값으로 맞춰지고 DrirectX 화면을 표시할때 버퍼 플립을 사용하지 않고 blit을 사용하여 성능이 저하되고, 디버그 출력에 거슬리는 메세지를 남긴다.

	/////////////////////////////////////////////////////////////////////////////////////
	//그래픽카드/모니터 주사율, 그래픽카드 이름, 그래픽카드 사용 가능한 메모리 가져오기//
	/////////////////////////////////////////////////////////////////////////////////////
	// DirectX 그래픽 인터페이스 팩토리를 만든다.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		return false;
	}

	// 팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스에 대한 어댑터를 만든다.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		return false;
	}

	// 출력(모니티)에 대한 첫번째 어댑터를 나열한다.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 모드의 개수를 구한다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성한다.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return false;
	}

	// 디스플레이 모드에 대한 리스트 구조를 채워넣는다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	// 이제 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드를 찾는다.
	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장한다.
	for (i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 이렇게 주사율의 분자와 분모를 가지게 되면, 그래픽 카드의 이름과 사용 가능한 그래픽 카드 메모리의 크기를 어댑터를 이용하여 받아온다.
	// 어댑터(그래픽카드)의 description을 가져온다.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	// 현재 그래픽카드의 메모리 용량을 메가바이트 단위로 저장한다.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 그래픽 카드의 이름을 char형 문자열 배열로 바꾼 뒤 저장한다.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	//이제 저장된 주사율 비율의 분자, 분모값과 그래픽카드의 정보를 알고 있기 때문에 정보를 얻기 위해 사용햇떤 구조체들과 인터페이스들을 해제해야한다.

	// 디스플레이 모드 리스트의 할당을 해제한다.
	delete[] displayModeList;
	displayModeList = 0;

	// 출력 어댑터를 할당 해제한다.
	adapterOutput->Release();
	adapterOutput = 0;

	// 어댑터를 할당 해제한다.
	adapter->Release();
	adapter = 0;

	//팩토리 객체를 할당 해제한다.
	factory->Release();
	factory = 0;

	/////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	// 현재 시스템에서 얻어온 주사율을 알고 있으므로 DirectX를 초기화 할 수 있다. 
	// 가장 먼저 해야할 일은 스왑체인의 description 구조체를 채워넣는 일이다. 
	// 스왑 체인은 실제로 렌더링은 프론트버퍼와 백버퍼에 기록된다.
	// 보통 렌더링을 할 때 하나의 벡버퍼만을 사용하며, 그 위에 장면을 그린뒤, 프론트 버퍼와 바꿔치기(swap)하여 유저의 화면에 보여주게 된다. 이것이 스왑체인이다.
	// 스왑 체인 description을 초기화한다.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 하나의 백버퍼만을 사용하도록 한다.
	swapChainDesc.BufferCount = 1;

	// 백버퍼의 너비와 높이를 설정한다.
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;

	// 백버퍼로 일반적인 32bit의 서페이스를 지정한다.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 주사율을 가지고 백버퍼의 주사을 설정한다.
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

	// 백버퍼의 용도를 설정한다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링이 이루어질 윈도우의 핸들을 설정한다.
	swapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링을 끈다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 윈도우 모드 또는 풀스크린 모드를 설정한다.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 스캔라인의 정렬과 스캔라이닝을 지정되지 않음으로 (unspecified) 설정한다.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 이후의 백버퍼의 내용을 버리도록 한다.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그를 사용하지 않는다.
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// 스왑체인 구조체를 설정하고 나서 피쳐 레벨이라는 변수도 설정해야한다.
	// 이 변수는 우리가 어느 버전의 DirectX를 사용할 것이진 알려준다. 
	// 피쳐 레벨을 DirectX 11로 설정한다.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/////////////////////////////////////////////////////////////////////////////////////

	// 이 과정을 거치면 이제 스왑 체인, Direct3D 장치, Driect3D 장치 컨텍스트를 만들 수 있다.
	// Direct3D와 Direct3D 장치 컨텍스트는 기존의 Direct3D 장치의 기능을 두개로 쪼갠 것이다. 따라서 두 객체가 모두 필요하다.
	// Direct3D 장치과 Driect3D 컨텍스트는 모든 Direct3D 함수들의 인터페이스가 되기 때문에 매우 중요하다.

	// 스왑 체인, Direct3D 디바이스, Direct3D 디바이스 컨텍스트를 생성합니다.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 만약 유저가 DirectX11을 지원하는 그래픽카드를 가지고 있지 않다면, 장치와 장치 컨텍스트를 생성하는 함수가 실패할 것이다.
	// 그러면 위의 부분에서 D3D_DRIVER_TYPE_HARDWARE를 D3D_DRIVER_TYPE_REFERNECE로 바꿔주면 그래픽 카드가 아닌 CPU에서 렌더링을 처리하게 할 수 있다
	// 속도는 1/1000 정도로 느리지만 DirectX11을 지원하는 그래픽카드가 없는 사람들에게는 사용하기 좋은 기능이다.
	// 가끔씩 첫번째 그래픽카드가 DirectX 10을 지원하고 두번째 그래픽카드가 DirectX 11을 지원하지 않는 기기가 있을 수도 있따.
	// 또한 일부 하이브리드 그래픽카드의 경우 첫번째 그래픽 카드는 저전력의 인텔 그래픽카드고, 두번째 그래픽카드는 Nvidia의 그래픽카드로 동작할 수 있다.
	// 이런 경우에 모두 대응하기 위해서는 절대로 기본 그래픽 카드만을 사용하지 말아야하고, 기기의 모든 비디오 카드들을 나열하여 유저가 직접 가장 잘맞는 그래픽 카드로 장치를 생성할 수 있게 해야한다.

	/////////////////////////////////////////////////////////////////////////////////////

	//// backBufferDesc 은 굳이 작성하지 않아도 스왑체인을 생성한후 백버퍼를 가져오고 그것을 렌더 타겟뷰랑 연결하면 되지만
	//// 현재는 백버퍼의 내용을 리소스뷰로 내보내기때문에 RTT를 따로 만들어준다. RTT는 백버퍼대신 스왑체인과 연결시켜준다.
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

	//// RTT를 생성한다.
	//result = m_device->CreateTexture2D(&backBufferDesc, NULL, &m_renderTargetTexture);
	//if (FAILED(result)) {
	//	return false;
	//}

	// 이제 스왑체인이 있으므로 백버퍼의 포인터를 가져와 스왑체인에 연결시켜줘야한다.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		return false;
	}

	// 스왑체인과 RTT(백버퍼x)를 연결하기 위해서는 Direct3D 장치의 렌더타겟 뷰를 RTT(백버퍼x)로 설정한다. 
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result)) {
		return false;
	}

	// 백버퍼 포인터를 더이상 사용하지 않으므로 할당 해제한다.
	backBufferPtr->Release();
	backBufferPtr = 0;

	//// 리소스 뷰에 대한 discription을 작성한다.
	//shaderResourceViewDesc.Format = backBufferDesc.Format;
	//shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	//shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//// 리소스뷰를 생성한다.
	//result = m_device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	//if (FAILED(result)) {
	//	return false;
	//}

	/////////////////////////////////////////////////////////////////////////////////////

	// 이에 더해 깊이 버퍼의 description 구조체도 작성한다.
	// 깊이 버퍼를 만들어야 3D 공간에서 우리의 폴리곤들이 올바르게 그려진다. 또한 동시에 스텐실 버퍼도 이 깊이 버퍼에 연결할 것이다.
	// 스텐실 버퍼는 모션 블러라던가, 볼류메트릭 그림자 등의 효과를 낼 때 사용된다.

	// 깊이 버퍼의 description을 초기화한다.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 description을 작성한다.
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

	// 깊이 버퍼와 스텐실버퍼를 연결하기 위해서는 Direct3D 장치의 깊이버퍼의 텍스처를 생성한다.
	// 여기서는 CreateTexture2D 함수로 버퍼를 생성하는데 그 이유는 버퍼 역시 2D 텍스처이기 때문이다.
	// 폴리곤들이 정렬되고 래스터화 된 이후에는 어쨌든 2D 좌표의 픽셀들이 되기 때문이다. 
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////

	// 이제 필요한 것은 깊이-스텐실 상태 description을 작성하는 것이다.
	// 이것은 우리가 Direct3D에서 각 픽셀에 어떤 깊이 테스트를 할 것인지 정할 수 있게 해준다. 

	// 스텐실 상태의 description을 초기화한다.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태의 description을 작성한다. 
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

	// 위에서 작성한 description을 가지고 깊이-스텐실 상태 변수를 만든다.

	// 깊이-스텐실 상태를 생성한다.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result)) {
		return false;
	}

	// 상태변수를 만들면 이것을 적용해야하는데 장치 컨텍스트에 적용하는 것을 주의하자
	// 깊이-스텐실 상태를 설정한다.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 여기에 깊이 스텐실 상태변수 2의 description을 추가한다. 
	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;  // 위의 상태변수와 다르게 이 값이 false로 작성되었다.
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

	// 깊이 스텐실 상태를 생성한다.
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	// 블렌딩 상태의 description을 초기화한다.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// 알파값이 적용된 블렌딩 상태를 만들기 위해서 BlendEnable를 true로 바꾸고 DestBlend를 D3D11_BLEND_INV_SRC_ALPHA로 설정한다.
	// 다른 설정들은 기본값으로 하는데 이 값들은 DirectX 그래픽스 문서에서 사용가능하다.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;


	// 그리고 나서 description을 이용하여 알파값이 켜진 블렌딩 상태를 생성한다.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result)) {
		return false;
	}

	// 알파값을 사용하지 않는 상태를 만들기 위해서 BlendEnable을 false로 설정한다. 
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// 바뀐 description을 사용하여 알파값이 꺼진 블렌딩상태를 생성한다. 이제 알파 블렌딩을 끄고 켜는데 필요한 두 블렌딩 상태를 가지게된다.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////

	// 다음으로 만들어야 할 것은 깊이-스텐실 버퍼의 뷰에 대한 description이다.
	// 이 작업을 해야 Direct3D가 깊이 버퍼를 깊이-스텐실 텍스쳐로 인식한다.
	// 이 구조체를 채우고 난 후에 CreateDepthStencilView 함수를 호출하여 깊이-스텐실 버퍼 뷰를 생성한다.

	// 깊이-스텐실 뷰의 description을 초기화한다.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이-스텐실 뷰의 description을 작성합니다. 
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이-스텐실 뷰를 생성합니다. 
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) {
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////

	// 여기까지 왔다면 OMSetRenderTarget 함수를 호출할 수 있다.
	// 이 함수는 렌더타겟뷰와 깊이-스텐실 뷰를 출력 렌더링 파이프라인에 바인딩 시킨다. 
	// 이렇게 하여 파이프라인을 이용한 렌더링이 수행 될때 우리가 만들었던 백버퍼에 장면이 그려지게 된다.
	// 백버퍼에 그려진 것을 프론트 버퍼와 바궈치기하여 유저의 모니터에 출력하는 것이다.

	// 렌더타겟 뷰와 깊이-스텐실 버퍼를 각각 출력 파이프라인에 바인딩한다.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	/////////////////////////////////////////////////////////////////////////////////////

	// 렌더 타켓이 설정된 뒤 우리는 앞으로 예제들을 위한 추가적인 기능을 설정할 수 있다.


	// 첫번째로 만들게 되는 것은 레스터화기 상태(rasterizer state)다. 이것은 도형이 어떻게 픽셀로 그려지는 지에 대한 제어를 할 수 있게 해준다.
	// 우리의 화면을 와이어프레임 모드로 그리거나 도형의 앞 뒷면을 모두 그리도록 할 수 있다.
	// 아래와 똑같은 설정으로 만들어져 돌아가는 DriectX 기본 래스터화기 상태가 있지만, 직접 만들지 않는 이상 이에 대한 제어권은 없다.

	// 어떤 도형을 어떻게 그릴 것인지 결정하는 래스터화기 description을 작성합니다. 
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

	// 작성한 description으로부터 래스터화기 상태를 생성합니다. 
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result)) {
		return false;
	}

	// 래스터화기 상태를 설정합니다.
	m_deviceContext->RSSetState(m_rasterState);
	/////////////////////////////////////////////////////////////////////////////////////
	float fieldOfView, screenAspect;
	// 물론 뷰포트도 있어야 렌더타겟 공간에서 클리핑을 수행할 수 있다.
	// 우선 뷰포트를 윈도우 전체 크기와 동일하게 설정한다.

	// 렌더링을 위한 뷰포트를 설정합니다. 
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// 뷰포트를 생성합니다.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// 이제는 투영 행렬을 생성할 차례다
	// 투영행렬을 3D의 화면을 앞서 만들었떤 2D 뷰포트 공간으로 변환하도록 도와주는 것이다.
	// 이 행렬의 복사본을 만들어 셰이더에서 장면을 그릴 때 사용할 수 있도록 해야한다.

	// 투영행렬을 설정한다.
	fieldOfView = 45.0f;   // 수직 시야각 45도 (ㅠ/4)
	screenAspect = (float)screenWidth / (float)screenHeight;
	// 3D 렌더링을 위한 원근 투영행렬을 생성한다.
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

	// 또한 월드 행렬이라는 또 다른 행렬을 만들어야한다.
	// 이 행렬은 오브젝트들의 좌표를 3D 세계의 좌표로 변환하는 데 사용된다.
	// 또한 3차원 공간에서의 회전/이동/크기 변환에도 사용된다.
	// 처음에는 이 행렬을 단위 행렬로 만들고 복사본을 만든다. 이 복사본 역시 셰이더에 전달되어 사용할 수 있게 한다.

	// 월드 행렬을 단위 행렬로 초기화한다.
	m_worldMatrix = XMMatrixIdentity();

	// 여기가 보통 뷰 행렬을 생성하는 곳이다. 뷰 행렬은 현재 장면에서 우리가 어느 위치에서 어느 방향을 바라보고 있는가를 계산하는 데 이용된다.
	// 3D세계를 카마레라고 한다면 카메라에 대한 행렬이라고 생각할 수 있다.
	// 그 목적이 명확하기 때문에 지금은 뷰행렬을 건너뛰고 나중 예제에 나오는 카메라 클래스에서 다루도록하자

	// 그리고 Initilize 함수에서 마지막으로 해야 할 것은 직교 투영행렬을 만드는 것이다.
	// 이 행렬은 3D객체가 아니라 UI 같은 2D 요소들을 그리기 위해 사용된다. 나중에 2D 그래픽과 폰트를 다루는 예제에서 사용할 것이다.

	// 2D 렌더링에 사용될 직교 투영 행렬을 생성한다.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, m_depthNear, m_depthFar);
	/////////////////////////////////////////////////////////////////////////////////////

	return true;
}


// Shutdown 함수는 직관적으로 Initialize 함수에서 사용했던 포인터들을 해제하고 정리하는 일을 할 것이다.
// 하지만 이 작업 전에 스왑체인을 윈도우 모드로 바꾸는 함수를 호출한다.
// 만일 이작업을 하지 않고 풀스크린 상태에서 스왑체인을 해제하면 몇몇 예외가 발생하게 된다.
// 따라서 그런 예외를 피하기 위해 Direct3D를 종료하기 전에는 언제나 스왑 체인을 윈도우 모드로 바꾸어주어야한다.

void D3DClass::Shutdown() {
	// 종료하기 전에 이렇게 윈도우 모드로 바꾸지 않으면 스왑체인을 할당 해제할 때 예외가 발생합니다. 
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

// D3DClass에는 몇 가지 도우미 함수들이 있다. 처음 두개는 BeginScene과 EndScene이다.
// BeginScene은 매 프레임의 시작마다 3D 화면을 그리기 시작할 때 호출된다.
// 이 함수는 버퍼를 빈 값으로 초기화하고 렌더링이 이루어지도록 준비한다.
// EndScene함수는 매 프레임의 마지막에 스왑 체인에게 백버퍼에 그린 3D화면을 표시하도록 하게 한다.

void D3DClass::BeginScene(float red, float green, float blue, float alpha) {
	float color[4];
	// 버퍼를 어떤 색상으로 지울 것인지 설정합니다. 
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 백버퍼의 내용을 지웁니다.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼를 지웁니다. 
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene() {
	// 렌더링이 완료되었으므로 백버퍼의 내용을 화면에 표시합니다. 
	if (m_vsync_enabled) {
		// 새로고침 비율을 고정합니다. 
		m_swapChain->Present(1, 0);
	}
	else {
		// 가능한 한 빠르게 표시합니다. 
		m_swapChain->Present(0, 0);
	}
	return;
}


// 다음 함수는 단순히 Direct3D 디바이스와 Driect3D 컨텍스트의 포인터를 가져오는 함수다.
// 이 도우미 함수들은 프레임워크에서 종종 호출될것이다.

ID3D11Device* D3DClass::GetDevice() {
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext() {
	return m_deviceContext;
}

// 다음 세 도우미 함수는 투영, 월드, 직교 투영 행렬을 반환한다.
// 대부분의 셰이더에서는 이 행렬들이 필요하기 때문에 이를 가져올 손쉬운 방법이 필요하다.

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix) {
	orthoMatrix = m_orthoMatrix;
	return;
}


// 마지막 도우미 함수는 그래픽카드의 이름과 사용가능한 메모리의 양을 반환한다.
// 그래픽 카드의 이름과 메모리의 크기를 아는 것은 다른 설정에서 디버깅하는 데 도움이 된다.
void D3DClass::GetVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

// 아래의 두 함수는 Z 버퍼를 켜고 끄는 역할을 한다.

void D3DClass::TurnZBufferOn() {
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff() {
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

// 새 함수인 TurnOnAlphaBlending은 OMSetBlendState 함수와 m_alphaEnableBlendingState 변수를 사용하여 알파블렌딩을 켜는 일을 한다.
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
// 그 다음 함수인 TurnOffAlphaBlending도 역시 OMSetBlendState 함수와 m_alphaDisableBlendingState 변수를 사용하여 알파블렌딩을 끄는 일을 한다.
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

// SetBackBufferRenderTarget 함수는 이 클래스의 백버퍼를 현재의 렌더 타겟으로 설정한다.
// 보통 RTT가 끝나고 렌더 타겟을 다시 백버퍼로 돌릴때 사용된다. 
void D3DClass::SetBackBufferRenderTarget() {
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return;
}

void D3DClass::ResetViewport() {
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}