#include "stdafx.h"
#include "SystemApp.h"
#include "InputManager.h"
#include "GraphicsManager.h"

SystemApp::SystemApp()
{
}


SystemApp::~SystemApp()
{
}

SystemApp::SystemApp(const SystemApp& InOther)
{
}

bool SystemApp::Initialize()
{
	// 윈도우 창 가로, 세로 넓이 변수 초기화
	int ScreenWidth = 0;
	int ScreenHeight = 0;

	// 윈도우 생성 초기화
	InitializeWindows(ScreenWidth, ScreenHeight);

	// m_Input 객체 초기화
	InputManager::GetInstance().Initialize(m_Hinstance, m_Hwnd, ScreenWidth, ScreenHeight);


	m_Graphics = new GraphicsManager;
	if (!m_Graphics)
	{
		return false;
	}

	return m_Graphics->Initialize(ScreenWidth, ScreenHeight, m_Hwnd);
}

void SystemApp::Shutdown()
{
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	InputManager::GetInstance().Shutdown();

	ShutdownWindows();
}

void SystemApp::Run()
{
	// 메시지 구조체 생성 및 초기화
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame())
				break;
		}
	}
}

bool SystemApp::Frame()
{
	if (!InputManager::GetInstance().Frame())
	{
		return false;
	}

	if (InputManager::GetInstance().IsEscapePressed())
	{
		return false;
	}

	return m_Graphics->Frame();
}

LRESULT CALLBACK SystemApp::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 키보드가 눌러졌는가 처리
	case WM_KEYDOWN:
	{
		//m_Input->KeyDown(static_cast<unsigned int>(wparam));
		return 0;
	}

	// 키보드가 떨어졌는가 처리
	case WM_KEYUP:
	{
		//m_Input->KeyUp(static_cast<unsigned int>(wparam));
		return 0;
	}

	// 그 외의 모든 메시지들은 기본 메시지 처리로 넘깁니다.
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SystemApp::InitializeWindows(int& InScreenWidth, int& InScreenHeight)
{
	// 외부 포인터를 이 객체로 지정합니다
	ApplicationHandle = this;

	// 이 프로그램의 인스턴스를 가져옵니다
	m_Hinstance = GetModuleHandle(NULL);

	// 프로그램 이름을 지정합니다
	m_ApplicationName = L"DirectX11_FrameWork";

	// windows 클래스를 아래와 같이 설정합니다.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_Hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_ApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// windows class를 등록합니다
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도를 읽어옵니다
	InScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	InScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	int PosX = 0;
	int PosY = 0;

	// FULL_SCREEN 변수 값에 따라 화면을 설정합니다.
	if (FULL_SCREEN)
	{
		// 풀스크린 모드로 지정했다면 모니터 화면 해상도를 데스크톱 해상도로 지정하고 색상을 32bit로 지정합니다.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)InScreenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)InScreenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린으로 디스플레이 설정을 변경합니다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// 윈도우 모드의 경우 800 * 600 크기를 지정합니다.
		InScreenWidth = 800;
		InScreenHeight = 600;

		// 윈도우 창을 가로, 세로의 정 가운데 오도록 합니다.
		PosX = (GetSystemMetrics(SM_CXSCREEN) - InScreenWidth) / 2;
		PosY = (GetSystemMetrics(SM_CYSCREEN) - InScreenHeight) / 2;
	}

	// 윈도우를 생성하고 핸들을 구합니다.
	m_Hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		PosX, PosY, InScreenWidth, InScreenHeight, NULL, NULL, m_Hinstance, NULL);

	// 윈도우를 화면에 표시하고 포커스를 지정합니다
	ShowWindow(m_Hwnd, SW_SHOW);
	SetForegroundWindow(m_Hwnd);
	SetFocus(m_Hwnd);
}


void SystemApp::ShutdownWindows()
{
	// 풀스크린 모드였다면 디스플레이 설정을 초기화합니다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거합니다
	DestroyWindow(m_Hwnd);
	m_Hwnd = NULL;

	// 프로그램 인스턴스를 제거합니다
	UnregisterClass(m_ApplicationName, m_Hinstance);
	m_Hinstance = NULL;

	// 외부포인터 참조를 초기화합니다
	ApplicationHandle = NULL;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}