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
	// ������ â ����, ���� ���� ���� �ʱ�ȭ
	int ScreenWidth = 0;
	int ScreenHeight = 0;

	// ������ ���� �ʱ�ȭ
	InitializeWindows(ScreenWidth, ScreenHeight);

	// m_Input ��ü �ʱ�ȭ
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
	// �޽��� ����ü ���� �� �ʱ�ȭ
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
		// Ű���尡 �������°� ó��
	case WM_KEYDOWN:
	{
		//m_Input->KeyDown(static_cast<unsigned int>(wparam));
		return 0;
	}

	// Ű���尡 �������°� ó��
	case WM_KEYUP:
	{
		//m_Input->KeyUp(static_cast<unsigned int>(wparam));
		return 0;
	}

	// �� ���� ��� �޽������� �⺻ �޽��� ó���� �ѱ�ϴ�.
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SystemApp::InitializeWindows(int& InScreenWidth, int& InScreenHeight)
{
	// �ܺ� �����͸� �� ��ü�� �����մϴ�
	ApplicationHandle = this;

	// �� ���α׷��� �ν��Ͻ��� �����ɴϴ�
	m_Hinstance = GetModuleHandle(NULL);

	// ���α׷� �̸��� �����մϴ�
	m_ApplicationName = L"DirectX11_FrameWork";

	// windows Ŭ������ �Ʒ��� ���� �����մϴ�.
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

	// windows class�� ����մϴ�
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �о�ɴϴ�
	InScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	InScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	int PosX = 0;
	int PosY = 0;

	// FULL_SCREEN ���� ���� ���� ȭ���� �����մϴ�.
	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ���� �����ߴٸ� ����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� �����ϰ� ������ 32bit�� �����մϴ�.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)InScreenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)InScreenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ������ ���÷��� ������ �����մϴ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// ������ ����� ��� 800 * 600 ũ�⸦ �����մϴ�.
		InScreenWidth = 800;
		InScreenHeight = 600;

		// ������ â�� ����, ������ �� ��� ������ �մϴ�.
		PosX = (GetSystemMetrics(SM_CXSCREEN) - InScreenWidth) / 2;
		PosY = (GetSystemMetrics(SM_CYSCREEN) - InScreenHeight) / 2;
	}

	// �����츦 �����ϰ� �ڵ��� ���մϴ�.
	m_Hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		PosX, PosY, InScreenWidth, InScreenHeight, NULL, NULL, m_Hinstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �����մϴ�
	ShowWindow(m_Hwnd, SW_SHOW);
	SetForegroundWindow(m_Hwnd);
	SetFocus(m_Hwnd);
}


void SystemApp::ShutdownWindows()
{
	// Ǯ��ũ�� ��忴�ٸ� ���÷��� ������ �ʱ�ȭ�մϴ�.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����մϴ�
	DestroyWindow(m_Hwnd);
	m_Hwnd = NULL;

	// ���α׷� �ν��Ͻ��� �����մϴ�
	UnregisterClass(m_ApplicationName, m_Hinstance);
	m_Hinstance = NULL;

	// �ܺ������� ������ �ʱ�ȭ�մϴ�
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