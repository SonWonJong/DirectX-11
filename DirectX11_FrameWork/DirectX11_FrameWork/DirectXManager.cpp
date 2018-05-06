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
	// ���� ����ȭ ���¸� ���� 
	m_VsyncEnabled = InVsync;

	// DirectX �׷��� �������̽� ���丮 ����
	IDXGIFactory* Factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory)))
	{
		return false;
	}

	// ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽� ��͸� �����մϴ�
	IDXGIAdapter* Adapter = nullptr;
	if (FAILED(Factory->EnumAdapters(0, &Adapter)))
	{
		return false;
	}

	// ���(�����)�� ���� ù��° ��͸� �����մϴ�.
	IDXGIOutput* AdapterOutput = nullptr;
	if (FAILED(Adapter->EnumOutputs(0, &AdapterOutput)))
	{
		return false;
	}

	// ��� (�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ǥ�� ���Ŀ� �´� ��� ���� �����ɴϴ�
	unsigned int NumModes = 0;
	if (FAILED(AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, NULL)))
	{
		return false;
	}

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����մϴ�
	DXGI_MODE_DESC* DisplayModeList = new DXGI_MODE_DESC[NumModes];
	if (!DisplayModeList)
	{
		return false;
	}

	// ���� ���÷��� ��忡 ���� ����Ʈ�� ä��ϴ�
	if (FAILED(AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, DisplayModeList)))
	{
		return false;
	}

	// ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã���ϴ�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����մϴ�.
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

	// ����ī���� ����ü�� ����ϴ�
	DXGI_ADAPTER_DESC AdapterDesc;
	if (FAILED(Adapter->GetDesc(&AdapterDesc)))
	{
		return false;
	}

	// ����ī�� �޸� �뷮 ������ �ް�����Ʈ ������ �����մϴ�
	m_VideoCardMemory = static_cast<int>(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ����ī���� �̸��� �����մϴ�
	size_t StringLength = 0;
	if (wcstombs_s(&StringLength, m_VideoCardDescription, 128, AdapterDesc.Description, 128) != 0)
	{
		return false;
	}
	// ���÷��� ��� ����Ʈ�� �����մϴ�
	delete[] DisplayModeList;
	DisplayModeList = 0;

	// ��� ��͸� �����մϴ�
	AdapterOutput->Release();
	AdapterOutput = nullptr;

	// ��͸� �����մϴ�
	Adapter->Release();
	Adapter = nullptr;

	// ���丮 ��ü�� �����մϴ�
	Factory->Release();
	Factory = nullptr;

	// ����ü�� ����ü�� �ʱ�ȭ�մϴ�
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// ����۸� 1���� ����ϵ��� �����մϴ�
	SwapChainDesc.BufferCount = 1;

	// ������� ���̿� ���̸� �����մϴ�
	SwapChainDesc.BufferDesc.Width  = InScreenWidth;
	SwapChainDesc.BufferDesc.Height = InScreenHeight;

	// 32bit �����̽��� �����մϴ�
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� ���ΰ�ħ ������ �����մϴ�
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

	// ������� ���뵵�� �����մϴ�
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� ���� ������ �ڵ��� �����մϴ�
	SwapChainDesc.OutputWindow = InHwnd;

	// ��Ƽ���ø��� ���ϴ�
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;

	// â��� or Ǯ��ũ�� ��带 �����մϴ�
	if (InFullScreen)
	{
		SwapChainDesc.Windowed = false;
	}
	else
	{
		SwapChainDesc.Windowed = true;
	}

	// ��ĵ ���� ���� �� ũ�⸦ �������� �������� �����մϴ�.
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ���� ����۸� ��쵵�� �����մϴ�
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��׸� ������� �ʽ��ϴ�
	SwapChainDesc.Flags = 0;

	// ��ó������ DirectX 11 �� �����մϴ�
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// ���� ü��, Direct3D ��ġ �� Direct3D ��ġ ���ؽ�Ʈ�� ����ϴ�.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevel, 1,
		D3D11_SDK_VERSION, &SwapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext)))
	{
		return false;
	}

	// ����� �����͸� ���ɴϴ�
	ID3D11Texture2D* BackBufferPtr = nullptr;
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr)))
	{
		return false;
	}

	// �� ���� �����ͷ� ���� Ÿ�� �並 �����Ѵ�.
	if (FAILED(m_Device->CreateRenderTargetView(BackBufferPtr, NULL, &m_RenderTargetView)))
	{
		return false;
	}

	// ����� �����͸� �����մϴ�
	BackBufferPtr->Release();
	BackBufferPtr = nullptr;

	// ���� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// ���� ���� ����ü�� �ۼ��մϴ�
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

	// ������ ���̹��� ����ü�� ����Ͽ� ���� ���� �ؽ��ĸ� �����մϴ�
	if (FAILED(m_Device->CreateTexture2D(&DepthBufferDesc, NULL, &m_DepthStencilBuffer)))
	{
		return false;
	}

	// ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));

	// ���ٽ� ���� ����ü�� �ۼ��մϴ�
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	DepthStencilDesc.StencilEnable = true;
	DepthStencilDesc.StencilReadMask = 0xFF;
	DepthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ� ������ ���ٽ� �����Դϴ�
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ� �޸��� ���ٽ� �����Դϴ�
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���¸� �����մϴ�
	if (FAILED(m_Device->CreateDepthStencilState(&DepthStencilDesc, &m_DepthStencilState)))
	{
		return false;
	}

	// ���� ���ٽ� ���¸� �����մϴ�
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	// ���� ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// ���� ���ٽ� �� ����ü�� �����մϴ�
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �並 �����մϴ�
	if (FAILED(m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &DepthStencilViewDesc, &m_DepthStencilView)))
	{
		return false;
	}

	// ������ ��� ��� ���� ���ٽ� ���۸� ��� ���� ������ ���ο� ���ε��մϴ�
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	// �׷����� ������� ����� ������ ������ ����ü�� �����մϴ�
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

	// ��� �ۼ��� ����ü���� ������ ������ ���¸� ����ϴ�
	if (FAILED(m_Device->CreateRasterizerState(&RasterDesc, &m_RasterState)))
	{
		return false;
	}

	// ���� ������ ������ ���¸� �����մϴ�
	m_DeviceContext->RSSetState(m_RasterState);

	// �������� ���� ����Ʈ�� �����մϴ�
	D3D11_VIEWPORT Viewport;
	Viewport.Width	= static_cast<float>(InScreenWidth);
	Viewport.Height = static_cast<float>(InScreenHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;

	// ����Ʈ�� �����մϴ�
	m_DeviceContext->RSSetViewports(1, &Viewport);

	// ���� ����� �����մϴ�
	float fieldOfView = XM_PI / 4.0f;
	float screenAspect = (float)InScreenWidth / (float)InScreenHeight;

	// 3D ������������ ���� ����� ����ϴ�
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, InScreenNear, InScreenDepth);

	// ���� ����� �׵� ��ķ� �ʱ�ȭ�մϴ�
	m_WorldMatrix = XMMatrixIdentity();

	// 2D ������������ ���� ���� ����� ����ϴ�
	m_OrthoMatrix = XMMatrixOrthographicLH((float)InScreenWidth, (float)InScreenHeight, InScreenNear, InScreenDepth);

	// ���� 2D �������� ���� Z���۸� ���� �� ��° ���� ���ٽ� ���¸� ���� ������ ��������
	// DepthEnable�� false�� �����ϸ� �ٸ� ��� �Ű������� �ٸ� ���� ���Ľ� ���¿� ������.
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

	// ��ġ�� ����Ͽ� ���¸� ����
	if (FAILED(m_Device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState)))
	{
		return false;
	}

	return true;
}

void DirectXManager::Shutdown()
{
	// ���� �� ������ ���� �������� ������ ���� ü���� ���� �� �� ���ܰ� �߻��մϴ�.
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
	// ���۸� ���� ���� �����մϴ�
	float color[4] = { InRed, InGreen, InBlue, InAlpha };

	// ����۸� ����ϴ�
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);

	// ���� ���۸� ����ϴ�
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectXManager::EndScene()
{
	// �������� �Ϸ�Ǿ����Ƿ� ȭ�鿡 �� ���۸� ǥ���մϴ�.
	if (m_VsyncEnabled)
	{
		// ȭ�� ���� ��ħ ������ �����մϴ�.
		m_SwapChain->Present(1, 0);
	}
	else
	{
		// ������ ������ ����մϴ�
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