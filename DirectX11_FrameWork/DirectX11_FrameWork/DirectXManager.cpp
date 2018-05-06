#include "stdafx.h"
#include "DirectXManager.h"



DirectXManager::DirectXManager()
{
}


DirectXManager::~DirectXManager()
{
}

bool DirectXManager::Initialize(int InScreenWidth, int InScreenHeight, bool InVsync, HWND InHwnd, bool InFullScreen, float InScreenDepth, float InScreenNear)
{
	// 수직 동기화 상태를 저장 
	m_VsyncEnabled = InVsync;

	// DirectX 그래픽 인터페이스 팩토리 생성
	IDXGIFactory* Factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory)))
	{
		return false;
	}

	// 팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스 어뎁터를 생성합니다
	IDXGIAdapter* Adapter = nullptr;
	if (FAILED(Factory->EnumAdapters(0, &Adapter)))
	{
		return false;
	}

	// 출력(모니터)에 대한 첫번째 어뎁터를 지정합니다.
	IDXGIOutput* AdapterOutput = nullptr;
	if (FAILED(Adapter->EnumOutputs(0, &AdapterOutput)))
	{
		return false;
	}

	// 출력 (모니터)에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드 수를 가져옵니다
	unsigned int NumModes = 0;
	if (FAILED(AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, NULL)))
	{
		return false;
	}

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성합니다
	DXGI_MODE_DESC* DisplayModeList = new DXGI_MODE_DESC[NumModes];
	if (!DisplayModeList)
	{
		return false;
	}

	// 이제 디스플레이 모드에 대한 리스트를 채웁니다
	if (FAILED(AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, DisplayModeList)))
	{
		return false;
	}

	// 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드를 찾습니다.
	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장합니다.
	unsigned int Numerator = 0;
	unsigned int Denominator = 0;
	for (unsigned int i = 0; i < NumModes; i++)
	{
		if (DisplayModeList[i].Width == static_cast<unsigned int>(InScreenWidth))
		{
			if (DisplayModeList[i].Height == static_cast<unsigned int>(InScreenHeight))
			{
				Numerator = DisplayModeList[i].RefreshRate.Numerator;
				Denominator = DisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 비디오카드의 구조체를 얻습니다
	DXGI_ADAPTER_DESC AdapterDesc;
	if (FAILED(Adapter->GetDesc(&AdapterDesc)))
	{
		return false;
	}

	// 비디오카드 메모리 용량 단위를 메가바이트 단위로 저장합니다
	m_VideoCardMemory = static_cast<int>(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오카드의 이름을 저장합니다
	size_t StringLength = 0;
	if (wcstombs_s(&StringLength, m_VideoCardDescription, 128, AdapterDesc.Description, 128) != 0)
	{
		return false;
	}
	// 디스플레이 모드 리스트를 해제합니다
	delete[] DisplayModeList;
	DisplayModeList = 0;

	// 출력 어뎁터를 해제합니다
	AdapterOutput->Release();
	AdapterOutput = nullptr;

	// 어뎁터를 해제합니다
	Adapter->Release();
	Adapter = nullptr;

	// 팩토리 객체를 해제합니다
	Factory->Release();
	Factory = nullptr;

	// 스왑체인 구조체를 초기화합니다
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// 백버퍼를 1개만 사용하도록 지정합니다
	SwapChainDesc.BufferCount = 1;

	// 백버퍼의 넓이와 높이를 지정합니다
	SwapChainDesc.BufferDesc.Width  = InScreenWidth;
	SwapChainDesc.BufferDesc.Height = InScreenHeight;

	// 32bit 서페이스를 설정합니다
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼의 새로고침 비율을 설정합니다
	if (m_VsyncEnabled)
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = Numerator;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = Denominator;
	}
	else
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼의 사용용도를 지정합니다
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 랜더링에 사용될 윈도우 핸들을 지정합니다
	SwapChainDesc.OutputWindow = InHwnd;

	// 멀티샘플링을 끕니다
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;

	// 창모드 or 풀스크린 모드를 설정합니다
	if (InFullScreen)
	{
		SwapChainDesc.Windowed = false;
	}
	else
	{
		SwapChainDesc.Windowed = true;
	}

	// 스캔 라인 순서 및 크기를 지정하지 않음으로 설정합니다.
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 다음 백버퍼를 비우도록 지정합니다
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그를 사용하지 않습니다
	SwapChainDesc.Flags = 0;

	// 피처레벨을 DirectX 11 로 설정합니다
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑 체인, Direct3D 장치 및 Direct3D 장치 컨텍스트를 만듭니다.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevel, 1,
		D3D11_SDK_VERSION, &SwapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext)))
	{
		return false;
	}

	// 백버퍼 포인터를 얻어옵니다
	ID3D11Texture2D* BackBufferPtr = nullptr;
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr)))
	{
		return false;
	}

	// 백 버퍼 포인터로 렌더 타겟 뷰를 생성한다.
	if (FAILED(m_Device->CreateRenderTargetView(BackBufferPtr, NULL, &m_RenderTargetView)))
	{
		return false;
	}

	// 백버퍼 포인터를 해제합니다
	BackBufferPtr->Release();
	BackBufferPtr = nullptr;

	// 깊이 버퍼 구조체를 초기화합니다
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// 깊이 버퍼 구조체를 작성합니다
	DepthBufferDesc.Width = InScreenWidth;
	DepthBufferDesc.Height = InScreenHeight;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.SampleDesc.Count = 1;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	// 설정된 깊이버퍼 구조체를 사용하여 깊이 버퍼 텍스쳐를 생성합니다
	if (FAILED(m_Device->CreateTexture2D(&DepthBufferDesc, NULL, &m_DepthStencilBuffer)))
	{
		return false;
	}

	// 스텐실 상태 구조체를 초기화합니다
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));

	// 스텐실 상태 구조체를 작성합니다
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	DepthStencilDesc.StencilEnable = true;
	DepthStencilDesc.StencilReadMask = 0xFF;
	DepthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀 정면의 스텐실 설정입니다
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀 뒷면의 스텐실 설정입니다
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태를 생성합니다
	if (FAILED(m_Device->CreateDepthStencilState(&DepthStencilDesc, &m_DepthStencilState)))
	{
		return false;
	}

	// 깊이 스텐실 상태를 설정합니다
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	// 깊이 스텐실 뷰의 구조체를 초기화합니다
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체를 설정합니다
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 생성합니다
	if (FAILED(m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &DepthStencilViewDesc, &m_DepthStencilView)))
	{
		return false;
	}

	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩합니다
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	// 그려지는 폴리곤과 방법을 결정할 래스터 구조체를 설정합니다
	D3D11_RASTERIZER_DESC RasterDesc;
	RasterDesc.AntialiasedLineEnable = false;
	RasterDesc.CullMode = D3D11_CULL_BACK;
	RasterDesc.DepthBias = 0;
	RasterDesc.DepthBiasClamp = 0.0f;
	RasterDesc.DepthClipEnable = true;
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.FrontCounterClockwise = false;
	RasterDesc.MultisampleEnable = false;
	RasterDesc.ScissorEnable = false;
	RasterDesc.SlopeScaledDepthBias = 0.0f;

	// 방금 작성한 구조체에서 래스터 라이저 상태를 만듭니다
	if (FAILED(m_Device->CreateRasterizerState(&RasterDesc, &m_RasterState)))
	{
		return false;
	}

	// 이제 래스터 라이저 상태를 설정합니다
	m_DeviceContext->RSSetState(m_RasterState);

	// 렌더링을 위해 뷰포트를 설정합니다
	D3D11_VIEWPORT Viewport;
	Viewport.Width	= static_cast<float>(InScreenWidth);
	Viewport.Height = static_cast<float>(InScreenHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;

	// 뷰포트를 생성합니다
	m_DeviceContext->RSSetViewports(1, &Viewport);

	// 투영 행렬을 설정합니다
	float fieldOfView = XM_PI / 4.0f;
	float screenAspect = (float)InScreenWidth / (float)InScreenHeight;

	// 3D 렌더링을위한 투영 행렬을 만듭니다
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, InScreenNear, InScreenDepth);

	// 세계 행렬을 항등 행렬로 초기화합니다
	m_WorldMatrix = XMMatrixIdentity();

	// 2D 렌더링을위한 직교 투영 행렬을 만듭니다
	m_OrthoMatrix = XMMatrixOrthographicLH((float)InScreenWidth, (float)InScreenHeight, InScreenNear, InScreenDepth);

	// 이제 2D 렌더링을 위한 Z버퍼를 끄는 두 번째 깊이 스텐실 상태를 만듬 유일한 차이점은
	// DepthEnable을 false로 설정하면 다른 모든 매개변수는 다른 깊이 스탠실 상태와 동일함.
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
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

	// 장치를 사용하여 상태를 만듬
	if (FAILED(m_Device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState)))
	{
		return false;
	}

	return true;
}

void DirectXManager::Shutdown()
{
	// 종료 전 윈도우 모드로 설정하지 않으면 스왑 체인을 해제 할 때 예외가 발생합니다.
	if (m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_RasterState)
	{
		m_RasterState->Release();
		m_RasterState = 0;
	}

	if (m_RenderTargetView)
	{
		m_RenderTargetView->Release();
		m_RenderTargetView = 0;
	}

	if (m_DepthStencilState)
	{
		m_DepthStencilState->Release();
		m_DepthStencilState = 0;
	}

	if (m_DepthStencilBuffer)
	{
		m_DepthStencilBuffer->Release();
		m_DepthStencilBuffer = 0;
	}

	if (m_RenderTargetView)
	{
		m_RenderTargetView->Release();
		m_RenderTargetView = 0;
	}

	if (m_DeviceContext)
	{
		m_DeviceContext->Release();
		m_DeviceContext = 0;
	}

	if (m_Device)
	{
		m_Device->Release();
		m_Device = 0;
	}

	if (m_SwapChain)
	{
		m_SwapChain->Release();
		m_SwapChain = 0;
	}
}

void DirectXManager::BeginScene(float InRed, float InGreen, float InBlue, float InAlpha)
{
	// 버퍼를 지울 색을 설정합니다
	float color[4] = { InRed, InGreen, InBlue, InAlpha };

	// 백버퍼를 지웁니다
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);

	// 깊이 버퍼를 지웁니다
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectXManager::EndScene()
{
	// 렌더링이 완료되었으므로 화면에 백 버퍼를 표시합니다.
	if (m_VsyncEnabled)
	{
		// 화면 새로 고침 비율을 고정합니다.
		m_SwapChain->Present(1, 0);
	}
	else
	{
		// 가능한 빠르게 출력합니다
		m_SwapChain->Present(0, 0);
	}
}

ID3D11Device * DirectXManager::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext * DirectXManager::GetDeviceContext()
{
	return m_DeviceContext;
}

void DirectXManager::GetProjectionMatrix(XMMATRIX& InProjectionMatrix)
{
	InProjectionMatrix = m_ProjectionMatrix;
}

void DirectXManager::GetWorldMatrix(XMMATRIX& InWorldMatrix)
{
	InWorldMatrix = m_WorldMatrix;
}

void DirectXManager::GetOrthoMatrix(XMMATRIX& InOrthoMatrix)
{
	InOrthoMatrix = m_OrthoMatrix;
}

void DirectXManager::GetVideoCardInfo(char* InCardName, int& InMemory)
{
	strcpy_s(InCardName, 128, m_VideoCardDescription);
	InMemory = m_VideoCardMemory;
}

void DirectXManager::TurnZBufferOn()
{
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);
}


void DirectXManager::TurnZBufferOff()
{
	m_DeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}